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

#include <codecvt>

std::vector<utility::string_t> &split (const utility::string_t &s, wchar_t delim, std::vector<utility::string_t> &elems) {
	std::wstringstream ss (s.c_str ()) ;
	std::wstring item ;
	while ( std::getline (ss, item, delim) )
		elems.push_back (item) ;
	return (elems) ;
}

std::vector<utility::string_t> split (const utility::string_t &s, wchar_t delim) {
    std::vector<utility::string_t> elems ;
    split (s, delim, elems) ;
    return (elems) ;
}

std::string ws2s (const std::wstring &wstr) {
    typedef std::codecvt_utf8<wchar_t> convert_typeX ;
    std::wstring_convert<convert_typeX, wchar_t> converterX ;
    return (converterX.to_bytes (wstr)) ;
}

std::string wchar2string (const wchar_t *text) {
	std::string result ;
	if ( text ) {
		char *textChar ;
		int count =(int)wcslen (text) ;
		count =::WideCharToMultiByte (CP_ACP, 0, text, count, 0, 0, NULL, NULL) ;
		if ( count > 0 ) {
			textChar =new char [count + 1] ;
			::WideCharToMultiByte (CP_ACP, 0, text, count, textChar, count + 1 , NULL, NULL) ;
			textChar [count] ='\0' ;
			result =textChar ;
			delete [] textChar ;
		}
	}
	return (result) ;
}

void GetFilesInDirectory (std::vector<utility::string_t> &out, const utility::string_t &directory) {
#ifdef _MS_WINDOWS
	HANDLE dir ;
	WIN32_FIND_DATA file_data ;

	if ( (dir =FindFirstFile ((directory + U("/*")).c_str (), &file_data)) == INVALID_HANDLE_VALUE )
		return ;
	do {
		const utility::string_t file_name =file_data.cFileName ;
		const utility::string_t full_file_name =directory + U("/") + file_name ;
		const bool is_directory =(file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 ;
		if ( file_name [0] == '.' )
			continue ;
		if ( is_directory )
			continue ;
		out.push_back (full_file_name) ;
	} while ( FindNextFile (dir, &file_data) ) ;
	FindClose (dir) ;
#else
	class dirent *ent ;
	class stat st ;
	DIR *dir =opendir (directory) ;
	while ( (ent =readdir (dir)) != NULL ) {
		const std::string file_name =ent->d_name ;
		const std::string full_file_name =directory.c_str () + "/" + file_name ;
		if ( file_name [0] == '.' )
			continue ;
		if ( stat (full_file_name.c_str (), &st) == -1 )
			continue ;
		const bool is_directory =(st.st_mode & S_IFDIR) != 0 ;
		if ( is_directory )
			continue ;
		out.push_back (full_file_name) ;
	}
	closedir (dir) ;
#endif
}

#if defined(_MS_WINDOWS) && !defined(__cplusplus_winrt)
#include <windows.h>
#include <Shellapi.h>
#endif

void open_browser (utility::string_t auth_uri) {
#if defined(_MS_WINDOWS) && !defined(__cplusplus_winrt)
	auto r =ShellExecute (NULL, U("open"), auth_uri.c_str (), NULL, NULL, SW_SHOWNORMAL) ;
#elif defined(__APPLE__)
	string_t browser_cmd (U("open \"") + auth_uri + U("\"")) ;
	system (browser_cmd.c_str ()) ;
#else
	string_t browser_cmd (U("xdg-open \"") + auth_uri + U("\"")) ;
	system (browser_cmd.c_str ()) ;
#endif
}
