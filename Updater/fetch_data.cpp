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
 * @file	fetch_data.cpp
 *
 * @brief	File holds all methods related to fetching data with curl
 */

#include "includes.h"
#include <gumbo.h>

// This struct holds data about how much weeks of data is available, this seems to be a different value for each department
struct week_t
{
	std::string departmentStringId;
	int amountOfWeeks;
	std::string cpath;
};
ThreadSafeObject <week_t> weekAmountList;

/**
 * @fn	bool doFetchClassList(const char *url_str, const int departmentId, const std::string& cpath, ThreadSafeQueue<Class> &classList)
 *
 * @brief	Fetches a list of all classes from given url, puts result in classList, Thread safe
 *
 * @author	Jordi
 * @date	6-7-2015
 *
 * @param	url_str			The URL string.
 * @param	departmentId	Identifier for the department.
 * @param	cpath			The cpath.
 * @param	classList		Pointer to ThreadSafe object which holds the classes
 *
 * @return	true if it succeeds, false if it fails.
 */

bool doFetchClassList(const char *url_str, const int departmentId, const std::string& cpath, ThreadSafeQueue<Class> *classList)
{
	std::stringstream ss;
	ss << url_str << cpath; // concat cpath to url_str
	std::string url_part = ss.str();
	const char *url = url_part.c_str();
	//const char *url = ss.str().c_str(); // allocate url buffer for curl
	std::string buffer; // destination buffer where the CURL callback will write to
	CURL *curl;
	CURLcode res;

	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
	curl_easy_setopt(curl, CURLOPT_REFERER, NHL_REFERER);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, CURL_TIMEOUT); // 5 sec time out on whole request
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, CURL_CONNECT_TIMEOUT); // 10 sec connect time out
	res = curl_easy_perform(curl);

	if (res == CURLE_OK)
	{
		static const std::regex ip_regex("<option (.*)</option>");
		std::sregex_iterator it(buffer.begin(), buffer.end(), ip_regex);
		std::sregex_iterator end;
		// lock classlist
		std::string departmentStr = getStringBetween("filter=", "&obj", (const char *)url);
		classList->lock();
		std::string lastValue = "";
		while (it != end)
		{
			std::smatch match_class = *it;
			std::string line = match_class.str();
			std::string classStr = getStringBetween("value=\"", "\"", line);
			lastValue = classStr;
			if (classStr.length() > 4){ // TODO: This is a fix so the week option values are not being included, find something solid for this
				std::string nameStr = getStringBetween("\" >", "</option>", line);
				
				classList->push(Class(nameStr, classStr, departmentId, cpath, departmentStr));
			}
			it++;
		}
		
		classList->unlock();
		weekAmountList.lock();
		int amount = atoi(lastValue.c_str());
		weekAmountList.push_back({ departmentStr, amount, cpath });
		//printf("Added %s(%s) - %d\n", departmentStr.c_str(), cpath.c_str(), amount);
		weekAmountList.unlock();
		// department has lastValue amount of weeks
		curl_easy_cleanup(curl);
		return true;
	}
	std::cout << "doFetchClassList() ERROR: " << curl_easy_strerror(res) << std::endl;
	curl_easy_cleanup(curl);
	return false;
}

/**
* @fn	GumboNode *GetTBodyNode(GumboNode *node)
*
* @brief	Recursive method which returns the tbody node
*
* @author	Jordi
* @date	17-10-2015
*
* @param	node	Rootnode which contains a tbody
*
* @return	returns node if found else NULL
*/

GumboNode *GetTBodyNode(GumboNode *node){
	if (node->type != GUMBO_NODE_ELEMENT) {
		return NULL;
	}
	if (node->v.element.tag == GUMBO_TAG_TBODY){
		return node;
	}
	GumboVector* children = &node->v.element.children;
	for (unsigned int i = 0; i < children->length; ++i) {
		GumboNode *node1 = static_cast<GumboNode*>(children->data[i]);
		GumboNode *result = GetTBodyNode(node1);
		if (result != NULL)
			return result;
	}
	return NULL;
}

