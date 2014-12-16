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
#pragma once

class ReCapClient {

public:
	typedef std::map<utility::string_t, utility::string_t> ReCapParams ;

protected:
	std::wstring _recapClientID ;
	oauth1_config _access ;
	std::unique_ptr<http_client> _recapClient ;
	http_client_config _config ;

	web::http::status_code _lastCode ;
	json::value _lastResponse ;

	bool _debug ;

public:
	ReCapClient (LPCWSTR recapClientID, oauth1_config &access, bool proxy =false) : _recapClientID(recapClientID), _access(access), _debug(false) {
		_config.set_oauth1 (_access) ;
		if ( proxy == true ) {
			web::web_proxy proxy (L"http://127.0.0.1:8888") ;
			_config.set_proxy (proxy) ;
		}
		_recapClient =std::unique_ptr<http_client> (new http_client (ReCapAPIURL, _config)) ;
	}

	http_client *newClient (oauth1_config &access) {
		http_client_config config (_config) ;
		config.set_oauth1 (access) ;
		return (new http_client (ReCapAPIURL, config)) ;
	}

	bool Time (bool json =true) ;
	bool Version (bool json =true) ;
	bool SetNotificationMessage (utility::string_t emailType, utility::string_t emailTxt, bool json =true) ;
	bool CreateSimplePhotoscene (utility::string_t format, utility::string_t meshQuality, bool json =true) ;
	bool CreatePhotoscene (utility::string_t format, utility::string_t meshQuality, ReCapParams &options, bool json =true) ;
	bool SceneList (utility::string_t attributeName, utility::string_t attributeValue, bool json =true) ;
	bool SceneProperties (utility::string_t photosceneid, bool json =true) ;
	bool UploadFiles (utility::string_t photosceneid, std::vector<utility::string_t> &photos, bool json =true) ;
	bool ProcessScene (utility::string_t photosceneid, bool json =true) ;
	bool SceneProgress (utility::string_t photosceneid, bool json =true) ;
	bool GetPointCloudArchive (utility::string_t photosceneid, utility::string_t format, bool json =true) ;
	void DownloadFile (utility::string_t scenelink, utility::string_t saveTo) ;
	bool DeleteScene (utility::string_t photosceneid, bool json =true) ;

	json::value &json () { return (_lastResponse) ; }
	bool isOk () ;

	bool &debug () { return (_debug) ; }

	static bool DeletePhotosceneId () ;
	static bool ReadPhotosceneId (utility::string_t &photosceneid) ;
	static bool SavePhotosceneId (utility::string_t photosceneid) ;

protected:
	pplx::task<void> get (utility::string_t path, bool json =true) {
		ReCapParams parameters ;
		return (get (path, parameters, json)) ;
	}
	pplx::task<void> get (utility::string_t path, ReCapParams &parameters, bool json =true) ;
	pplx::task<void> postUrlEncoded (utility::string_t path, ReCapParams &parameters, bool json =true) ;
	pplx::task<void> postMultipart (utility::string_t path, ReCapParams &parameters, std::vector<utility::string_t> &photos, bool json =true) ;
	pplx::task<void> delete_ (utility::string_t path, ReCapParams &parameters, bool json =true) ;
	void RequestFileStreamAsync (utility::string_t scenelink, utility::string_t saveTo) ;

} ;
