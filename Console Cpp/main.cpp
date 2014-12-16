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
#include "ReCapClient.h"
#include "http_multipart_formdata.h"

// http://casablanca.codeplex.com/discussions/532255
// http://docs.telerik.com/fiddler/configure-fiddler/tasks/ConfigureWinHTTPApp

static int debug_flag =0 ;
static int proxy_flag =0 ;

void usage () {
	ucout << std::endl << U("ReCap [-h] [-l] [-r] [-d] [-y] [-i photosceneid] [-c command] [-p photo(s)] ") << std::endl ;

	ucout << U("\t-l\tLog on the oAuth server") << std::endl ;
	ucout << U("\t-r\tRefresh the oAuth token only") << std::endl ;

	ucout << U("-d\tDebug mode. Display the RESTful response") << std::endl ;
	ucout << U("-y\tEnable proxy debug when using Fiddler") << std::endl ;

	ucout << U("-c\tCommand list") << std::endl ;
	ucout << U("\t   version - Displays the current ReCap server version") << std::endl ;
	ucout << U("\t   current - Displays the current photosceneid in use") << std::endl ;
	ucout << U("\t   create - Create a new Photoscene") << std::endl ;
	ucout << U("\t   set - Set the current Photoscene ID - requires -i option") << std::endl ;
	ucout << U("\t   release - Release the current photosceneid") << std::endl ;
	ucout << U("\t   list - List all photoscenes present on your account") << std::endl ;
	ucout << U("\t   properties - Displays current Photoscene properties") << std::endl ;
	ucout << U("\t   upload - Upload photo(s) on your current Photoscene - requires -p option (could be a single file, a folder, or a search string)") << std::endl ;
	ucout << U("\t   start - Launch your Photoscene") << std::endl ;
	ucout << U("\t   progress - Report progress on processing the Photoscene") << std::endl ;
	ucout << U("\t   result - Get the result") << std::endl ;
	ucout << U("\t   delete - Delete the Photoscene and resources from server") << std::endl ;
	
	ucout << U("-h\tHelp - this message") << std::endl ;
}

static struct option long_options [] ={
	{ U("help"), ARG_NONE, 0, U('h') },

	{ U("logon"), ARG_NONE, 0, U('l') },
	{ U("refresh"), ARG_NONE, 0, U('r') },

	{ U("debug"), ARG_NONE, 0, U('d') },
	{ U("proxy"), ARG_NONE, 0, U('y') },

	{ U("command"), ARG_NONE, 0, U('c') },
	{ U("photosceneid"), ARG_NONE, 0, U('i') },
	{ U("photos"), ARG_NONE, 0, U('p') },

	{ ARG_NULL, ARG_NULL, ARG_NULL, ARG_NULL }
} ;

