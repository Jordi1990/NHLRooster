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
* @file	include\ThreadSafeQueue.h
*
* @brief	Template for a object which can only be approached using locks
*/

#ifndef _THREADSAFEQUEUE_H
#define _THREADSAFEQUEUE_H

#include "includes.h"

template <class T>
	class ThreadSafeQueue {
	private:
		std::queue<T> list;
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
		
		bool empty()
		{
			return list.empty();
		}
		
		void push_fast(T object) {
			list.push(object);
		}

		void push(T object) {
			if (!locked)
			{
				std::cout << "# ERR: push_back not locked " << __FILE__;
				std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
				throw std::exception();
			}
			list.push(object);
		}
	
		size_t size() {
			return list.size();
		}

		T pop() {
			if (!locked)
			{
				std::cout << "# ERR: pop_back not locked " << __FILE__;
				std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
				throw std::exception();
			}
			T res = list.front();
			list.pop();
			return res;
		}
	};

#endif