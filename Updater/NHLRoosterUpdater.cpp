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
 * @file	NHLRoosterUpdater.cpp
 *
 * @brief	Holds main entry point and mostly methods to task workers.
 */

#include "NHLRoosterUpdater.h"

/**
 * @fn	void fetch_lesson()
 *
 * @brief	Thread safe loop which fetches lesson list from every class inside classList.
 *
 * @author	Jordi
 * @date	6-7-2015
 */

void fetch_lesson(ThreadSafeQueue<Class> *classListTemp, ThreadSafeQueue<Class> *classList, std::atomic<int> *totalLessons)
{
	while (true)
	{
		classListTemp->lock();
		// is there an item available?
		if (!classListTemp->empty())
		{
			Class c = classListTemp->pop();
			classListTemp->unlock();
			if (!retry(std::bind(doFetchLesson, c, classList, totalLessons), 10, 5000))
			{
				// if all the retry attempts failed, stop the program
				std::cout << "[" << currentDateTime() << "] All retry attempts for doFetchLesson() failed(" << c.className() << ", " << c.departmentString() << "), aborting program" << std::endl;
				exit(1);
			}
		}
		else
		{
			classListTemp->unlock();
			return;
		}
		std::this_thread::sleep_for(std::chrono::microseconds(100)); // Wait a short while, less strain on database, lots of parallel writes to database cause deadlock exceptions in odb
	}
}

/**
 * @fn	void fetch_class_list(department &d)
 *
 * @brief	Fetch student class, teacher class and exam class list of given department
 *
 * @author	Jordi
 * @date	6-7-2015
 *
 * @param [in,out]	d	The department to process.
 */

void fetch_class_list(department &d, ThreadSafeQueue<Class> *classList)
{
	char *urlBuffer = new char[512];
	int size = sprintf(urlBuffer, CLASS_TYPES_SOURCE, d.stringId().c_str());
	urlBuffer[size] = '\0';
	if (!doFetchClassList(urlBuffer, d.id(), "stud", classList) || !doFetchClassList(urlBuffer, d.id(), "doc", classList) || !doFetchClassList(urlBuffer, d.id(), "ttstud", classList)){
		std::cout << "[" << currentDateTime() << "] doFetchClassList() failed, aborting program" << std::endl;
		exit(1);
	}
	delete[] urlBuffer;
}

/**
 * @fn	void fetch_class()
 *
 * @brief	Loop which retrieves class list for every department in departmentList
 * 			Thread safe.
 *
 * @author	Jordi
 * @date	6-7-2015
 */

void fetch_class(ThreadSafeQueue<Class> *classList, ThreadSafeQueue<department> *departmentList)
{
	while (true)
	{
		departmentList->lock();
		// is there an item available?
		if (!departmentList->empty())
		{
			department d = departmentList->pop();
			departmentList->unlock();
			fetch_class_list(d, classList);
		}
		else
		{
			departmentList->unlock();
			return; // cancel thread because list is empty
		}
	}
}

/**
 * @fn	void do_insert()
 *
 * @brief	Insert into database routine Used to rebuild database from scratch, inserts all
 * 			classes from classFinalList into database.
 *
 * @author	Jordi
 * @date	6-7-2015
 */

void do_insert(ThreadSafeQueue<Class> *classList)
{
	//std::cout << "Thread #" << id << " started" << std::endl;
	while (true)
	{
		classList->lock();
		// is there an item available?
		if (!classList->empty())
		{
			Class c = classList->pop();
			classList->unlock();
			odb_retry(std::bind(insertClassInDB, c), 5, std::bind(revertInsertClassInDB, c));
		}
		else
		{
			classList->unlock();
			return;
		}
	}
}

/**
 * @fn	void do_change_detection()
 *
 * @brief	Thread safe loop which removes all entries from classFinalList and processes them
 * 			into a changelist per class.
 *
 * @author	Jordi
 * @date	6-7-2015
 */

