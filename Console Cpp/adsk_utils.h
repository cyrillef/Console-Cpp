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

#include <cpprest/basic_types.h>
#include <cpprest/filestream.h>

typedef std::basic_stringstream<unsigned char, std::char_traits<unsigned char>,
	std::allocator<unsigned char> > ustringstream ;

std::vector<utility::string_t> &split (const utility::string_t &s, wchar_t delim, std::vector<utility::string_t> &elems) ;
std::vector<utility::string_t> split (const utility::string_t &s, wchar_t delim) ;
std::string ws2s (const std::wstring &wstr) ;
std::string wchar2string (const wchar_t *text) ;

void GetFilesInDirectory (std::vector<utility::string_t> &out, const utility::string_t &directory) ;

void open_browser (utility::string_t auth_uri) ;
