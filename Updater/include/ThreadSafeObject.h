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
* @file	include\ThreadSafeObject.h
*
* @brief	Template for a object which can only be approached using locks
*/

#ifndef _THREADSAFEOBJECT_H
#define _THREADSAFEOBJECT_H

#include "includes.h"

template <class T>
class ThreadSafeObject {
private:
	std::vector<T> list;
	std::mutex mutex;

protected:
	bool locked = false;
public:
	void lock() {
		try
		{
			mutex.lock();
			locked = true;
		}
		catch (std::exception &e)
		{
			printf("Locking failed: %s\n", e.what());
			exit(0);
		}
	}
	void unlock() {
		locked = false;
		mutex.unlock();
	}

	// Implementation with a little overhead to make sure we never forget to lock lists that have to be thread safe
	T& at(int position) {
		if (!locked)
		{
			std::cout << "# ERR: at not locked " << __FILE__;
			std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
			throw std::exception();
		}
		return list.at(position);
	}

	void push_back(T object) {
		if (!locked)
		{
			std::cout << "# ERR: push_back not locked " << __FILE__;
			std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
			throw std::exception();
		}
		list.push_back(object);
	}

	// This should only be used when you know what you are doing
	// Could be used to iterate faster through the list
	T& at_fast(int position) {
		return list.at(position);
	}


	// This should only be used when you know what you are doing
	// Could be used to fill a list faster
	void push_back_fast(T object) {
		list.push_back(object);
	}
	
	size_t size() {
		return list.size();
	}

	void pop_back() {
		if (!locked)
		{
			std::cout << "# ERR: pop_back not locked " << __FILE__;
			std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
			throw std::exception();
		}
		list.pop_back();
	}

	T& back() {
		if (!locked)
		{
			std::cout << "# ERR: back not locked " << __FILE__;
			std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
			throw std::exception();
		}
		return list.back();
	}
};

#endif