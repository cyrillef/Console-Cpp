This sample is deprecated as there is a new API for ReCap
=======================
Go to https://developer.autodesk.com/en/docs/reality-capture/v1/overview/ for more information

[![language](https://img.shields.io/badge/language-C++-blue.svg)](https://www.visualstudio.com/)
[![ReCap](https://img.shields.io/badge/Reality%20Capture%20API-v3.1%20-green.svg)](http://developer-recap-autodesk.github.io/)
![Platforms](https://img.shields.io/badge/platform-windows-lightgray.svg)
[![License](http://img.shields.io/:license-mit-blue.svg)](http://opensource.org/licenses/MIT)

# Autodesk Reality Capture API -- C++ Console sample
-------------------

This sample is a command line sample where you control the various ReCap stage individually using one of the command below.

<b>Note:</b> For using those samples you need a valid oAuth credential and a ReCap client ID. Visit this [page](http://developer-recap-autodesk.github.io/) for instructions to get on-board.

## Motivation

The Reality Capture API Beta provides a web service to create textured mesh from a set of photos, and can request an automatic 3D calibration. The REST API provides a similar service as the [Autodesk ReCap 360](http://www.autodesk.com/products/recap-360/overview) web application. The purpose of this sample is to show an application that can provide a Reality Capture work flow using photographic images.

## Description

This sample uses the C++ based CPP REST SDK  to demonstrate how to use the Reality Capture API.

## Dependencies

This sample is dependent of the following 3rd party extension:

* The [C++ RESTful SDK](https://github.com/Microsoft/cpprestsdk) - version 2.8.0 (formerly known as Casablanca)
* The Visual Studio project is referecing the library direcly, and will get the SDK during the first buuld.


## Setup Instructions
	
* Build the ReCap sample project using Visual Studio 2015:

	1. Copy the UserSettings_.h into UserSettings.h
	
	2. Open the ReCap project in Visual Studio 2015

	3. Edit UserSettings.h, and replace the following key strings with appropriate value
           * CONSUMER_KEY
           * CONSUMER_SECRET
           * ReCapClientID
           * Email
	 
	4. Compile and you are ready to go.
           use the -h command argument to get help.
	
## Usage Instructions

The command will save your current PhotosceneID and oAuth token in local files, you can delete these files to reset the system.

```
Usage:    ReCap [-d] [-y] [-r] [-h] [-i photosceneid] [-c command] [-p photo(s)]

	-r	Refresh Access token only
	-d	Debug mode. Display the RESTful response
	-y	Enable proxy debug when using Fiddler
	-c	Command list
			version - Displays the current ReCap server version
			current - Displays the current photosceneid in use
			create - Create a new Photoscene
			set - Set the current Photoscene ID - requires -i option
			release - Release the current photosceneid
			list - List all photoscenes present on your account
			properties - Displays current Photoscene properties
			upload - Upload photo(s) on your current Photoscene - requires -p option (could be a single file, a folder, or a search string)
			start - Launch your Photoscene
			progress - Launch your Photoscene
			result - Get the result
			delete - Delete the Photoscene and resources from server
	-h	Help - this message
```

#### Typical scenario:
```
ReCap -c create
ReCap -c upload -p ../Examples/Tirelire
ReCap -c properties
ReCap -c start
ReCap -c progress

```
once 'progress' reports no error and completion at 100%, you can then querie final properties, and retrieve the mesh.
```
ReCap -c properties
ReCap -c result

```

## License

This sample is licensed under the terms of the [MIT License](http://opensource.org/licenses/MIT). Please see the [LICENSE](LICENSE) file for full details.


## Credits

Cyrille Fauvel (Autodesk Developer Network)  
http://www.autodesk.com/adn  
http://around-the-corner.typepad.com/  
