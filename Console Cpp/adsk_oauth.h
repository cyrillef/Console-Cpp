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

#include <cpprest/details/basic_types.h>
#include <cpprest/oauth1.h>

#define CALLBACK_URL U("http://localhost:8089/json")

bool DeleteTokens () ;
bool ReadTokens (utility::string_t &token, utility::string_t &secret, utility::string_t &session) ;
bool SaveTokens (utility::string_t token, utility::string_t secret, utility::string_t session) ;

int oAuthLogonAsync (oauth1_config &config) ;
int oAuthRefreshTokensAsync (oauth1_config &config) ;

#define oauth_session_handle U("oauth_session_handle")
#define oauth_authorization_expires_in U("oauth_authorization_expires_in")
#define oauth_expires_in U("oauth_expires_in")
#define x_oauth_user_name U("x_oauth_user_name")
#define x_oauth_user_guid U("x_oauth_user_guid")
#define x_scope U("x_scope")
