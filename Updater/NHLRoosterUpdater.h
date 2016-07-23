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
 * @file	NHLRoosterUpdater.h
 *
 * @brief	Header which has all the defined global variables
 */

#ifndef _NHLROOSTERUPDATER_H
#define _NHLROOSTERUPDATER_H
#include "includes.h"

/** @brief	List of worker threads. */
std::vector<std::thread> workers;

/**
 * @typedef	struct
 *
 * @brief	Change item
 * 			holds classname and a list of timestamps of lessons that have changed
 */

struct change_t
{
	std::vector<Value> changeList;
	std::string className;
};

/* Parameters */
/** @brief	true to do rebuild. */
bool doRebuild = false;
/** @brief	true to dry run push messages */
bool dryRun = false;

/** @brief	Database handle. */
std::unique_ptr<database> db;
#endif