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

// Hard coded consumer and secret keys and base URL.
// In real world Apps, these values need to secured.
// One approach is to encrypt and/or obfuscate these values
#define CONSUMER_KEY U("your_consumer_key")
#define CONSUMER_SECRET U("your_consumer_secret_key")
#define OAUTH_HOST U("https://accounts.autodesk.com/") // Autodesk production accounts server;
//#define OAUTH_HOST U("https://accounts-staging.autodesk.com/") // Autodesk staging accounts server

// ReCap: Fill in these macros with the correct information (only the 2 first are important)
#define ReCapAPIURL U("http://rc-api-adn.autodesk.com/3.1/API/")
#define ReCapClientID U("your_ReCap_client_ID")
//#define ReCapKey U("your ReCap client key") // not used anymore
#define ReCapUserID U("your_ReCap_user_ID") // Needed only for using the ReCapSceneList, otherwise bail

#define Email U("your email address") // used for notification

// Do not edit
#define OAUTH_REQUESTTOKEN OAUTH_HOST U("OAuth/RequestToken")
#define OAUTH_ACCESSTOKEN OAUTH_HOST U("OAuth/AccessToken")
#define OAUTH_AUTHORIZE OAUTH_HOST U("OAuth/Authorize")
#define OAUTH_INVALIDATETOKEN OAUTH_HOST U("OAuth/InvalidateToken")
#define OAUTH_ALLOW OAUTH_HOST U("OAuth/Allow")
