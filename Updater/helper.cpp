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
 * @file	helper.cpp
 *
 * @brief	Implements some helper functions
 */

#include "includes.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include <cstdio>

using namespace boost::posix_time;

extern std::vector<std::thread> workers;

/**
 * @fn	bool isAlreadyRunning()
 *
 * @brief	Returns true if there already is an instance of this program running
 *
 * @author	Jordi
 * @date	6-7-2015
 *
 * @return	true if already running, false if not.
 */

bool isAlreadyRunning(){
#ifdef __linux__
	int pid_file = open("NHLRoosterUpdater.pid", O_CREAT | O_RDWR, 0666);
	int rc = flock(pid_file, LOCK_EX | LOCK_NB);
	if (rc) {
		if (EWOULDBLOCK == errno)
		{
			return true;
		}
	}
	return false;
#elif _WIN32 || _WIN64
	CreateMutexA(0, false, "Local\\NHLRoosterUpdater");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
		return true;
	else
		return false;
#else
	#error "Unsupported operating system"
#endif
}

/**
 * @fn	int executeWorkersTask(void(*task)())
 *
 * @brief	Executes task method on NUM_THREADS threads, pushes the thread ids to workers list.
 *
 * @author	Jordi
 * @date	6-7-2015
 *
 * @param [in,out]	task	If non-null, the task.
 *
 * @return	An int.
 */

int executeWorkersTask(std::function<void()> task){
	workers.clear();
	for (int i = 0; i < NUM_THREADS; i++)
	{
		auto t = std::thread(task);
		workers.push_back(std::move(t));
	}
	return NUM_THREADS;
}

/**
 * @fn	void waitForThreads()
 *
 * @brief	Waits until all worker threads are done
 *
 * @author	Jordi
 * @date	6-7-2015
 */

void waitForThreads() {
	std::for_each(workers.begin(),
		workers.end(),
		[](std::thread &t)
	{
		assert(t.joinable());
		t.join();
	});
}

/**
 * @fn	bool odb_retry(std::function<void()> func, size_t max_attempts, std::function<void()> revertFunc)
 *
 * @brief	Will retry odb recoverable exceptions for max_attempts times.
 *
 * @author	Jordi
 * @date	6-7-2015
 *
 * @param	func			The function which will be retried.
 * @param	max_attempts	The maximum number of attempts before a fail.
 * @param	revertFunc  	The revert function which will be executed when the function fails,
 * 							can be used to revert stuff.
 *
 * @return	true if it succeeds, false if it fails.
 */

bool odb_retry(std::function<void()> func, size_t max_attempts, std::function<void()> revertFunc) {
	for (size_t attempt{ 0 }; attempt < max_attempts; ++attempt) {
		try{
			func();
			return true;
		}
		catch (const odb::recoverable& e)
		{
			if (revertFunc != nullptr) // execute a optional revert function, also with retry wrapper
				odb_retry(revertFunc, max_attempts);
			continue; // retry when we get a recoverable odb exception
		}
		// Possible delay between retry attempts here
	}
	return false;
}

/**
 * @fn	bool retry(std::function<bool()> func, size_t max_attempts, unsigned long msDelay)
 *
 * @brief	Retries the given function untill success or max_attempts have been reached
 *
 * @author	Jordi
 * @date	6-7-2015
 *
 * @param	func			The function which will be retried
 * @param	max_attempts	The maximum number of attempts before a fail.
 * @param	msDelay			The optional delay in milliseconds between attempts.
 *
 * @return	true if it succeeds, false if it fails.
 */

bool retry(std::function<bool()> func, size_t max_attempts, unsigned long msDelay) {
	for (size_t attempt{ 0 }; attempt < max_attempts; ++attempt) {
		if (func()) { return true; } // retry until our function returns true
		// Delay between retry attempts
		std::this_thread::sleep_for(std::chrono::milliseconds(msDelay));
	}
	return false;
}

/**
 * @fn	std::string replaceAll(std::string str, const std::string& from, const std::string& to)
 *
 * @brief	Replaces all occurences with given string
 *
 * @author	Jordi
 * @date	6-7-2015
 *
 * @param	str 	The string which is being processed.
 * @param	from	Find these occurences.
 * @param	to  	Replace all from occurences with to.
 *
 * @return	String where all occurences of from have been replaced with to.
 */