/**
* @fn	std::string GetTextFromElement(GumboNode *node
*
* @brief	Returns text from a given element
*
* @author	Jordi
* @date	17-10-2015
*
* @param	node	Node which contains text
*
* @return	returns text if found, else an empty string
*/

std::string GetTextFromElement(GumboNode *node){
	if (node == NULL)
		return "";
	if (node->v.element.children.length > 0){
		GumboNode *node1 = static_cast<GumboNode*>(node->v.element.children.data[0]);
		std::string resultT = replaceAll(node1->v.text.text, "\n", " ");
		std::string result = trim(resultT); // TODO: Replace \n with spaces
		return result;
	}
	else
		return "";
}

/**
 * @fn	bool doFetchLesson(Class t, ThreadSafeQueue<Class> *classList, std::atomic<int> *totalLessons)
 *
 * @brief	Fetches a list of all lessons from given url, puts result in classFinalList
 * 			Thread safe
 *
 * @author	Jordi
 * @date	6-7-2015
 *
 * @param	t	The Class to process.
 * @param	classList	Pointer to thread safe object which contains a list of classes
 * @param	totalLessons Pointer to an atomic integer which is needed to increment the amount of lessons we fetch
 *
 * @return	true if it succeeds, false if it fails.
 */

bool doFetchLesson(Class t, ThreadSafeQueue<Class> *classList, std::atomic<int> *totalLessons)
{
	std::string buffer;
	CURL *curl;
	CURLcode res;
	
	weekAmountList.lock();
	int weekAmount = -1;
	for (int i = 0; i < weekAmountList.size(); i++)
	{
		if (weekAmountList.at(i).departmentStringId == t.departmentString() && weekAmountList.at(i).cpath == t.cpath())
		{
			weekAmount = weekAmountList.at(i).amountOfWeeks;
			break;
		}
	}
	if(weekAmount == -1)
		printf("ERROR: NOT FOUND: %s - %s\n", t.departmentString().c_str(), t.cpath().c_str());
	weekAmountList.unlock();
	
	char postFields[1024] = "";
	for (int i = 1; i < weekAmount+1; i++)
	{
		int n = sprintf(postFields, "%sweken[]=%d&", postFields, i);
		postFields[n] = '\0';
	}
	int n = sprintf(postFields, "%ssleutelveld=%s&object=%s&filter=%s", postFields, t.classIdString().c_str(), t.cpath().c_str(), t.departmentString().c_str());
	postFields[n] = '\0';
	//printf("%s\n", postFields);
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, "https://rooster.nhl.nl/1516/rooster.php");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields);
	//curl_easy_setopt(curl, CURLOPT_REFERER, NHL_REFERER);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, CURL_TIMEOUT); // 5 sec time out on whole request
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, CURL_CONNECT_TIMEOUT); // 10 sec connect time out
	res = curl_easy_perform(curl);
	//printf("------\n%s------\n\n", buffer.c_str());
	if (res == CURLE_OK)
	{
		GumboOutput* output = gumbo_parse(buffer.c_str());
		GumboNode* node = GetTBodyNode(output->root);
		if (node == NULL)
		{
			if (t.cpath() == "ttstud"){
				printf("[%s] FAIL(%s-%s-%s)\n", currentDateTime().c_str(), t.className().c_str(), t.departmentString().c_str(), t.cpath().c_str());
				curl_easy_cleanup(curl);
			}
			else{
				printf("[%s] FAIL(%s-%s-%s), aborting program\n", currentDateTime().c_str(), t.className().c_str(), t.departmentString().c_str(), t.cpath().c_str());
				curl_easy_cleanup(curl);
				exit(1);
			}
			return true;
		}
		GumboVector* children = &node->v.element.children;
		bool newDay = false;
		int lessonAmount = 0;
		int yearOffset = -1;
		int titleOffset = -1;
		int locationOffset = -1;
		int teacherOffset = -1;
		int typeOffset = -1;
		int commentOffset = -1;
		int endOffset = -1;
		std::string date;
		char dayName[128];
		int day;
		int month;
		int year;
		for (unsigned int i = 0; i < children->length; ++i) {
			GumboNode *node1 = static_cast<GumboNode*>(children->data[i]);
			if (node1->v.element.tag == GUMBO_TAG_TR){

				GumboAttribute *att = gumbo_get_attribute(&node1->v.element.attributes, "class");
				if (att)
				{
					//printf("TR CLASS: %s\n", att->value);
					std::string value = att->value;
					if (value == "datarij")
					{
						std::string yearType = "";
						GumboNode *startTimeNode = static_cast<GumboNode*>(node1->v.element.children.data[0]);
						std::string endDate = "";
						if (endOffset != -1)
						{
							GumboNode *endTimeNode = static_cast<GumboNode*>(node1->v.element.children.data[endOffset]);
							endDate = GetTextFromElement(endTimeNode);
						}
						if (yearOffset != -1)
						{
							GumboNode *yearNode = static_cast<GumboNode*>(node1->v.element.children.data[yearOffset]); // optional
							if (yearNode)
								yearType = GetTextFromElement(yearNode);
						}
						std::string typeStr = "";
						GumboNode *titleNode = static_cast<GumboNode*>(node1->v.element.children.data[titleOffset]);
						GumboNode *locationNode = static_cast<GumboNode*>(node1->v.element.children.data[locationOffset]);
						std::string teacher = "";
						if (teacherOffset != -1)
						{
							GumboNode *teacherNode = static_cast<GumboNode*>(node1->v.element.children.data[teacherOffset]);
							teacher = GetTextFromElement(teacherNode);
						}
						//if(teacher == "" && t.departmentString() == "TEE" && t.cpath() == "stud"){
						//printf("Teacher empty: %s - %s\n", GetTextFromElement(titleNode).c_str(), GetTextFromElement(startTimeNode).c_str());
						//printf("Buffer:\n%s\n----\n", buffer.c_str());
						//}
						//printf("%s\n", teacher.c_str());
						if (typeOffset != -1)
						{							
							GumboNode *typeNode = static_cast<GumboNode*>(node1->v.element.children.data[typeOffset]);
							typeStr = GetTextFromElement(typeNode);
						}
						GumboNode *commentsNode = static_cast<GumboNode*>(node1->v.element.children.data[commentOffset]);
						std::string startDate = GetTextFromElement(startTimeNode);
						int startHour;
						int startMinute;
						sscanf(startDate.c_str(), "%02d:%02d", &startHour, &startMinute);
						char newStartDate[128];
						//YYYY-MM-DDTHH:MM:SS
						int n = sprintf(newStartDate, "%04d-%02d-%02dT%02d:%02d:00", year, month, day, startHour, startMinute);
						newStartDate[n] = '\0';

						char newEndDate[128];
						if (endDate != "") {
							int endHour;
							int endMinute;
							sscanf(endDate.c_str(), "%02d:%02d", &endHour, &endMinute);
							//YYYY-MM-DDTHH:MM:SS
							int ne = sprintf(newEndDate, "%04d-%02d-%02dT%02d:%02d:00", year, month, day, endHour, endMinute);
							newEndDate[ne] = '\0';
						}else
							newEndDate[0] = '\0';

						//printf("%s - %s - %s - %s - %s (YEAR: %s)\n", GetTextFromElement(titleNode).c_str(), newStartDate, newEndDate, GetTextFromElement(teacherNode).c_str(), GetTextFromElement(locationNode).c_str(), yearType.c_str());
						int weekNr = getWeekNrFromDate(newStartDate);

						// Calculate time difference to remove old lessons
						time_t timeStampThisWeek = getTimeStampFromDate(newStartDate);
						time_t timeStampCurrentWeek;
						time(&timeStampCurrentWeek);
						//printf("Current week: %d - Lesson week: %d\n", getCurrentWeekNumber(), weekNr);
						double diff = difftime(timeStampThisWeek, timeStampCurrentWeek);
						double weeks = diff / 604800;
						//printf("Time difference: %.f\n", weeks);
						if (weeks > -2)
						{ // ignore old lessons
							t.addLesson(shared_ptr<Lesson>(new Lesson(GetTextFromElement(titleNode), GetTextFromElement(commentsNode), teacher, replaceAll(GetTextFromElement(locationNode), "    ", ", "), newStartDate, newEndDate, weekNr, yearType, typeStr)));
							++*totalLessons;
						}
						lessonAmount++;
					}
					else if (value == "weekheader")
					{
						yearOffset = -1;
						titleOffset = -1;
						locationOffset = -1;
						teacherOffset = -1;
						typeOffset = -1;
						commentOffset = -1;
						GumboVector* children = &node1->v.element.children;
						for (unsigned int i = 0; i < children->length; ++i)
						{
							GumboNode *child = static_cast<GumboNode*>(children->data[i]);
							std::string text = GetTextFromElement(child);
							if (text == "jaar")
								yearOffset = i;
							else if (text == "activiteit")
								titleOffset = i;
							else if (text == "lokaal")
								locationOffset = i;
							else if (text == "docent(en)" || text == "klas(en)")
								teacherOffset = i;
							else if (text == "werkvorm")
								typeOffset = i;
							else if (text == "opmerkingen")
								commentOffset = i;
							else if (text == "eind")
								endOffset = i;
						}
					}
				}
				else
				{
					// no class
					GumboNode *td = static_cast<GumboNode*>(node1->v.element.children.data[0]);
					if (td->v.element.tag == GUMBO_TAG_TD) {
						GumboAttribute *classAtt = gumbo_get_attribute(&td->v.element.attributes, "class");
						if (classAtt)
						{
							std::string dayRow = classAtt->value;
							if (dayRow == "dagrij")
							{
								GumboNode *dateNode = static_cast<GumboNode*>(td->v.element.children.data[0]);
								date = dateNode->v.text.text; // get date with sscanf

								sscanf(date.c_str(), "%s %02d-%02d-%04d", &dayName, &day, &month, &year);
								//printf("New day @ %s\n", date.c_str());
							}
						}
					}
				}
			}
		}
		//printf("Lessons: %d\n", lessonAmount);
		gumbo_destroy_output(&kGumboDefaultOptions, output);
		curl_easy_cleanup(curl);
		classList->lock();
		classList->push(t);
		classList->unlock();
		return true;
	}
	else{
		//printf("[%s] Fail: %s, aborting program", currentDateTime().c_str(), curl_easy_strerror(res));
		curl_easy_cleanup(curl);
		//exit(1);
		return false;
	}
		/* New ICAL style as of semester starting at 01-09-2015 */
		/*
		icalcomponent *rootNode = icalparser_parse_string(cstr);
		icalcomponent *comp = icalcomponent_get_first_component(rootNode, ICAL_VEVENT_COMPONENT);
		//icalcomponent *zoneComp = icalcomponent_get_first_component(rootNode, ICAL_VTIMEZONE_COMPONENT);
		//icaltimezone *zone = icaltimezone_get_builtin_timezone("Europe/Amsterdam");
	
		//icalcomponent *next = icalcomponent_get_next_component(rootNode, ICAL_VEVENT_COMPONENT);
		while (comp != NULL){
			//printf("%s\n", icalcomponent_as_ical_string(comp));
			std::string summary = icalcomponent_get_summary(comp);
			icaltimetype dtstart = icalcomponent_get_dtstart(comp);
			
			std::string starttime = formatDateTime(getTimeStampFromDateAlt(icaltime_as_ical_string(dtstart)));
			icaltimetype dtend = icalcomponent_get_dtend(comp);

			std::string endtime = formatDateTime(getTimeStampFromDateAlt(icaltime_as_ical_string(dtend)));
			const char *locationStr = icalcomponent_get_location(comp); // can be null
			std::string location = "";
			if (locationStr){
				location = locationStr;
				location = replaceAll(location, "    ", ", ");
			}
			std::string commentStr = (char *)icalcomponent_get_comment(comp);
			printf("Comment: %s\n", commentStr.c_str());
			char *comment = (char *)commentStr.c_str();
			char *line = strtok(comment, "\n");
			std::string docenten;
			while (line != NULL){
				if (strstr(line, "Docent(en): ")){
					line += 12; // 12 is length of Docent(en)
					int length = strlen(line);
					docenten = line;
				}
				line = strtok(NULL, "\n");
			}
			int weekNr = getWeekNrFromDate(starttime);
			//printf("%s - %s\n", summary.c_str(), starttime.c_str());
			if (weekNr > getCurrentWeekNumber() - 2) { // ignore old lessons
				t.addLesson(shared_ptr<Lesson>(new Lesson(summary, summary, docenten, location, starttime, endtime, weekNr)));
				printf("1Add Lesson(%s): %s - %s - %s - %s(%s)\n", t.className().c_str(), summary.c_str(), starttime.c_str(), endtime.c_str(), docenten.c_str(), commentStr.c_str());
			}
			icalcomponent_free(comp);
			comp = icalcomponent_get_next_component(rootNode, ICAL_VEVENT_COMPONENT);
		}
		icalcomponent_free(comp);
		icalcomponent_free(rootNode);
		classList->lock();
		classList->push(t);
		classList->unlock();
	*/
		// delete our garbage
		//delete[] refererUrlBuffer;
		//delete[] urlBuffer;
		//delete[] cstr;
		//curl_free(classIdStringEscaped);
		//curl_easy_cleanup(curl);

		//return true;
		/* Old XML Style
		xml_document<> doc;
		doc.parse<0>(cstr);
		xml_node<> *pRoot = doc.first_node();
		if (pRoot == 0)
		{
			std::cout << "doFetchLesson() ERROR: Invalid rootnode" << std::endl;
			exit(1); // Immediately abort program as the document is unreadable
		}
		else if (pRoot != NULL)
		{
			pRoot = pRoot->first_node();
			if (pRoot == 0)
			{
				std::cout << "doFetchLesson() ERROR: Rootnode has an invalid first node" << std::endl;
				exit(1); // Immediately abort program as the document is unreadable
			}
		}
		for (xml_node<> *pNode = pRoot->first_node("item"); pNode; pNode = pNode->next_sibling())
		{
			std::string title = pNode->first_node("title") ? pNode->first_node("title")->value() : "";
			if (title.length() > 2)
				title = title.substr(title.find(": ") + 2, title.length());
			std::string description = pNode->first_node("description") ? pNode->first_node("description")->value() : "";
			std::string teacher = getStringBetween(" - ", " -", description, "([a-zA-Z,. ]+)");

			std::string location = pNode->first_node("ev:location") ? pNode->first_node("ev:location")->value() : "";
			location = trim(location);
			char *locationDecoded = curl_easy_unescape(curl, location.c_str(), 0, NULL);
			location = std::string(locationDecoded);
			curl_free(locationDecoded);
			std::string startdate = pNode->first_node("ev:startdate") ? pNode->first_node("ev:startdate")->value() : "";
			std::string enddate = pNode->first_node("ev:enddate") ? pNode->first_node("ev:enddate")->value() : "";
			int weekNr = getWeekNrFromDate(startdate);
			if (weekNr > getCurrentWeekNumber() - 2) // ignore old lessons
				t.addLesson(shared_ptr<Lesson>(new Lesson(title, description, teacher, location, startdate, enddate, weekNr)));
		}
		// ready to push back the class
		classList->lock();
		classList->push_back(t);
		classList->unlock();

		// delete our garbage
		delete[] refererUrlBuffer;
		delete[] urlBuffer;
		delete[] cstr;
		curl_free(classIdStringEscaped);
		curl_easy_cleanup(curl);
		
		return true;*/
}
