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
 * @file	include\Lesson.h
 *
 * @brief	ODB class of a lesson
 */

#ifndef LESSON_H
#define LESSON_H
#include <string>
#include <memory>
#include <odb/tr1/memory.hxx>
#include <iostream>
using std::tr1::shared_ptr;
#include <odb/core.hxx>

#pragma db object

/**
 * @class	Lesson
 *
 * @brief	A lesson.
 *
 * @author	Jordi
 * @date	6-7-2015
 */

class Lesson
{
public:
	Lesson(std::string title, std::string description, std::string teacher, std::string location, std::string startdate, std::string enddate, int weekNr, std::string year, std::string type)
		: _title(title), _description(description), _teacher(teacher), _location(location), _startdate(startdate), _enddate(enddate), _weekNr(weekNr), _year(year), _type(type) {}
	int getSize()
	{
		return _title.size() + _description.size() + _teacher.size() + _location.size() + _startdate.size() + _enddate.size();
	}

	const std::string& title()
	{
		return this->_title;
	}

	const std::string& description()
	{
		return this->_description;
	}

	const std::string& teacher()
	{
		return this->_teacher;
	}

	const std::string& location()
	{
		return this->_location;
	}

	const std::string& startdate()
	{
		return this->_startdate;
	}

	const std::string& enddate()
	{
		return this->_enddate;
	}

	const std::string& year()
	{
		return this->_year;
	}

	const std::string& type() {
		return this->_type;
	}

	int getId() const
	{
		return _id;
	}

	int getWeeknr() const {
		return _weekNr;
	}

	bool equals(Lesson* l) const
    {
	    return (l->title() == this->_title && /*l->teacher() == this->_teacher &&*/ l->location() == this->_location && l->startdate() == this->_startdate && l->enddate() == this->_enddate);
    }
protected:
private:
	Lesson() {};
    friend class odb::access;
#pragma db id auto
    unsigned long _id;

    std::string _title;
    std::string _description;
    std::string _teacher;
    std::string _location;
    std::string _startdate;
    std::string _enddate;
	std::string _year;
	std::string _type;
    int _weekNr;
};

#endif // LESSON_H
