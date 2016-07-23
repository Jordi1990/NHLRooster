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
* @file	include\ChangeItem.h
*
* @brief	ODB class of a ChangeItem
*/

#ifndef CHANGEITEM_H
#define CHANGEITEM_H

#include <string>
#include <vector>
#include <odb/tr1/memory.hxx>
using std::tr1::shared_ptr;
#include <odb/core.hxx>
#pragma db object

/**
* @class	ChangeItem
*
* @brief	An item which holds changes to lessons.
*
* @author	Jordi
* @date	6-7-2015
*/

class ChangeItem
{
public:
	ChangeItem(std::string title, std::string description, std::string teacher, std::string location, std::string startdate, std::string enddate, int weekNr, std::string year, std::string type, std::string change)
		: _title(title), _description(description), _teacher(teacher), _location(location), _startdate(startdate), _enddate(enddate), _weekNr(weekNr), _year(year), _type(type), _change(change) {}

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

	const std::string& change() {
		return this->_change;
	}

	int getId() const
	{
		return _id;
	}
protected:
private:
	ChangeItem() {};
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
	std::string _change;
	int _weekNr;
};

#endif // CHANGEITEM_H
