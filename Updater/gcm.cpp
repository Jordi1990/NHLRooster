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
 * @file	gcm.cpp
 *
 * @brief	File holds all GCM related functions
 */

#include "includes.h"
extern bool dryRun;

/**
 * @fn	std::string getJson(std::vector<Value> changeList, std::vector<Value> registrationIds, Value className)
 *
 * @brief	Gets a JSON.
 *
 * @author	Jordi
 * @date	6-7-2015
 *
 * @param	changeList	   	List of changes.
 * @param	registrationIds	List of GCM registrations identifiers.
 * @param	className	   	Name of the class.
 *
 * @return	The JSON response.
 */

std::string getJson(std::vector<Value> changeList, std::vector<Value> registrationIds, Value className)
{
	Object dataList;
	dataList.push_back(Pair("classname", className));
	dataList.push_back(Pair("changes", changeList));
	Object obj;
	obj.push_back(Pair("registration_ids", registrationIds));
	obj.push_back(Pair("data", dataList));
	if (dryRun)
		obj.push_back(Pair("dry_run", true));
	return write_formatted(obj);
}

/**
 * @fn	bool sendPush(std::vector<Value> registrationIds, std::vector<Value> changeList, Value className)
 *
 * @brief	Sends GCM push messages to all registration identifiers
 *
 * @author	Jordi
 * @date	6-7-2015
 *
 * @param	registrationIds	List of GCM registrations identifiers.
 * @param	changeList	   	List of changes.
 * @param	className	   	Name of the class.
 *
 * @return	true if it succeeds, false if it fails.
 */

bool sendPush(std::vector<Value> registrationIds, std::vector<Value> changeList, Value className)
{
	std::string jsonString = getJson(changeList, registrationIds, className);
	CURL *curl;
	CURLcode res;
	std::string buffer;
	struct curl_slist *chunk = NULL;
	chunk = curl_slist_append(chunk, GCM_AUTH_KEY_HEADER);
	chunk = curl_slist_append(chunk, "Content-Type: application/json");
	curl = curl_easy_init(); // share accross 1 thread
	curl_easy_setopt(curl, CURLOPT_URL, "https://android.googleapis.com/gcm/send");
	curl_easy_setopt(curl, CURLOPT_POST, true);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
	//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonString.c_str());
	res = curl_easy_perform(curl);
	if (res == CURLE_OK)
	{
		std::cout << "Push packet sent" << std::endl;
		std::cout << "Result: " << buffer << std::endl;
		mValue val;
		auto succes = read(buffer, val);
		if (succes){
			try{
				auto obj = val.get_obj().find("results")->second.get_array();
				int n = 0;
				for (auto object : obj){
					mObject field = object.get_obj();
					try{
						std::string errorMessage = field["error"].get_str();
						if (errorMessage == "MissingRegistration" || errorMessage == "InvalidRegistration" || errorMessage == "NotRegistered"){
							std::cout << "GCM ERROR: " << errorMessage << " on registration id: " << registrationIds.at(n).get_str() << std::endl;
						}
						else{
							std::cout << "Recoverable GCM ERROR: " << errorMessage << " on registration id: " << registrationIds.at(n).get_str() << std::endl;
							// TODO: Retry send attempt on all recoverable failed clients
						}
					}
					catch (...){ // if item does not exist it will throw an exception  TODO: Lookup the json_spirit exceptions
						// message_id item can be extracted from the object here, but we do not need it, everything has been sent succesfully
					}
					n++;
				}
			}
			catch (...){ // TODO: Lookup the json_spirit exceptions
				std::cout << "Exception thrown when parsing GCM json response" << std::endl;
			}
		}
		else{
			std::cout << "Failed reading json response" << std::endl;
		}
	}
	else
	{
		fprintf(stderr,
			"curl_easy_perform() failed: %s\n",
			curl_easy_strerror(res));
	}
	curl_easy_cleanup(curl);
	curl_slist_free_all(chunk);
	return (res == CURLE_OK);
}