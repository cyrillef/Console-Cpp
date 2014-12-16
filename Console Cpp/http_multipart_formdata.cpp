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
#include "http_multipart_formdata.h"
#include "adsk_utils.h"

http_multipart_formdata::http_multipart_formdata () {
}

void http_multipart_formdata::initialize (web::http::http_request &request) {
	nonce_generator gen (8) ;
	_boundary =gen.generate () ;
	request.headers ().add (header_names::content_type, openBodyContentType (_boundary)) ;
	request.headers ().add (header_names::accept_charset, web::http::details::charset_types::utf8) ;
}

utility::string_t http_multipart_formdata::openBodyContentType (utility::string_t boundary) {
	utility::string_t st =
		  web::http::details::mime_types::multipart_form_data + U("; ")
		+ web::http::details::mime_types::boundary + U("=")
		+ boundary ;
	return (st) ;
}

void http_multipart_formdata::addParameter (utility::string_t key, utility::string_t value) {
	addParameter (_ostr, _boundary, key, web::uri::encode_data_string (value)) ;
}

void http_multipart_formdata::addParameters (std::map<utility::string_t, utility::string_t> &parameters) {
	for ( const auto &param : parameters )
		addParameter (_ostr, _boundary, param.first, web::uri::encode_data_string (param.second)) ;
}

void http_multipart_formdata::addFile (utility::string_t filename, int index) {
	wchar_t key [50] ;
	wsprintf (key, U("file[%d]"), index) ;
	//std::vector<wchar_t> drive (8) ;
	//std::vector<wchar_t> dir (MAX_PATH) ;
	std::vector<wchar_t> fname (MAX_PATH) ;
	std::vector<wchar_t> ext (MAX_PATH) ;
	_wsplitpath_s (filename.c_str (), nullptr, 0, nullptr, 0, fname.data (), fname.size (), ext.data (), ext.size ()) ;

	std::wstring fn (&fname.front ()) ;
	fn +=std::wstring (&ext.front ()) ;
	Concurrency::streams::file_stream<byte>::open_istream (filename)
		.then ([&] (task<Concurrency::streams::basic_istream<byte> > previousTask) {
			try {
				auto fileStream =previousTask.get () ;
				fileStream.seek (0, std::ios::end) ;
				auto length =static_cast<size_t> (fileStream.tell ()) ;
				fileStream.seek (0, std::ios_base::beg) ;

				addChunk (_ostr, _boundary, key, fn, fileStream) ;
				//req.set_body (fileStream) ;
			} catch ( const http_exception &e ) {
				ucout << U("Error: ") << e.what () << std::endl ;
			}
	}).wait () ;
}

void http_multipart_formdata::addFiles (std::vector<utility::string_t> &files) {
	int index =0 ;
	for ( const auto &filename : files )
		addFile (filename, index++) ;
}

void http_multipart_formdata::closeBody (web::http::http_request &request) {
	closeBody (_ostr, _boundary) ;

	_ostr.seekg (0, std::ios::end) ;
	std::streampos length (_ostr.tellg ()) ;
	std::vector<unsigned char> body (static_cast<std::size_t> (length)) ;
	_ostr.seekg (0, std::ios_base::beg) ;
	std::string const &st =_ostr.str () ;
	body.assign (st.begin (), st.end ()) ;
	request.set_body (body) ;
}

void http_multipart_formdata::addParameter (std::stringstream &ostr, utility::string_t boundary, utility::string_t name, utility::string_t value) {
	ostr << "--" << ws2s (boundary.c_str ())
		 << "\r" << std::endl ;
	ostr << ws2s (web::http::header_names::content_disposition.c_str ()) << ": "
		 << ws2s (web::http::details::mime_types::form_data.c_str ())
		 << "; name=\"" << ws2s (name.c_str ()) << "\""
		 << "\r" << std::endl ;
	ostr << "\r" << std::endl ;
	ostr << ws2s (value.c_str ())
		 << "\r" << std::endl ;
}

void http_multipart_formdata::addParameter (std::wstringstream &ostr, utility::string_t boundary, utility::string_t name, utility::string_t value) {
	ostr << U("--") << boundary.c_str ()
		 << U("\r") << std::endl ;
	ostr << web::http::header_names::content_disposition.c_str () << U(": ")
		 << web::http::details::mime_types::form_data.c_str ()
		 << U("; name=\"") << name.c_str () << U("\"")
		 << U("\r") << std::endl ;
	ostr << U("\r") << std::endl ;
	ostr << value.c_str ()
		 << U("\r") << std::endl ;
}

void http_multipart_formdata:: addChunk (std::stringstream &ostr, utility::string_t boundary, utility::string_t name, utility::string_t filename, Concurrency::streams::basic_istream<byte> &chunk) {
	ostr << "--" << ws2s (boundary.c_str ())
		 << "\r" << std::endl ;
	ostr << ws2s (::http::header_names::content_disposition.c_str ()) << ": "
		 << ws2s (web::http::details::mime_types::form_data.c_str ())
		 << "; name=\"" << ws2s (name.c_str ()) << "\""
		 << "; filename=\"" << ws2s (filename.c_str ()) << "\""
		 << "\r" << std::endl ;
	ostr << ws2s (web::http::header_names::content_type.c_str ()) << ": "
		 << ws2s (web::http::details::mime_types::application_octetstream.c_str ())
		 << "\r" << std::endl ;
	ostr << "\r" << std::endl ;

	chunk.seek (0, std::ios_base::end) ;
	int length =(int)chunk.tell() ;
	chunk.seek (0, std::ios_base::beg) ;

	Concurrency::streams::streambuf<unsigned char> buff (chunk.streambuf ()) ;
	std::vector<uint8_t> v8 ;
	v8.resize (length) ;
	buff.seekpos (0, std::ios_base::beg) ;
	buff.getn (&v8.front (), length) ;
	std::vector<uint8_t>::iterator it =v8.begin () ;
	for ( ; it != v8.end () ; ++it )
		ostr << (byte)*it ;
	ostr << "\r" << std::endl ;
}

void http_multipart_formdata::addChunk (std::wstringstream &ostr, utility::string_t boundary, utility::string_t name, utility::string_t filename, Concurrency::streams::basic_istream<unsigned char> &chunk) {
	ostr << U("--") << boundary.c_str ()
		 << U("\r") << std::endl ;
	ostr << web::http::header_names::content_disposition.c_str () << U(": ")
		 << web::http::details::mime_types::form_data.c_str ()
		 << U("; name=\"") << name.c_str () << U("\"")
		 << U("; filename=\"") << filename.c_str () << U("\"")
		 << U("\r") << std::endl ;
	ostr << web::http::header_names::content_type.c_str () << U(": ")
		 << web::http::details::mime_types::application_octetstream.c_str ()
		 << U("\r") << std::endl ;
	ostr << U("\r") << std::endl ;
	ostr << chunk
		 << U("\r") << std::endl ;
}

void http_multipart_formdata::closeBody (std::stringstream &ostr, utility::string_t boundary) {
	ostr << "--" << ws2s (boundary.c_str ()) << "--" ;
}

void http_multipart_formdata::closeBody (std::wstringstream &ostr, utility::string_t boundary) {
	ostr << U("--") << boundary.c_str () << U("--") ;
}
