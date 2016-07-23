/*
* Copyright (C) 2015 Jordi Epema
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/

/**
 * @file	include\variables.h
 *
 * @brief	Holds all the macro's for settings mainly
 */

/**
* @def	ODB_RETRY_ATTEMPTS
*
* @brief	A macro that defines the number of threads which run parallel
*
* @author	Jordi
* @date	6-7-2015
*/

#define NUM_THREADS 20

/**
 * @def	ODB_RETRY_ATTEMPTS
 *
 * @brief	A macro that defines the number of odb retry attempts.
 *
 * @author	Jordi
 * @date	6-7-2015
 */

#define ODB_RETRY_ATTEMPTS 5

/**
 * @def	RETRY_ATTEMPTS
 *
 * @brief	A macro that defines the number of retry attempts.
 *
 * @author	Jordi
 * @date	6-7-2015
 */

#define RETRY_ATTEMPTS 10

// CURL settings

/**
 * @def	CURL_TIMEOUT
 *
 * @brief	A macro that defines the curl timeout.
 *
 * @author	Jordi
 * @date	7-7-2015
 */

#define CURL_TIMEOUT 30 // total request time out in seconds

/**
 * @def	CURL_CONNECT_TIMEOUT
 *
 * @brief	A macro that defines the curl connect timeout.
 *
 * @author	Jordi
 * @date	7-7-2015
 */

#define CURL_CONNECT_TIMEOUT 30

/**
 * @def	CURL_MAX_CONNECTS
 *
 * @brief	A macro that defines curl maximum connections.
 *
 * @author	Jordi
 * @date	7-7-2015
 */

#define CURL_MAX_CONNECTS 30

/**
 * @def	CURL_FRESH_CONNECT
 *
 * @brief	A macro that defines if CURL needs to create a new connection for each request
 *
 * @author	Jordi
 * @date	7-7-2015
 */

#define CURL_FRESH_CONNECT 0

/**
 * @def	CURL_FORBID_REUSE
 *
 * @brief	A macro that defines if CURL should reuse connections
 *
 * @author	Jordi
 * @date	7-7-2015
 */

#define CURL_FORBID_REUSE 0

// URL's where data comes from

/**
 * @def	NHL_REFERER
 *
 * @brief	A macro that defines the referer url needed for spoofing
 *
 * @author	Jordi
 * @date	7-7-2015
 */

#define NHL_REFERER "http://www.mijnnhl.nl/iframe/engineering-studentenrooster"

/**
 * @def	CLASS_TYPES_SOURCE
 *
 * @brief	A macro that defines the url where classes will be fetched from
 *
 * @author	Jordi
 * @date	7-7-2015
 */

#define CLASS_TYPES_SOURCE "https://rooster.nhl.nl/1516/roosterparser.php?filter=%s&obj="

/**
 * @def	WEEK_FETCH_SOURCE
 *
 * @brief	A macro that defines the url where schedule will be fetched from
 *
 * @author	Jordi
 * @date	7-7-2015
 */

//#define WEEK_FETCH_SOURCE "https://rooster.nhl.nl/1516/icalendar//rss/rss2.0.php?cal=%s&cpath=%s/%s&rssview=year"
#define WEEK_FETCH_SOURCE "https://rooster.nhl.nl/1516/createICAL.php?sleutelveld=%s&obj=%s&filter=%s"
/**
 * @def	NHL_REFERER_WEEK
 *
 * @brief	A macro that defines the referer needed for WEEK_FETCH_SOURCE url
 *
 * @author	Jordi
 * @date	7-7-2015
 */

#define NHL_REFERER_WEEK "https://rooster.nhl.nl/1516/rooster.php?obj=%s&filter="

// GCM - Google Cloud Messaging

/**
 * @def	GCM_AUTH_KEY_HEADER
 *
 * @brief	A macro that defines gcm authentication key header.
 *
 * @author	Jordi
 * @date	7-7-2015
 */

#define GCM_AUTH_KEY_HEADER ""

// Database credentials

/**
 * @def	DB_USER
 *
 * @brief	A macro that defines database user.
 *
 * @author	Jordi
 * @date	7-7-2015
 */

#define DB_USER "NHLRooster"

/**
 * @def	DB_PASS
 *
 * @brief	A macro that defines database pass.
 *
 * @author	Jordi
 * @date	7-7-2015
 */

#define DB_PASS ""

/**
 * @def	DB_NAME
 *
 * @brief	A macro that defines database name.
 *
 * @author	Jordi
 * @date	7-7-2015
 */

#define DB_NAME "NHLRooster"

/**
 * @def	DB_HOST
 *
 * @brief	A macro that defines database host.
 *
 * @author	Jordi
 * @date	7-7-2015
 */

#define DB_HOST "localhost"

/**
 * @def	DB_PORT
 *
 * @brief	A macro that defines database port.
 *
 * @author	Jordi
 * @date	7-7-2015
 */

#define DB_PORT 3306

/**
 * @def	VERSION_NUMBER
 *
 * @brief	A macro that defines the version number.
 *
 * @author	Jordi
 * @date	7-7-2015
 */

#define VERSION_NUMBER "1.04"

 /**
 * @def	OS
 *
 * @brief	A macro that defines operating system it has been compiled for.
 *
 * @author	Jordi
 * @date	7-7-2015
 */

#ifdef __linux__
#if __i386__
	#define OS "linux-i386"
#elif __x86_64__
	#define OS "linux-amd64"
#else
	#error "Unsupported OS"
#endif
#elif _WIN32
	#define OS "win32"
#elif _WIN64
	#define OS "win64"
#endif
