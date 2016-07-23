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
 * @file	include\includes.h
 *
 * @brief	This file holds all include references and function prototypes
 */

#ifndef INCLUDES_H
#define INCLUDES_H
#pragma warning (disable:4068) // disable odb warnings
#include <iostream>
#include <queue> 
#include <string>
#include <vector>
#include <stdio.h>
#include <iterator>
#include <regex>
#include <algorithm>
#include <sstream>
#include <ctime>
#include <fstream>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/version_info.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <curl/curl.h>
#include "json_spirit/json_spirit.h"
#include <mutex>
#include <rapidxml.hpp>
using namespace rapidxml;
#include <odb/database.hxx>
#include <odb/transaction.hxx>

#include <odb/mysql/database.hxx>
#include <memory>
#include <chrono>
#include <atomic>

// thread safe object templates
#include "ThreadSafeObject.h"
#include "ThreadSafeQueue.h"

// ODB - ORM stuff
#include "pushClients.h"
#include "pushClients-odb.hxx"
#include "department.h"
#include "department-odb.hxx"
#include "Lesson.h"
#include "Lesson-odb.hxx"
#include "Class.h"
#include "Class-odb.hxx"
#include "ChangeItem.h"
#include "ChangeItem-odb.hxx"
#include <boost/version.hpp>
// Include our variables
#include "variables.h"

using namespace odb::core;
using namespace json_spirit;
#include <thread>
#ifdef __linux__
#include <sys/file.h>
#endif
#include <errno.h>

/**
 * @fn	static int writer(char *data, size_t size, size_t nmemb, std::string *writerData)
 *
 * @brief	Callback function for CURL which writes data to our buffer
 *
 * @author	Jordi
 * @date	6-7-2015
 *
 * @param [in,out]	data	  	If non-null, the data.
 * @param	size			  	The size.
 * @param	nmemb			  	The nmemb.
 * @param [in,out]	writerData	If non-null, information describing the writer.
 *
 * @return	An int.
 */

static int writer(char *data, size_t size, size_t nmemb, std::string *writerData)
{
	if (writerData == NULL)
		return 0;

	writerData->append(data, size*nmemb);

	return size * nmemb;
}

// prototypes

//fetch_data.cpp
bool doFetchClassList(const char *url_str, const int departmentId, const std::string& cpath, ThreadSafeQueue<Class> *classList);
bool doFetchLesson(Class t, ThreadSafeQueue<Class> *classList, std::atomic<int> *totalLessons);

//gcm.cpp
std::string getJson(std::vector<Value> changeList, std::vector<Value> registrationIds, Value className);
bool sendPush(std::vector<Value> registrationIds, std::vector<Value> changeList, Value className);

//database_actions.cpp
void insertClassInDB(Class &c);
void revertInsertClassInDB(Class &c);
void persistClass(Class &c);
void persistLessons(shared_ptr<Lesson> &l, Class &newClass);
void updateExistingClass(shared_ptr<Lesson> &l, Class &c);
void removeFromDatabase(int &id, Class &c);
void rebuildDatabase();
void loadFromDatabase(std::vector<Class> *classListFromDB);

//helper
bool isAlreadyRunning();
int executeWorkersTask(std::function<void()> task);
void waitForThreads();
bool odb_retry(std::function<void()> func, size_t max_attempts = ODB_RETRY_ATTEMPTS, std::function<void()> revertFunc = nullptr);
bool retry(std::function<bool()> func, size_t max_attempts = RETRY_ATTEMPTS, unsigned long msDelay = 0);
std::string replaceAll(std::string str, const std::string& from, const std::string& to);
std::string getStringBetween(const std::string& before, const std::string& after, const std::string &line, const std::string regex_between = "(.*)");
inline std::string& ltrim(std::string& string, const char* t = " \t\n\r\f\v");
inline std::string& rtrim(std::string& string, const char* t = " \t\n\r\f\v");
std::string& trim(std::string& string, const char* t = " \t\n\r\f\v");
int getCurrentWeekNumber();
int getWeekNrFromDate(const std::string& date);
unsigned long getTimeStampFromDate(const std::string& date);
std::string getTimeStampFromDateAsString(const std::string& date);
const std::string currentDateTime();
const std::string formatDateTime(time_t now);
unsigned long getTimeStampFromDateAlt(const std::string& date);
#endif