std::string replaceAll(std::string str, const std::string& from, const std::string& to)
{
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

/**
 * @fn	std::string getStringBetween(const std::string& before, const std::string& after, const std::string &line, const std::string regex_between)
 *
 * @brief	Returns the string which is between the other 2 strings by using a regular expression
 *
 * @author	Jordi
 * @date	6-7-2015
 *
 * @param	before		 	The string before.
 * @param	after		 	The string after.
 * @param	line		 	The string which needs to be processed.
 * @param	regex_between	The regular expression.
 *
 * @return	String without the found expression
 */

std::string getStringBetween(const std::string& before, const std::string& after, const std::string &line, const std::string regex_between)
{
    // use streams to concat values
    std::string result;
    std::stringstream ss;
    ss << before << regex_between << after;
    std::string regex_str = ss.str();
    const std::regex e(regex_str);
    std::sregex_iterator next(line.begin(), line.end(), e);
    std::smatch match = *next;
    result = match.str();
    result = replaceAll(result, before, "");
    result = replaceAll(result, after, "");
    return result;
}

/**
 * @fn	inline std::string& ltrim(std::string& string, const char* t)
 *
 * @brief	Trims whitespace\tabs from start of given string.
 *
 * @author	Jordi
 * @date	6-7-2015
 *
 * @param [in,out]	string	The string.
 * @param	t			  	Optional characters which needs to be trimmed.
 *
 * @return	trimmed results;
 */

inline std::string& ltrim(std::string& string, const char* t)
{
	string.erase(0, string.find_first_not_of(t));
	return string;
}

/**
 * @fn	inline std::string& rtrim(std::string& string, const char* t)
 *
 * @brief	Trims whitespace\tabs from end of given string.
 *
 * @author	Jordi
 * @date	6-7-2015
 *
 * @param [in,out]	string	The string.
 * @param	t			  	Optional characters which needs to be trimmed.
 *
 * @return	trimmed results;
 */

inline std::string& rtrim(std::string& string, const char* t)
{
	string.erase(string.find_last_not_of(t) + 1);
	return string;
}

/**
 * @fn	std::string& trim(std::string& string, const char* t)
 *
 * @brief	Trims whitespace\tabs from start and end of given string
 *
 * @author	Jordi
 * @date	6-7-2015
 *
 * @param [in,out]	string	The string.
 * @param	t			  	Optional characters which needs to be trimmed
 *
 * @return	trimmed results;
 */

std::string& trim(std::string& string, const char* t)
{
    return ltrim(rtrim(string, t), t);
}

/**
 * @fn	int getCurrentWeekNumber()
 *
 * @brief	Gets current week number.
 *
 * @author	Jordi
 * @date	6-7-2015
 *
 * @return	The current week number(1-52).
 */

int getCurrentWeekNumber()
{
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[4];
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, 4, "%W", timeinfo);   // '%W' = week number of the year, eg 1/1/09 == 1
	return atoi(buffer) + 1;     // convert char to int
}

/**
 * @fn	int getWeekNrFromDate(const std::string& date)
 *
 * @brief	Returns the week number from given date according to ISO 8601
 *
 * @author	Jordi
 * @date	6-7-2015
 *
 * @param	date	Has to be in format YYYY-MM-DDTHH:MM:SS
 *
 * @return	The week nr(1-52) from date.
 */

int getWeekNrFromDate(const std::string& date)
{
	//2015-04-29T15:15:00
	int year, month, day, hour, minute, seconds;
	sscanf(date.c_str(), "%d-%d-%dT%d:%d:%d", &year, &month, &day, &hour, &minute, &seconds);
    struct tm tm;
	tm.tm_hour = hour;
	tm.tm_min = minute;
	tm.tm_sec = seconds;
	tm.tm_mday = day;
	tm.tm_mon = month - 1;
	tm.tm_year = year - 1900;
	mktime(&tm);
    char timebuf[64];
	strftime(timebuf, sizeof(timebuf), "%V", &tm);
    return atoi(timebuf);
}

/**
 * @fn	unsigned long getTimeStampFromDate(const std::string& date)
 *
 * @brief	Returns timestamp generated from given date as string
 *
 * @author	Jordi
 * @date	6-7-2015
 *
 * @param	date	Has to be in format YYYY-MM-DDTHH:MM:SS
 *
 * @return	The time stamp from date.
 */

unsigned long getTimeStampFromDate(const std::string& date)
{
	int year, month, day, hour, minute, seconds;
	sscanf(date.c_str(), "%d-%d-%dT%d:%d:%d", &year, &month, &day, &hour, &minute, &seconds);
	struct tm tm;
	tm.tm_hour = hour;
	tm.tm_min = minute;
	tm.tm_sec = seconds;
	tm.tm_mday = day;
	tm.tm_mon = month - 1;
	tm.tm_year = year - 1900;
    return (long)mktime(&tm);
}

unsigned long getTimeStampFromDateAlt(const std::string& date)
{
	int year, month, day, hour, minute, seconds;
	sscanf(date.c_str(), "%4d%2d%2dT%2d%2d%2d", &year, &month, &day, &hour, &minute, &seconds);
	struct tm tm;
	tm.tm_hour = hour;
	tm.tm_min = minute;
	tm.tm_sec = seconds;
	tm.tm_mday = day;
	tm.tm_mon = month - 1;
	tm.tm_year = year - 1900;
	return (long)mktime(&tm);
}

/**
 * @fn	std::string getTimeStampFromDateAsString(const std::string& date)
 *
 * @brief	Returns timestamp generated from given date as string	
 *
 * @author	Jordi
 * @date	6-7-2015
 *
 * @param	date	Has to be in format YYYY-MM-DDTHH:MM:SS
 *
 * @return	The time stamp from date as string.
 */

std::string getTimeStampFromDateAsString(const std::string& date)
{
	unsigned long timestamp = getTimeStampFromDate(date) * 1000;
	char timestampStr[64];
	int n = sprintf(timestampStr, "%lu", timestamp);
	timestampStr[n] = '\0';
	return std::string(timestampStr);
}

/**
 * @fn	const std::string currentDateTime()
 *
 * @brief	Returns current date	
 *
 * @author	Jordi
 * @date	6-7-2015
 *
 * @return	Date in format: YYY-MM-DD HH:MM:SS
 */

const std::string currentDateTime()
{
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    return buf;
}

/**
* @fn	const std::string formatDateTime()
*
* @brief	Returns formatted date string
*
* @author	Jordi
* @date	30-8-2015
*
* @param	now	time as seconds past the UNIX epoch
*
* @return	Date in format: YYY-MM-DD HH:MM:SS
*/

const std::string formatDateTime(time_t now)
{
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &tstruct);
	return buf;
}

