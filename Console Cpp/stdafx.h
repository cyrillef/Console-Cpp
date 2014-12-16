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

#include "windows.h"
#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <conio.h>

#include <iostream>
#include <string>
#include <mutex>
#include <functional>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <chrono>
#include <ctime>
#include <memory>

#include <sys/types.h>
#include <sys/stat.h>

// C++ REST SDK (codename "Casablanca")
// https://casablanca.codeplex.com/
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/http_listener.h>          // HTTP server
#include <cpprest/json.h>                   // JSON library
#include <cpprest/uri.h>                    // URI library
#include <cpprest/http_helpers.h>
#include <cpprest/oauth1.h>

using namespace utility ;                   // Common utilities like string conversions
using namespace web ;                       // Common features like URIs.
using namespace web::http ;                 // Common HTTP functionality
using namespace web::http::client ;         // HTTP client features
using namespace concurrency::streams ;      // Asynchronous streams
using namespace web::http::experimental::listener ; // HTTP server
using namespace web::json ;                 // JSON library
using namespace pplx ;
using namespace utility ;
using namespace web::http::oauth1::experimental ;

// http://www.codeproject.com/Articles/157001/Full-getopt-Port-for-Unicode-and-Multibyte-Microso
#include "getopt.h"

// Project
#include "UserSettings.h"
