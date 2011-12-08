/**
*	Copyright (C) 2011  Xu Cheng, Yang Zhengyu ,Zuo Zhiheng
*
*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 3 of the License, or
*	(at your option) any later version.
*	
*	This program is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*	GNU General Public License for more details.
*	
*	You should have received a copy of the GNU General Public License
*	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
///:SQLite.h
#ifndef SQLITE_H
#define SQLITE_H

/* Make CppSQLite work without sqlite3.dll */

#if (defined(_WIN32) || defined(WIN32)) && !defined(_CRT_SECURE_NO_WARNINGS)
/* This needs to come before any includes for MSVC compiler */
#define _CRT_SECURE_NO_WARNINGS
#endif

/* Include all the header for sqlite 3 */
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

/* Include the header for CppSQLite */
#include "CppSQLite3.h"

#endif
///:~