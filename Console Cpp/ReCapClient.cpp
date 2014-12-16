//
// Copyright (c) Autodesk, Inc. All rights reserved 
//
// C++ Autodesk ReCap Console Sample
// by Cyrille Fauvel - Autodesk Developer Network (ADN)
// September 2014
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
#include "stdafx.h"
#include "ReCapClient.h"
#include "http_multipart_formdata.h"

bool ReCapClient::Time (bool json) {
	this->get (U("/service/date"), json).wait () ;
	return (isOk ()) ;
}

bool ReCapClient::Version (bool json) {
	this->get (U("/version"), json).wait () ;
	return (isOk ()) ;
}

bool ReCapClient::SetNotificationMessage (utility::string_t emailType, utility::string_t emailTxt, bool json) {
	ReCapParams parameters ;
	parameters [U("emailType")] =emailType ;
	parameters [U("emailTxt")] =emailTxt ;
	this->postUrlEncoded (U("notification/template"), parameters, json).wait () ;
	return (isOk ()) ;
}

bool ReCapClient::CreateSimplePhotoscene (utility::string_t format, utility::string_t meshQuality, bool json) {
	utility::ostringstream_t os ;
	os << utility::datetime::utc_timestamp () ;

	ReCapParams parameters ;
	parameters [U("scenename")] =utility::string_t (U("MyPhotoScene")) + os.str () ;
	parameters [U("format")] =format ;
	parameters [U("meshQuality")] =meshQuality ;
	parameters [U("callback")] =Email ;
	this->postUrlEncoded (U("photoscene"), parameters, json).wait () ;
	return (isOk ()) ;
}

bool ReCapClient::CreatePhotoscene (utility::string_t format, utility::string_t meshQuality, ReCapParams &options, bool json) {
	utility::ostringstream_t os ;
	os << utility::datetime::utc_timestamp () ;

	options [U("scenename")] =utility::string_t (U("MyPhotoScene")) + os.str () ;
	options [U("format")] =format ;
	options [U("meshQuality")] =meshQuality ;
	options [U("callback")] =Email ;
	this->postUrlEncoded (U("photoscene"), options, json).wait () ;
	return (isOk ()) ;
}

bool ReCapClient::SceneList (utility::string_t attributeName, utility::string_t attributeValue, bool json) {
	ReCapParams parameters ;
	parameters [U("attributeName")] =attributeName ;
	parameters [U("attributeValue")] =attributeValue ;

	this->get (U("/photoscene/properties"), parameters, json).wait () ;
	return (isOk ()) ;
}

bool ReCapClient::SceneProperties (utility::string_t photosceneid, bool json) {
	this->get (U("photoscene/") + photosceneid + U("/properties"), json).wait () ;
	return (isOk ()) ;
}

bool ReCapClient::UploadFiles (utility::string_t photosceneid, std::vector<utility::string_t> &photos, bool json) {
	ReCapParams parameters ;
	parameters [U("photosceneid")] =photosceneid ;
	this->postMultipart (U("file"), parameters, photos, json).wait () ;
	return (isOk ()) ;
}

bool ReCapClient::ProcessScene (utility::string_t photosceneid, bool json) {
	ReCapParams parameters ;
	parameters [U("forceReprocess")] =U("1") ;
	this->postUrlEncoded (U("photoscene/") + photosceneid, parameters, json).wait () ;
	return (isOk ()) ;
}

bool ReCapClient::SceneProgress (utility::string_t photosceneid, bool json) {
	ReCapParams parameters ;
	this->get (U("photoscene/") + photosceneid + U("/progress"), json).wait () ;
	return (isOk ()) ;
}

bool ReCapClient::GetPointCloudArchive (utility::string_t photosceneid, utility::string_t format, bool json) {
	ReCapParams parameters ;
	parameters [U("format")] =format ;
	this->get (U("/photoscene/") + photosceneid, parameters, json).wait () ;
	return (isOk ()) ;
}

void ReCapClient::DownloadFile (utility::string_t scenelink, utility::string_t saveTo) {
	RequestFileStreamAsync (scenelink, saveTo) ;
}

bool ReCapClient::DeleteScene (utility::string_t photosceneid, bool json) {
	ReCapParams parameters ;
	this->delete_ (U("/photoscene/") + photosceneid, parameters, json).wait () ;
	return (isOk ()) ;
}

bool ReCapClient::isOk () {
	if ( _lastResponse == json::value::null () )
		return (false) ;
	if ( _lastCode < 200 || _lastCode >= 300 )
		return (false) ;
	return (!_lastResponse.has_field (U("error")) && ! _lastResponse.has_field (U("Error"))) ;
}