int _tmain (int argc, _TCHAR *argv []) {
	ucout << U("Initializing...") << std::endl ;
	utility::string_t token, secret, session ;
	utility::string_t command, photosceneid, photos ;
	ReCapClient::ReadPhotosceneId (photosceneid) ;
	oauth1_config config (CONSUMER_KEY, CONSUMER_SECRET, OAUTH_REQUESTTOKEN, OAUTH_AUTHORIZE, OAUTH_ACCESSTOKEN, CALLBACK_URL, oauth1_methods::hmac_sha1) ;

	bool bLoop =true ;
	while ( bLoop ) {        
		int option_index =0 ;
		// http://www.gnu.org/software/libc/manual/html_node/Argument-Syntax.html
		// http://www.gnu.org/software/libc/manual/html_node/Argp-Examples.html#Argp-Examples
		// http://stackoverflow.com/questions/13251732/c-how-to-specify-an-optstring-in-the-getopt-function
		int c =getopt_long (argc, argv, U("dyhlrc:i:p:"), long_options, &option_index) ;
		// Check for end of operation or error
		if ( c == -1 )
			break ;

		// Handle options
		switch ( c ) {
			case 0:
				break ;
			case U('?'):
				// getopt_long already printed an error message.
				break ;
			default:
				bLoop =false ;
				break ;

			case U('h'): // Help message
				usage () ;
				return (0) ;

			case U('l'): // Log on the oAuth server
				if ( oAuthLogonAsync (config) ) {
					ucout << U("Could not log on the oAuth server! Exiting...") << std::endl ;
					return (1) ;
				}
				return (0) ;
			case U('r'): // Refresh the oAuth token
				if ( ReadTokens (token, secret, session) ) {
					oauth1_token tokens (token, secret) ;
					tokens.set_session (session) ;
					config.set_token (tokens) ;
					if ( oAuthRefreshTokensAsync (config) ){
						ucout << U("Failed to refresh your oAuth tokens! Exiting...") << std::endl ;
						return (1) ;
					}
				}
				return (0) ;

			case U('d'):
				debug_flag =1 ;
				break ;
			case U('y'):
				proxy_flag =1 ;
				break ;

			case U('c'):
				command =optarg_w ;
				break ;
			case U('i'):
				photosceneid =optarg_w ;
				break ;
			case U('p'):
				photos =optarg_w ;
				break ;
		}
	}

	// Get oAuth job done first.
	if ( !ReadTokens (token, secret, session) ) {
		if ( oAuthLogonAsync (config) ) {
			ucout << U("Could not log on the oAuth server! Exiting...") << std::endl ;
			return (1) ;
		}
	} else {
		oauth1_token tokens (token, secret) ;
		tokens.set_session (session) ;
		config.set_token (tokens) ;
		if ( oAuthRefreshTokensAsync (config) ) {
			ucout << U("Failed to refresh the oAuth tokens! Trying to log you on...") << std::endl ;
			oauth1_token tokens (U(""), U("")) ;
			config.set_token (tokens) ;
			if ( oAuthLogonAsync (config) ) {
				ucout << U("Could not log on the oAuth server! Exiting...") << std::endl ;
				return (1) ;
			}
		}
	}

	// ReCap
	ReCapClient recap (ReCapClientID, config, proxy_flag != 0) ;
	recap.debug () =(debug_flag != 0) ;

	//- Requesting the ReCap service/date to start and check our connection/authentication
	//- We always do regardless of the command line option to version our connection
	ucout << std::endl << U("Verifying ReCap Server connection - ") ;
	if ( recap.Time () == false ) {
		ucout << U("service/date - Failed to get a valid response from the ReCap server!") << std::endl ;
		return (2) ;
	}
	json::value dt =recap.json ().at (U("date")) ;
	ucout << U("service/date response: ") << dt.as_string () << std::endl << std::endl ;

	// Command processing
	if ( command == U("version") ) {
		// -d -c version
		if ( recap.Version () == false ) {
			ucout << U("version - Failed to get a valid response from the ReCap server!") << std::endl ;
			return (2) ;
		}
		json::value version =recap.json ().at (U("version")) ;
		ucout << U("version response: ") << version.as_string () << std::endl ;
	} else if ( command == U("current") ) {
		// -d -c current
		ucout << U("Your current Photosceneid is: ") << photosceneid << std::endl ;
	} else if ( command == U("release") ) {
		ReCapClient::DeletePhotosceneId () ;
		ucout << U("Your current Photosceneid is now released") << std::endl ;
	} else if ( command == U("create") ) {
		// -d -c create
		if ( recap.CreateSimplePhotoscene (U("obj"), U("7")) == false ) {
			ucout << U("photoscene - Failed to get a valid response from the ReCap server!") << std::endl ;
			return (2) ;
		}
		json::value v =recap.json ().at (U("Photoscene")) ;
		photosceneid =v.at (U("photosceneid")).as_string () ;
		ReCapClient::SavePhotosceneId (photosceneid) ;
		ucout << U("Your new current Photosceneid is: ") << photosceneid << std::endl ;
	} else if ( command == U("set") ) {
		// -d -c set -i XXXXXX
		ReCapClient::SavePhotosceneId (photosceneid) ;
		ucout << U("Your current Photosceneid is now: ") << photosceneid << std::endl ;
	} else if ( command == U("list") ) {
		// -d -c list
		if ( recap.SceneList (U("userID"), ReCapUserID) == false ) {
			ucout << U("photoscene/properties - Failed to get a valid response from the ReCap server!") << std::endl ;
			return (2) ;
		}
		json::value v =recap.json ().at (U("Photoscenes")).at (U("Photoscene")) ;
		//json::object &obj =v.as_object () ;
		json::array &obj =v.as_array () ;
		if ( obj.size () == 0 ) {
			ucout << U("\t0 Photoscene present on your account") << std::endl ;
		} else {
			ucout << U("\tPhotoscenes already created: ") << obj.size () << U(" Photoscene(s)") << std::endl ;
			json::array::iterator it =obj.begin () ;
			for ( ; it != obj.end () ; ++it ) {
				json::value photoscene =*it ;
				if ( photoscene.has_field (U("deleted")) && photoscene.at (U("deleted")).as_string () == U("true") )
					continue ;
				json::value id =photoscene.at (U("photosceneid")) ;
				json::value st =photoscene.at (U("status")) ;
				ucout << U("\t\t") << id.as_string () << U("[") << st.as_string () << U("]") << std::endl ;
			}
			ucout << U("\tDeleted Photoscenes") << std::endl ;
			it =obj.begin () ;
			for ( ; it != obj.end () ; ++it ) {
				json::value photoscene =*it ;
				if ( !photoscene.has_field (U("deleted")) || photoscene.at (U("deleted")).as_string () != U("true") )
					continue ;
				json::value id =photoscene.at (U("photosceneid")) ;
				json::value st =photoscene.at (U("status")) ;
				ucout << U("\t\t{deleted} - ") << id.as_string () << U("[") << st.as_string () << U("]") << std::endl ;
			}
		}
	} else if ( command == U("properties") ) {
		// -d -c properties
		if ( photosceneid == U("") ) {
			ucout << U("You need to specify a Photoscene ID") << std::endl ;
			return (3) ;
		}
		ucout << U("Working with ReCap Photoscene: ") << photosceneid << std::endl ;
		if ( recap.SceneProperties (photosceneid) == false ) {
			ucout << U("photoscene/.../properties - Failed to get a valid response from the ReCap server!") << std::endl ;
			return (2) ;
		}
		json::value photoscene =recap.json ().at (U("Photoscenes")).at (U("Photoscene")) ;
		ucout << U("PhotoScene: ") << photosceneid << std::endl ;
		ucout << U("\tName: ") << photoscene.at (U("name")).as_string () << std::endl ;
		ucout << U("\tFormat: ") << photoscene.at (U("convertFormat")).as_string () << std::endl ;
		//if ( photoscene.has_field (U("callback")) ) // not a string
		//	ucout << U("\tcallback: ") << ??? << std::endl ;
		if ( photoscene.has_field (U("Files")) ) {
			//json::object &ofiles =photoscene.at (U("Files")).as_object () ;
			json::array &ofiles =photoscene.at (U("Files")).at (U("file")).as_array () ;
			ucout << U("\tFile(s):") << std::endl ;
			for ( const auto &param : ofiles ) {
				if ( !param.has_field (U("filename")) )
					continue ;
				ucout << U("\t\t")
					  << param.at (U("@attributes")).at (U("pos")).as_string () << U(": ")
					  << param.at (U("filename")).as_string () << U(" [")
					  << param.at (U("fileid")).as_string () << U("]")
					  << std::endl ;
			}
		} else {
			ucout << U("\tFiles: 0 files loaded") << std::endl ;
		}
		ucout << U("\tclientStatus: ") << photoscene.at (U("clientStatus")).as_string () << std::endl ; // [CREATED / SENT]
		ucout << U("\tconvertStatus: ") <<photoscene.at (U("convertStatus")).as_string ()  << std::endl ; // [CREATED / ERROR]
		ucout << U("\tstatus: ") << photoscene.at (U("status")).as_string () << std::endl ; // [PROCESSING / ERROR]
	} else if ( command == U("upload") ) {
		// -d -c upload -p "C:\Users\cyrille\Documents\GitHub\Autodesk-ReCap-Samples\Examples\Tirelire\Tirelire1.jpg"
		// -d -c upload -p "D:\Projects\Autodesk-ReCap-Samples\Examples\Tirelire\Tirelire1.jpg"
		if ( photos == U("") ) {
			ucout << U("Error: missing argument -p, see help for details") << std::endl ;
			return (4) ;
		}
		if ( photosceneid == U("") ) {
			ucout << U("You need to specify a Photoscene ID") << std::endl ;
			return (3) ;
		}
		ucout << U("Working with ReCap Photoscene: ") << photosceneid << std::endl ;

		//std::vector<utility::string_t> files =split (photos, U(';')) ;
		std::vector<utility::string_t> files ;
		struct stat st ;
		stat (ws2s (photos).c_str (), &st) ;
		if ( st.st_mode & _S_IFDIR )
			GetFilesInDirectory (files, photos) ;
		else
			files.push_back (photos) ;
		if ( files.size () == 0 ) {
			ucout << U("Error: no photos to upload") << std::endl ;
			return (4) ;
		}
		if ( recap.UploadFiles (photosceneid, files) == false ) {
			ucout << U("file - Failed to get a valid response from the ReCap server!") << std::endl ;
			return (2) ;
		}
		ucout << U("File(s) uploaded:") << std::endl ;
		json::value jfiles =recap.json ().at (U("Files")).at (U("file")) ;
		if ( jfiles.is_array () ) { // multiple files
			json::array &ofiles =jfiles.as_array () ;
			for ( const auto &param : ofiles ) {
				ucout << U("\t")
					  << param.at (U("filename")).as_string () << U("[")
					  << param.at (U("fileid")).as_string () << U("]")
					  << std::endl ;
			}
		} else { // single file
			json::object &ofiles =jfiles.as_object () ;
			ucout << U("\t")
				  << ofiles.at (U("filename")).as_string () << U("[")
				  << ofiles.at (U("fileid")).as_string () << U("]")
				  << std::endl ;
		}
	} else if ( command == U("start") ) {
		// -d -c start
		if ( photosceneid == U("") ) {
			ucout << U("You need to specify a Photoscene ID") << std::endl ;
			return (3) ;
		}
		ucout << U("Working with ReCap Photoscene: ") << photosceneid << std::endl ;
		if ( recap.ProcessScene (photosceneid) == false ) {
			ucout << U("photoscene/... - Failed to get a valid response from the ReCap server!") << std::endl ;
			return (2) ;
		}
		ucout << U("Photoscene processing request sent") << std::endl ;
	} else if ( command == U("progress") ) {
		// -d -c progress
		if ( photosceneid == U("") ) {
			ucout << U("You need to specify a Photoscene ID") << std::endl ;
			return (3) ;
		}
		ucout << U("Working with ReCap Photoscene: ") << photosceneid << std::endl ;
		if ( recap.SceneProgress (photosceneid) == false ) {
			ucout << U("photoscene/.../progress - Failed to get a valid response from the ReCap server!") << std::endl ;
			return (2) ;
		}
		json::value resp =recap.json () ;
		if ( resp.has_field (U("Photoscene")) ) {
			json::value photoscene =recap.json ().at (U("Photoscene")) ;
			ucout << U("photoscene/.../progress response: ")
				  << photoscene.at (U("progressmsg")).as_string () << U(" ")
				  << photoscene.at (U("progress")).as_string () << U("%")
				  << std::endl ;
		} else {
			ucout << U("photoscene/.../progress - no progress message") << std::endl ;
		}
	} else if ( command == U("result") ) {
		// -d -c result
		if ( photosceneid == U("") ) {
			ucout << U("You need to specify a Photoscene ID") << std::endl ;
			return (3) ;
		}
		ucout << U("Working with ReCap Photoscene: ") << photosceneid << std::endl ;
		if ( recap.GetPointCloudArchive (photosceneid, U("obj")) == false ) {
			ucout << U("photoscene/... - Failed to get a valid response from the ReCap server!") << std::endl ;
			return (2) ;
		}
		json::value photoscene =recap.json ().at (U("Photoscene")) ;
		ucout << U("photoscene/... response: ")
			  << web::uri::decode (photoscene.at (U("scenelink")).as_string ()) << U(" - ")
			  << photoscene.at (U("filesize")).as_string () << U("b")
			  << std::endl ;
		
		WCHAR buffer [MAX_PATH] ;
		::GetModuleFileName (NULL, buffer, MAX_PATH) ;
		std::wstring::size_type pos =std::wstring (buffer).find_last_of (U("\\/")) ;
		std::wstring path =std::wstring (buffer).substr (0, pos + 1) + photosceneid + U(".zip") ;
		recap.DownloadFile (
			photoscene.at (U("scenelink")).as_string (),
			path
		) ;
		ucout << U("PhotoScene saved into: ") << path << std::endl ;
	} else if ( command == U("delete") ) {
		// -d -c delete
		if ( photosceneid == U("") ) {
			ucout << U("You need to specify a Photoscene ID") << std::endl ;
			return (3) ;
		}
		ucout << U("Working with ReCap Photoscene: ") << photosceneid << std::endl ;
		if ( recap.DeleteScene (photosceneid) == false ) {
			ucout << U("photoscene/... - Failed to get a valid response from the ReCap server!") << std::endl ;
			return (2) ;
		}
		json::value resp =recap.json () ;
		if ( resp.has_field (U("Photoscene")) ) {
			json::value photoscene =recap.json ().at (U("Photoscene")) ;
			if ( photoscene.has_field (U("deleted")) ) {
				ucout << U("Your ReCap PhotoScene is now deleted") << std::endl ;
				ReCapClient::DeletePhotosceneId () ;
				ucout << U("Your current Photosceneid is now released") << std::endl ;
			} else {
				ucout << U("Failed deleting the PhotoScene and resources!") << std::endl ;
			}
		} else {
			ucout << U("photoscene/... - no delete message") << std::endl ;
		}
	} else {
		ucout << U("Invalid command") << std::endl ;
	}

#ifdef _DEBUG
	ucout << U("Press any key to terminate") << std::endl ;
	for ( ; !_kbhit () ; ) ;
#endif
	ucout << U("Done.") << std::endl ;
	return (0) ;
}
