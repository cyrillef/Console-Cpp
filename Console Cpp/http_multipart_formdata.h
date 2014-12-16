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

class http_multipart_formdata {

protected:
	utility::string_t _boundary ;
	std::stringstream _ostr ;

public:
	http_multipart_formdata () ;

	void initialize (web::http::http_request &request) ;
	void addParameter (utility::string_t key, utility::string_t value) ;
	void addParameters (std::map<utility::string_t, utility::string_t> &parameters) ;
	void addFile (utility::string_t filename, int index =0) ;
	void addFiles (std::vector<utility::string_t> &files) ;
	void http_multipart_formdata::closeBody (web::http::http_request &request) ;

protected:
	static utility::string_t http_multipart_formdata::openBodyContentType (utility::string_t boundary) ;
	static void addParameter (std::stringstream &ostr, utility::string_t boundary, utility::string_t name, utility::string_t value) ;
	static void addParameter (std::wstringstream &ostr, utility::string_t boundary, utility::string_t name, utility::string_t value) ;
	static void addChunk (std::stringstream &ostr, utility::string_t boundary, utility::string_t name, utility::string_t filename, Concurrency::streams::basic_istream<byte> &chunk) ;
	static void addChunk (std::wstringstream &ostr, utility::string_t boundary, utility::string_t name, utility::string_t filename, Concurrency::streams::basic_istream<unsigned char> &chunk) ;
	static void http_multipart_formdata::closeBody (std::stringstream &ostr, utility::string_t boundary) ;
	static void http_multipart_formdata::closeBody (std::wstringstream &ostr, utility::string_t boundary) ;

} ;


