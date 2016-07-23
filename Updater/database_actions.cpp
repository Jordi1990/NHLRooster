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
 * @file	database_actions.cpp
 *
 * @brief	This file holds all methods related to database actions, all these methods should be wrapped in odb_retry helper
 */

#include "includes.h"
extern std::unique_ptr<database> db;
extern std::vector<Class> classInMemList;
extern std::mutex classInMemListMutex;

/**
 * The following code contains functions which have to be wrapped in odb_retry at all times, all parameters act as references
 * The code CAN throw odb exceptions, which will be caught in odb_retry
 */

/**
 * @fn	void insertClassInDB(Class &c)
 *
 * @brief	Inserts given class into database(including lessons)
 *
 * @author	Jordi
 * @date	6-7-2015
 *
 * @param [in,out]	c	The Class &amp; to process.
 */

void insertClassInDB(Class &c){
	transaction tr(db->begin());
	//db->execute("SET FOREIGN_KEY_CHECKS=0"); // Disable checks because they are not needed at this time and are very performance intensive
	//db->execute("SET AUTOCOMMIT=0"); // Do not autocommit, we make one huge transaction to improve insert times
	for (shared_ptr<ChangeItem> ci : c.getChanges()) { // iterate through all lessons
												  //printf("Persisting %s - %s - %s\n", l.get()->title().c_str(), l.get()->startdate().c_str(), l.get()->teacher().c_str());
		db->persist(ci.get()); // persist lesson
	}
	for (shared_ptr<Lesson> l : c.getLessons()) { // iterate through all lessons
		//printf("Persisting %s - %s - %s\n", l.get()->title().c_str(), l.get()->startdate().c_str(), l.get()->teacher().c_str());
		db->persist(l.get()); // persist lesson
	}
	db->persist(c); // persist class
	//db->execute("SET FOREIGN_KEY_CHECKS=1"); // Enable checks in database again
	tr.commit();
}

/**
 * @fn	void revertInsertClassInDB(Class &c)
 *
 * @brief	Reverts a failed insertClassInDB(), removes lessons and class from database
 *
 * @author	Jordi
 * @date	6-7-2015
 *
 * @param [in,out]	c	The Class to process.
 */

void revertInsertClassInDB(Class &c){
	try
	{
		transaction tr(db->begin());
		for (shared_ptr<Lesson> l : c.getLessons()) // remove all lessons from this class
			db->erase(l.get());
		db->erase(c); // remove class
		tr.commit();
	}
	catch (odb::object_not_persistent)
	{
		// not much, object didn't exist so everything is OK
	}
}

/**
 * @fn	void persistClass(Class &c)
 *
 * @brief	Inserts given class into database
 *
 * @author	Jordi
 * @date	6-7-2015
 *
 * @param [in,out]	c	The Class to process.
 */

void persistClass(Class &c){
	transaction tr(db->begin());
	db->persist(c); // persist the class
	tr.commit();
}

/**
 * @fn	void persistLessons(shared_ptr<Lesson> &l, Class &newClass)
 *
 * @brief	Inserts given lessons into database and connects it to given class
 *
 * @author	Jordi
 * @date	6-7-2015
 *
 * @param [in,out]	l			The lesson as shared_ptr to process.
 * @param [in,out]	c	Class where lesson will be connected to
 */

void persistLessons(shared_ptr<Lesson> &l, Class &c){
	transaction tr(db->begin());
	db->persist(l.get()); // persist lesson into database
	c.addLesson(l); // add lesson to our class
	db->update(c); // update class structure so it links to our new lesson
	tr.commit();
}

/**
 * @fn	void updateExistingClass(shared_ptr<Lesson> &l, Class &c)
 *
 * @brief	Inserts a new lesson into database and connects it to given class from classInMemList
 *			Thread safe
 *
 * @author	Jordi
 * @date	6-7-2015
 *
 * @param [in,out]	l	The lesson as shared_ptr to process.
 * @param [in,out]	c	The Class to process.
 */