void do_change_detection(ThreadSafeObject<change_t> *resultChangeList, std::vector<Class> *classListFromDB, ThreadSafeQueue<Class> *classList)
{

	while (true)
	{
		classList->lock();
		// is there an item available?
		if (!classList->empty())
		{
			Class c = classList->pop();
			classList->unlock();

			int indexClassInMem = -1;
			std::vector<Value> changeList;
			for (size_t i = 0; i < classListFromDB->size(); i++)
			{
				// Compare every class in memory with our new class, if we found a match, remember the index to use it later on				
				if (classListFromDB->at(i).classIdString() == c.classIdString()) {
				// Removed as of 19-6-2015: Seems like there are classes with exactly the same name, cpath and department strings, replaced with classidString
				//if (classInMemList.at(i).className() == c.className() && classInMemList.at(i).departmentString() == c.departmentString() && c.cpath() == classInMemList.at(i).cpath()) {
					indexClassInMem = i;
					break;
				}
			}

			if (indexClassInMem == -1){ // New class does not exist in database
				std::cout << "New class: " << c.className() << ", lessons: " << c.lessonCount() << std::endl;
				Class newClass = Class(c.className(), c.classIdString(), c.departmentId(), c.cpath(), c.departmentString()); // create a new class, without lessons so we can create a new persistant object
				odb_retry(std::bind(persistClass, newClass));
				for (int z = 0; z < c.lessonCount(); z++){
					Lesson *l = c.getLessons().at(z).get();
					//odb_retry(std::bind(persistLessons, c.getLessons().at(z), newClass));
					//printf("New lesson: %s - %s - %s\n", c.getLessons().at(z).get()->title().c_str(), c.getLessons().at(z).get()->location().c_str(), c.getLessons().at(z).get()->startdate().c_str());
					c.addChange(shared_ptr<ChangeItem>(new ChangeItem(l->title(), l->description(), l->teacher(), l->location(), l->startdate(), l->enddate(), l->getWeeknr(), l->year(), l->type(), "ADD")));
					changeList.push_back(getTimeStampFromDateAsString(l->startdate())); // push timestamp as string into our changelist
				}
			}
			else
			{ // class exists in database, so update it
				// move all our changes to new copy
				//std::vector<std::shared_ptr<ChangeItem>> changes = classListFromDB->at(indexClassInMem).getChanges();
				//for (size_t z = 0; z < changes.size;z++) {
				//	c.addChange(changes.at(z));
				//}
				std::vector<shared_ptr<Lesson> > lessons = c.getLessons();
				for (size_t z = 0; z < lessons.size(); z++)
				{
					Lesson *l = lessons.at(z).get();
					if (!classListFromDB->at(indexClassInMem).contains(l))  // if lessons does not exist in our old class, its a new lesson
					{
						/*printf("Class: %s(%s)\nThis class: %s\n", classListFromDB->at(indexClassInMem).className().c_str(), classListFromDB->at(indexClassInMem).cpath().c_str(), c.className().c_str());
						printf("New lesson: %s - %s - %s - %s - %s\n", l->title().c_str(), l->location().c_str(), l->startdate().c_str(), l->enddate().c_str(), l->teacher().c_str());
						for (int i = 0; i < classListFromDB->at(indexClassInMem).getLessons().size(); i++)
						{
							Lesson *oldL = classListFromDB->at(indexClassInMem).getLessons().at(i).get();
							printf("Old lesson: %s - %s - %s - %s - %s\n", oldL->title().c_str(), oldL->location().c_str(), oldL->startdate().c_str(), oldL->enddate().c_str(), oldL->teacher().c_str());
						}*/
						
						//printf("Updating class %s\n", l->title().c_str());
						//odb_retry(std::bind(updateExistingClass, lessons.at(z), classListFromDB->at(indexClassInMem)));
						c.addChange(shared_ptr<ChangeItem>(new ChangeItem(l->title(), l->description(), l->teacher(), l->location(), l->startdate(), l->enddate(), l->getWeeknr(), l->year(), l->type(), "ADD")));
						changeList.push_back(getTimeStampFromDateAsString(l->startdate())); // push timestamp as string into our changelist
					}
				}
			}
			// push our changes into change list, used for push notifications
			if (changeList.size() > 0)
			{
				//printf("Change at %s - %s\n", c.className().c_str(), c.departmentString().c_str());
				resultChangeList->lock();
				resultChangeList->push_back({ changeList, c.className() }); // push change to our list of changes
				resultChangeList->unlock();
			}
			// loop through old ones to remove deleted lessons
			
			if (indexClassInMem != -1) { // If this class is already in the database, check if there are deleted lessons by comparing it to our new lessons
				for (int z = 0; z < classListFromDB->at(indexClassInMem).lessonCount(); z++)
				{
					Lesson *l = classListFromDB->at(indexClassInMem).getLessons().at(z).get();
					if (!c.contains(l))
					{
						//printf("Removed lesson: %s - %s - %s\n", l->title().c_str(), l->location().c_str(), l->startdate().c_str());
						// Removed as of 14-7-2015: Can cause race conditions, because other read actions are not being synchronized
						//classListFromDB->lock();
						//classListFromDB->at(indexClassInMem).removeLesson(l->getId());
						//classListFromDB->unlock();
						//odb_retry(std::bind(removeFromDatabase, l->getId(), classListFromDB->at(indexClassInMem)));

						c.addChange(shared_ptr<ChangeItem>(new ChangeItem(l->title(), l->description(), l->teacher(), l->location(), l->startdate(), l->enddate(), l->getWeeknr(), l->year(), l->type(), "DEL")));
					}
				}
			}
			odb_retry(std::bind(insertClassInDB, c), 5, std::bind(revertInsertClassInDB, c));
		}
		else
		{
			classList->unlock();
			return;
		}
	}
}

