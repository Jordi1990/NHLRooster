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
 * @file	include\pushClients.h
 *
 * @brief	ODB class of a push client
 */

#ifndef PUSHCLIENTS_H
#define PUSHCLIENTS_H
#include <string>
#include <odb/tr1/memory.hxx>

using std::tr1::shared_ptr;
#include <odb/core.hxx>

#pragma db object

/**
 * @class	pushClients
 *
 * @brief	A push client.
 *
 * @author	Jordi
 * @date	6-7-2015
 */

class pushClients
{
public:
    pushClients(std::string deviceId, std::string registrationId, std::string className) : _deviceId(deviceId), _registrationId(registrationId), _className(className) {};

    const std::string& deviceId()
    {
        return _deviceId;
    }

    const std::string& registrationId()
    {
        return _registrationId;
    }

    const std::string& className()
    {
        return _className;
    }
private:
    pushClients() {};
    friend class odb::access;
#pragma db id
#pragma db type ("VARCHAR(255)")
    std::string _deviceId;

#pragma db type ("VARCHAR(255)")
    std::string _registrationId;

#pragma db type ("VARCHAR(128)") null
    std::string _className;

};

#endif // PUSHCLIENTS_H
