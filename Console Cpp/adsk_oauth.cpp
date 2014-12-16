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
#include "adsk_utils.h"
#include "adsk_oauth.h"

bool DeleteTokens () {
	WCHAR buffer [MAX_PATH] ;
	::GetModuleFileName (NULL, buffer, MAX_PATH) ;
	std::wstring::size_type pos =std::wstring (buffer).find_last_of (U("\\/")) ;
	std::wstring path =std::wstring (buffer).substr (0, pos + 1) ;
	return (::DeleteFile ((path + U("oauth.txt")).c_str ()) != FALSE) ;
}

bool ReadTokens (utility::string_t &token, utility::string_t &secret, utility::string_t &session) {
	WCHAR buffer [MAX_PATH] ;
	::GetModuleFileName (NULL, buffer, MAX_PATH) ;
	std::wstring::size_type pos =std::wstring (buffer).find_last_of (U("\\/")) ;
	std::wstring path =std::wstring (buffer).substr (0, pos + 1) ;
	std::wifstream in_f (path + U("oauth.txt"), std::ios::in) ;
	if ( !in_f.is_open () )
		return (false) ;
	in_f 
		>> token 
		>> secret
		>> session ;
	in_f.close () ;
	return (true) ;
}

bool SaveTokens (utility::string_t token, utility::string_t secret, utility::string_t session) {
	WCHAR buffer [MAX_PATH] ;
	::GetModuleFileName (NULL, buffer, MAX_PATH) ;
	std::wstring::size_type pos =std::wstring (buffer).find_last_of (U("\\/")) ;
	std::wstring path =std::wstring (buffer).substr (0, pos + 1) ;
	std::wofstream out (path + U("oauth.txt"), std::ios::out) ;
	if ( !out.is_open () )
		return (false) ;
	out 
		<< token.c_str () << std::endl 
		<< secret.c_str () << std::endl 
		<< session.c_str () << std::endl ;
	out.close () ;
	return (true) ;
}

int oAuthLogonAsync (oauth1_config &config) {
	int retCode =0 ;
	try {
		DeleteTokens () ;

		//- 1st leg: Get the 'request token'
		ucout << U("1st leg: Get the 'request token'...") << std::endl ;
		auto auth_uri_task (config.build_authorization_uri ()) ;
		auto auth_uri (auth_uri_task.get ()) ;

		//- 2nd leg: Authorize the token
		ucout << U("2nd leg: Authorize the token...") << std::endl ;
		try {
			utility::string_t oauth_verifier ;
			bool bWait =true ;
			std::unique_ptr<http_listener> listener (new http_listener (CALLBACK_URL)) ;
			// Lambda Expression Syntax - http://msdn.microsoft.com/en-us/library/dd293603.aspx
			listener->support (methods::GET, [&/*&_config, &bWait, &oauth_verifier*/] (http_request request) -> void {
				// http://localhost:8089/json?oauth_verifier=XXXX&oauth_token=XXXX

				auto http_get_vars =uri::split_query (request.request_uri ().query ()) ;
				auto oauth_verifierIter =http_get_vars.find (U("oauth_verifier")) ;
				if ( oauth_verifierIter == end (http_get_vars) ) {
					auto err =U("Error: No oauth_verifier parameter received!") ;
					ucout << err << std::endl ;
					request.reply (status_codes::BadRequest, err) ;
					retCode =4 ;
					return ;
				}
				auto oauth_tokenIter =http_get_vars.find (U("oauth_token")) ;
				if ( oauth_tokenIter == end (http_get_vars) ) {
					auto err =U("Error: No oauth_token parameter received!") ;
					ucout << err << std::endl ;
					request.reply (status_codes::BadRequest, err) ;
					retCode =5 ;
					return ;
				}

				oauth_verifier =web::uri::decode (oauth_verifierIter->second) ;
				request.reply (status_codes::OK, U("You are authorized!\nYou can close this window.")) ;
				ucout << U("\tYou are authorized!\n\tYou can close this window.") << std::endl ;

				config.token_from_verifier (oauth_verifier).then ([&] () -> void {
					auto token =config.token ().access_token () ;
					auto secret =config.token ().secret () ;
					auto session =config.token ().session () ;
					ucout << U("oauth_token => ") << token << U(",") << std::endl
						  << U("oauth_token_secret => ") << secret << std::endl
						  << U("oauth_session_handle => ") << session << std::endl ;
					SaveTokens (token, secret, session) ;

					bWait =false ;
				}).wait () ;

			}) ;
			open_browser (auth_uri) ;
			listener->open () ;

			// Wait while the listener waits for the oAuth callback.
			ucout << U("Please, authorize the token in your browser\n\tWaiting for incoming connection...") << std::endl ;
			while ( bWait ) {
				std::this_thread::sleep_for (std::chrono::milliseconds (2000)) ;
				if ( _kbhit () ) {
					ucout << U("Cancelled by user, Aborting...") << std::endl ;
					listener->close () ;
					return (1) ;
				}
			}
 			// Nothing left to do but commit suicide - Terminating the listener.
			listener->close () ;
		} catch (const oauth1_exception &e) {
			ucout << U("Error: ") << e.what () << std::endl ;
			return (2) ;
		}
	} catch (const oauth1_exception &e) {
		ucout << U("Error: ") << e.what () << std::endl ;
		return (3) ;
	}
	return (retCode) ;
}

int oAuthRefreshTokensAsync (oauth1_config &config) {
	try {
		//- Refresh the tokens
		ucout << U("Refresh the tokens...") << std::endl ;
		config.refresh_token ().then ([&] () -> void {
			auto token =config.token ().access_token () ;
			auto secret =config.token ().secret () ;
			auto session =config.token ().session () ;
			ucout << U("oauth_token => ") << token << U(",") << std::endl
				  << U("oauth_token_secret => ") << secret << std::endl
				  << U("oauth_session_handle => ") << session << std::endl ;
			SaveTokens (token, secret, session) ;
		}).wait () ;
	} catch (const oauth1_exception &e) {
		ucout << U("Error: ") << e.what () << std::endl ;
		return (1) ;
	}
	return (0) ;
}