void updateExistingClass(shared_ptr<Lesson> &l, Class &c){
	transaction tr(db->begin());
	db->persist(l.get()); // persist lesson in database
	c.addLesson(l); // add lesson to our class
	db->update(c); // update class structure so it links to our new lesson
	tr.commit();
}

/**
 * @fn	void removeFromDatabase(int &id, Class &c)
 *
 * @brief	Removes given lesson id from database and also removes the connection to its class
 *
 * @author	Jordi
 * @date	6-7-2015
 *
 * @param [in,out]	id	The identifier.
 * @param [in,out]	c 	The Class to process.
 */

void removeFromDatabase(int &id, Class &c){
	typedef odb::query<Lesson> query;
	transaction tr(db->begin());
	db->erase_query<Lesson>(query::id == id);
	db->update(c);
	tr.commit();
}

/**
 * @fn	void rebuildDatabase()
 *
 * @brief	Rebuilds database from scratch, removes classes and lessons, reinserts all hardcoded
 * 			departments.
 *
 * @author	Jordi
 * @date	6-7-2015
 */

void rebuildDatabase(){
	transaction tr(db->begin());
	db->erase_query<department>();
	shared_ptr<department> ecc(new department("Communicatie", "ECC"));
	shared_ptr<department> ecd(new department("Docent Theater BA", "ECD"));
	shared_ptr<department> ece(new department("Exacte Vakken", "ECE"));
	shared_ptr<department> ecf(new department("Sociale Vakken(deeltijd)", "ECF"));
	shared_ptr<department> ecs(new department("Sociale Vakken(voltijd)", "ECS"));
	shared_ptr<department> eck(new department("Beeldende Kunst en Vormgeving", "ECK"));
	shared_ptr<department> ecp(new department("Pabo", "ECP"));
	shared_ptr<department> ect(new department("Talen", "ECT"));
	shared_ptr<department> em(new department("Economie & Management", "EM"));
	shared_ptr<department> mas(new department("Masters", "MAS"));
	shared_ptr<department> mpe(new department("Master Pedagogiek", "MPE"));
	shared_ptr<department> teb(new department("Built Environment", "TEB"));
	shared_ptr<department> tee(new department("Engineering", "TEE"));
	shared_ptr<department> tem(new department("Maritieme Techniek", "TEM"));
	shared_ptr<department> zwv(new department("Verpleegkunde", "ZWV"));
	shared_ptr<department> zww(new department("Welzijnsopleidingen", "ZWW"));
	shared_ptr<department> miwb(new department("Maritiem Instituut Willem Barentsz", "MIWB"));
	db->persist(ecc.get());
	db->persist(ecd.get());
	db->persist(ece.get());
	db->persist(ecf.get());
	db->persist(ecs.get());
	db->persist(eck.get());
	db->persist(ecp.get());
	db->persist(ect.get());
	db->persist(em.get());
	db->persist(mas.get());
	db->persist(mpe.get());
	db->persist(teb.get());
	db->persist(tee.get());
	db->persist(tem.get());
	db->persist(zwv.get());
	db->persist(zww.get());
	db->persist(miwb.get());

	tr.commit();
}

/**
 * @fn	bool loadFromDatabase()
 *
 * @brief	Loads all classes from database into memory
 *			Used so we can match new lessons to our old lessons
 *
 * @author	Jordi
 * @date	6-7-2015
 */

void loadFromDatabase(std::vector<Class> *classListFromDB){
	typedef odb::result<Class> result;
	int lessonCount = 0;
	int changesCount = 0;
	transaction tr(db->begin());
	result r(db->query<Class>());
	for (result::iterator i(r.begin()); i != r.end(); ++i)
	{
		Class c = *i;
		changesCount += c.getChanges().size;
		lessonCount += c.lessonCount();
		classListFromDB->push_back(c);
	}
	tr.commit();
	std::cout << "From DB -> Lessons: " << lessonCount << " in " << classListFromDB->size() << " classes, " << changesCount << " changes" << std::endl;
}
