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
 * @file	include\department.h
 *
 * @brief	ODB class of a department
 */

#ifndef DEPARTMENT_H
#define DEPARTMENT_H
#include <string>
#include <odb/core.hxx>

#pragma db object

/**
 * @class	department
 *
 * @brief	A department.
 *
 * @author	Jordi
 * @date	6-7-2015
 */

class department
{
public:
    department(std::string name, std::string stringId) : _name(name), _stringId(stringId) {};

    const std::string& name()
    {
        return _name;
    }

    const std::string& stringId()
    {
        return _stringId;
    }

    unsigned long id()
    {
        return _id;
    }
private:
    department() {};
    friend class odb::access;
#pragma db id auto
    unsigned long _id;

    std::string _name;
    std::string _stringId;

};

#endif // DEPARTMENT_H
