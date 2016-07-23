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
 * @file	include\Class.h
 *
 * @brief	ODB class of a class
 */

#ifndef CLASS_H
#define CLASS_H

#include <string>
#include <vector>
#include <odb/tr1/memory.hxx>
using std::tr1::shared_ptr;
#include <odb/core.hxx>
#include "Lesson.h"
#include "ChangeItem.h"
#pragma db object

/**
 * @class	Class
 *
 * @brief	A class.
 *
 * @author	Jordi
 * @date	6-7-2015
 */

class Class
{
public:
    Class(std::string className, std::string classIdString, int departmentId, std::string cpath, std::string departmentString) : _className(className), _classIdString(classIdString), _departmentId(departmentId), _cpath(cpath), _departmentString(departmentString) {};
    void print();
    const std::string& cpath()
    {
        return this->_cpath;
    }
    const std::string& className()
    {
        return this->_className;
    }

    const std::string& classIdString()
    {
        return this->_classIdString;
    }

    const int departmentId()
    {
        return this->_departmentId;
    }

    const std::string& departmentString()
    {
        return this->_departmentString;
    }

	std::vector<shared_ptr<ChangeItem> > getChanges()
	{
		return _changeList;
	}

	std::vector<shared_ptr<Lesson> > getLessons()
    {
        return _lessons;
    }

    bool contains(Lesson* l)
    {
        for(size_t i=0; i<_lessons.size(); i++)
        {
	        if (_lessons.at(i).get()->equals(l))
            {
                return true;
            }
        }
        return false;
    }

    void addLesson(shared_ptr<Lesson> l)
    {
        _lessons.push_back(l);
    }


	void addChange(shared_ptr<ChangeItem> l)
	{
		_changeList.push_back(l);
	}

    int getId() const {
        return _id;
    }

    void removeLesson(const int lessonId)
    {
        int index = -1;
        for(size_t i=0; i<_lessons.size(); i++)
        {
            if(_lessons.at(i).get()->getId()== lessonId)
            {
                index = i;
                break;
            }
        }
        if(index != -1)
            _lessons.erase(_lessons.begin()+index);
    }

    void clearLessons()
    {
        _lessons.clear();
    }

    int lessonCount()
    {
        return _lessons.size();
    }
	

protected:
private:
    Class() {};
    friend class odb::access;
#pragma db id auto
    unsigned long _id;

    std::string _className;
    std::string _classIdString;
    int _departmentId;
    std::string _cpath; // type of schedule, stud -> student, doc -> teacher, ttstud -> exams
    std::string _departmentString;
	
	std::vector<shared_ptr<Lesson> > _lessons;

	std::vector<shared_ptr<ChangeItem> > _changeList;
};

#endif // CLASS_H