pplx::task<void> ReCapClient::get (utility::string_t path, ReCapParams &parameters, bool json) {
	parameters [U("clientID")] =_recapClientID ;
	parameters [json == true ? U("json") : U("xml")] =U("1") ;

	http::uri uri =http::uri_builder (path).to_uri () ;
	for ( const auto &param : parameters ) {
		uri =http::uri_builder (uri)
			.append_query (param.first, web::uri::encode_data_string (param.second), false)
			.to_uri () ;
	}

	web::http::http_request request (web::http::methods::GET) ;
	request.set_request_uri (uri) ;
	if ( json == true )
		request.headers ().add (header_names::accept, web::http::details::mime_types::application_json) ;

	return (_recapClient->request (request).then ([this, path] (http_response response) /*-> pplx::task<json::value>*/ {
		response.content_ready ().wait () ;
		if ( _debug )
			ucout << path << U(" raw response: ") << std::endl
				  << U("------") << std::endl
				  << response.to_string () << std::endl
				  << U("======") << std::endl << std::endl ;
		if ( (_lastCode =response.status_code ()) == status_codes::OK ) {
			// Bug in ReCap, it returns as text/html
			return (response.extract_json (true)) ;
		}
		// Handle error cases, for now return empty json value... 
		return (pplx::task_from_result (json::value ())) ;
	}).then ([this] (pplx::task<json::value> json) {
		try {
			_lastResponse =json.get () ;
		} catch ( const http_exception &e ) {
			_lastResponse =json::value::null () ;
			ucout << U("Error: ") << e.what () << std::endl ; // Print error.
		}
	})) ;
}

pplx::task<void> ReCapClient::postUrlEncoded (utility::string_t path, ReCapParams &parameters, bool json) {
	parameters [U("clientID")] =_recapClientID ;
	parameters [json == true ? U("json") : U("xml")] =U("1") ;

	http::uri uri =http::uri_builder (U("")).to_uri () ;
	for ( const auto &param : parameters ) {
		uri =http::uri_builder (uri)
			.append_query (param.first, web::uri::encode_data_string (param.second), false)
			.to_uri () ;
	}

	web::http::http_request request (web::http::methods::POST) ;
	request.set_request_uri (http::uri_builder (path).to_uri ()) ;
	request.set_body (uri.to_string ().substr (2), web::http::details::mime_types::application_x_www_form_urlencoded) ;
	if ( json == true )
		request.headers ().add (header_names::accept, web::http::details::mime_types::application_json) ;

	return (_recapClient->request (request).then ([this, path] (http_response response) /*-> pplx::task<json::value>*/ {
		response.content_ready ().wait () ;
		if ( _debug )
			ucout << path << U(" raw response: ") << std::endl
				  << U("------") << std::endl
				  << response.to_string () << std::endl
				  << U("======") << std::endl << std::endl ;
		if ( (_lastCode =response.status_code ()) == status_codes::OK ) {
			// Bug in ReCap, it returns as text/html
			return (response.extract_json (true)) ;
		}
		// Handle error cases, for now return empty json value... 
		return (pplx::task_from_result (json::value ())) ;
	}).then ([this] (pplx::task<json::value> json) {
		try {
			_lastResponse =json.get () ;
		} catch ( const http_exception &e ) {
			_lastResponse =json::value::null () ;
			ucout << U("Error: ") << e.what () << std::endl ; // Print error.
		}
	})) ;
}

pplx::task<void> ReCapClient::postMultipart (utility::string_t path, ReCapParams &parameters, std::vector<utility::string_t> &photos, bool json) {
	parameters [U("clientID")] =_recapClientID ;
	parameters [json == true ? U("json") : U("xml")] =U("1") ;
	parameters [U("type")] =U("image") ;

	web::http::http_request request (web::http::methods::POST) ;
	request.set_request_uri (http::uri_builder (path).to_uri ()) ;
	if ( json == true )
		request.headers ().add (header_names::accept, web::http::details::mime_types::application_json) ;

	http_multipart_formdata bodydata ;
	bodydata.initialize (request) ;
	bodydata.addParameters (parameters) ;
	bodydata.addFiles (photos) ;
	bodydata.closeBody (request) ;
	
	oauth1_config access (_access) ;
	access.set_parameters (parameters) ;
	_config.set_oauth1 (access) ;
	std::unique_ptr<http_client> recapClient =std::unique_ptr<http_client> (new http_client (ReCapAPIURL, _config)) ;

	return (recapClient->request (request).then ([this, path] (http_response response) /*-> pplx::task<json::value>*/ {
		response.content_ready ().wait () ;
		_config.set_oauth1 (_access) ;
		if ( _debug )
			ucout << path << U(" raw response: ") << std::endl
				  << U("------") << std::endl
				  << response.to_string () << std::endl
				  << U("======") << std::endl << std::endl ;
		if ( (_lastCode =response.status_code ()) == status_codes::OK ) {
			// Bug in ReCap, it returns as text/html
			return (response.extract_json (true)) ;
		}
		// Handle error cases, for now return empty json value... 
		return (pplx::task_from_result (json::value ())) ;
	}).then ([this] (pplx::task<json::value> json) {
		try {
			_config.set_oauth1 (_access) ;
			_lastResponse =json.get () ;
		} catch ( const http_exception &e ) {
			_lastResponse =json::value::null () ;
			ucout << U("Error: ") << e.what () << std::endl ; // Print error.
		}
	})) ;
}