/**
 * @fn	int main(int argc, char* argv[])
 *
 * @brief	Main entry-point for this application.
 *
 * @author	Jordi
 * @date	6-7-2015
 *
 * @param	argc	Number of command-line arguments.
 * @param	argv	Array of command-line argument strings.
 *
 * @return	Exit-code for the process - 0 for success, else an error code.
 */

int main(int argc, char* argv[])
{
	if (isAlreadyRunning()){
		printf("Another instance is already running");
		return 0;
	}
	curl_global_init(CURL_GLOBAL_ALL);
	std::cout << "NHLRoosterUpdater " << VERSION_NUMBER << "-" << OS << " - using " << std::thread::hardware_concurrency() << " cores" << std::endl;
	std::cout << "by Jordi Epema" << std::endl;
	std::cout << "" << std::endl;
	std::cout << "Boost version: " << BOOST_LIB_VERSION << std::endl;
	std::cout << "LibODB: " << LIBODB_MYSQL_VERSION_STR << std::endl;
#ifdef LIBMYSQL_VERSION
	std::cout << "LibMySQL: " << LIBMYSQL_VERSION << std::endl;
#endif
	std::cout << "MySQL C++ Connector: " << MYCPPCONN_DM_VERSION << std::endl;
	std::cout << "LibCURL: " << LIBCURL_VERSION << std::endl;
	std::cout << "" << std::endl;
	std::cout << currentDateTime() << std::endl;
	std::cout << "" << std::endl;
	long start = clock();
	try
	{
		std::string oneDepartment = "";
		db = std::unique_ptr<database>(new odb::mysql::database(DB_USER, DB_PASS, DB_NAME, DB_HOST, DB_PORT));
		//typedef odb::query<department> query;
		typedef odb::result<department> result;
		if (argc >= 2)
		{
			if (strcmp(argv[1], "--dry-run") == 0)
			{
				std::cout << "Push messages are disabled" << std::endl;
				dryRun = true;
			}
			else if (strcmp(argv[1], "--rebuild") == 0)
			{
				std::cout << "" << std::endl;
				std::cout << "Doing a complete rebuild of database" << std::endl;
				std::cout << "" << std::endl;
				doRebuild = true;
				odb_retry(rebuildDatabase);
			}
			else if (strcmp(argv[1], "--notify-all") == 0)
			{
				std::cout << "" << std::endl;
				std::cout << "Notifying all users" << std::endl;
				std::cout << "" << std::endl;
				std::vector<Value> changeList;
				transaction tr(db->begin());
				typedef odb::query<Class> query;
				typedef odb::result<Class> result;
				result r(db->query<Class>());
				for (result::iterator id(r.begin()); id != r.end(); ++id)
				{
					Class& c = *id;
					typedef odb::query<pushClients> query;
					typedef odb::result<pushClients> result;
					result r(db->query<pushClients>(query::className == c.className()));
					std::vector<Value> registrationIds;
					for (result::iterator id(r.begin()); id != r.end(); ++id)
					{
						pushClients& d = *id;
						registrationIds.push_back(d.registrationId());
						if (registrationIds.size() > 0)
						{
							sendPush(registrationIds, changeList, c.className());
						}
					}
				}
				
				tr.commit();
				std::cout << "All push messages have been sent" << std::endl;
				return 0;
			}
			else
			{
				std::cout << "Invalid parameter: " << argv[1] << std::endl;
				return -1;
			}
		}

		if (argc >= 4){
			if (strcmp(argv[2], "--department"))
				oneDepartment = argv[3];
		}
		transaction tr(db->begin());
		result r(db->query<department>());
		//Holds list of all departments
		ThreadSafeQueue<department> departmentList;

		for (result::iterator i(r.begin()); i != r.end(); ++i)
		{
			department& d = *i;
			if (oneDepartment != ""){
				if (d.stringId() == oneDepartment)
					departmentList.push_fast(d);
			}
			else{
				//if(d.stringId() == "TEE")
				departmentList.push_fast(d);
			}
		}
		tr.commit();
		std::cout << departmentList.size() << " departments " << std::endl;

		//List of classes, during lesson fetch classes with lessons will be inserted into this list.
		ThreadSafeQueue<Class> classList;

		// Classes with lessons from database
		std::vector<Class> classListFromDB;
		std::atomic<int> totalLessons(0);
		{
			//List of classes, holds only classes during fetch.
			ThreadSafeQueue<Class> classListTemp;

			executeWorkersTask(std::bind(fetch_class, &classListTemp, &departmentList));
			waitForThreads();

			std::cout << classListTemp.size() << " classes" << std::endl;

			executeWorkersTask(std::bind(fetch_lesson, &classListTemp, &classList, &totalLessons));
			// Before waiting for all threads, fetch everything from database

			if (!doRebuild)
			{
				// before load remove all old changes
				typedef odb::query<ChangeItem> changeQuery;
				transaction trChange(db->begin());
				db->erase_query<ChangeItem>(changeQuery::weekNr < getCurrentWeekNumber() - 4); // month old changes get deleted, this will bug at new year :(
				trChange.commit();
				odb_retry(std::bind(loadFromDatabase, &classListFromDB));
			}
			std::cout << "[" << currentDateTime() << "] Waiting for threads to finish" << std::endl;
			waitForThreads();
		} // classListTemp will now be removed in memory, everything should be in classList

		if (doRebuild)
		{
			std::cout << "[" << currentDateTime() << "] Rebuilding... " << std::endl;
			// first empty tables
			transaction tr(db->begin());
			db->erase_query<Lesson>();
			db->erase_query<Class>();
			db->erase_query<ChangeItem>();
			tr.commit();
			int totalThreads = executeWorkersTask(std::bind(do_insert, &classList));
			waitForThreads();
		}
		else
		{
			std::cout << "[" << currentDateTime() << "] Lessons: " << totalLessons.load() << std::endl;
			if (totalLessons.load() < 0) {
				std::cout << "No lessons detected, keep our old database and stop updating" << std::endl;
				return -1;
			}
			// Initialize our thread safe change list
			ThreadSafeObject<change_t> changeList;
			// Change detection, when no change, don't do an insert
			std::cout << "[" << currentDateTime() << "] Change detection... " << std::endl;
			// if item does not exist, add it, also note it as a change
			// change final list
			{
				transaction tr(db->begin());
				db->erase_query<Lesson>();
				db->erase_query<Class>();
				db->erase_query<ChangeItem>();
				tr.commit();
				int totalThreads = executeWorkersTask(std::bind(do_change_detection, &changeList, &classListFromDB, &classList));
				waitForThreads();
			}

			int numChanges = 0;
			for (size_t i = 0; i < changeList.size(); i++)
			{
				numChanges += changeList.at_fast(i).changeList.size();
			}

			std::cout << "[" << currentDateTime() << "] " << numChanges << " changes detected" << std::endl;
			std::cout << "[" << currentDateTime() << "] Sending push messages" << std::endl;
			for (size_t i = 0; i < changeList.size(); i++)
			{
				std::string className = changeList.at_fast(i).className;
				transaction tr(db->begin());

				typedef odb::query<pushClients> query;
				typedef odb::result<pushClients> result;
				result r(db->query<pushClients>(query::className == className));
				std::vector<Value> registrationIds;
				std::cout << "Sending to " << className << std::endl;
				for (result::iterator id(r.begin()); id != r.end(); ++id)
				{
					pushClients& d = *id;
					registrationIds.push_back(d.registrationId());
				}
				tr.commit();
				if (registrationIds.size() > 0)
				{
					sendPush(registrationIds, changeList.at_fast(i).changeList, className);
				}
			}
		}
		/* end */
		std::cout << "[" << currentDateTime() << "] END" << std::endl;
		system("PAUSE");
	}
	catch (sql::SQLException &e)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}
	return 0;
}