pplx::task<void> ReCapClient::delete_ (utility::string_t path, ReCapParams &parameters, bool json) {
	parameters [U("clientID")] =_recapClientID ;
	parameters [json == true ? U("json") : U("xml")] =U("1") ;

	web::http::http_request request (web::http::methods::DEL) ;
	request.set_request_uri (http::uri_builder (path).to_uri ()) ;
	if ( json == true )
		request.headers ().add (header_names::accept, web::http::details::mime_types::application_json) ;

	http_multipart_formdata bodydata ;
	bodydata.initialize (request) ;
	bodydata.addParameters (parameters) ;
	bodydata.closeBody (request) ;
	
	oauth1_config access (_access) ;
	access.set_parameters (parameters) ;
	_config.set_oauth1 (access) ;
	std::unique_ptr<http_client> recapClient =std::unique_ptr<http_client> (new http_client (ReCapAPIURL, _config)) ;

	return (recapClient->request (request).then ([this, path] (http_response response) /*-> pplx::task<json::value>*/ {
		response.content_ready ().wait () ;
		_config.set_oauth1 (_access) ;
		if ( _debug )
			ucout << path << U(" raw response: ") << std::endl
				  << U("------") << std::endl
				  << response.to_string () << std::endl
				  << U("======") << std::endl << std::endl ;
		if ( (_lastCode =response.status_code ()) == status_codes::OK ) {
			// Bug in ReCap, it returns as text/html
			return (response.extract_json (true)) ;
		}
		// Handle error cases, for now return empty json value... 
		return (pplx::task_from_result (json::value ())) ;
	}).then ([this] (pplx::task<json::value> json) {
		try {
			_config.set_oauth1 (_access) ;
			_lastResponse =json.get () ;
		} catch ( const http_exception &e ) {
			_lastResponse =json::value::null () ;
			ucout << U("Error: ") << e.what () << std::endl ;
		}
	})) ;
}

void ReCapClient::RequestFileStreamAsync (utility::string_t scenelink, utility::string_t saveTo) {
	// Open a stream to the file to write the HTTP response body into.
	auto fileBuffer =std::make_shared<streambuf<uint8_t>> () ;
	file_buffer<uint8_t>::open (saveTo, std::ios::out).then ([=] (streambuf<uint8_t> outFile) -> pplx::task<http_response> {
		*fileBuffer =outFile ;
		http_client client (scenelink) ;
		return client.request (methods::GET) ;
	}).then ([=] (http_response response) -> pplx::task<size_t> { // Write the response body into the file buffer.
		return (response.body ().read_to_end (*fileBuffer)) ;
	}).then ([=] (size_t) { // Close the file buffer.
		return (fileBuffer->close ()) ;
	}).wait () ; // Wait for the entire response body to be written into the file.
}

bool ReCapClient::DeletePhotosceneId () {
	WCHAR buffer [MAX_PATH] ;
	::GetModuleFileName (NULL, buffer, MAX_PATH) ;
	std::wstring::size_type pos =std::wstring (buffer).find_last_of (U("\\/")) ;
	std::wstring path =std::wstring (buffer).substr (0, pos + 1) ;
	return (::DeleteFile ((path + U("recap.txt")).c_str ()) != FALSE) ;
}

bool ReCapClient::ReadPhotosceneId (utility::string_t &photosceneid) {
	WCHAR buffer [MAX_PATH] ;
	::GetModuleFileName (NULL, buffer, MAX_PATH) ;
	std::wstring::size_type pos =std::wstring (buffer).find_last_of (U("\\/")) ;
	std::wstring path =std::wstring (buffer).substr (0, pos + 1) ;
	std::wifstream in_f (path + U("recap.txt"), std::ios::in) ;
	if ( !in_f.is_open () )
		return (false) ;
	in_f >> photosceneid ;
	in_f.close () ;
	return (true) ;
}

bool ReCapClient::SavePhotosceneId (utility::string_t photosceneid) {
	WCHAR buffer [MAX_PATH] ;
	::GetModuleFileName (NULL, buffer, MAX_PATH) ;
	std::wstring::size_type pos =std::wstring (buffer).find_last_of (U("\\/")) ;
	std::wstring path =std::wstring (buffer).substr (0, pos + 1) ;
	std::wofstream out (path + U("recap.txt"), std::ios::out) ;
	if ( !out.is_open () )
		return (false) ;
	out << photosceneid.c_str () << std::endl ;
	out.close () ;
	return (true) ;
}
