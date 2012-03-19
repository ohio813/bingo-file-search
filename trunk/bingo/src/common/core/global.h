/**
*   Copyright (C) 2011-2012  Xu Cheng, Yang Zhengyu, Zuo Zhiheng, Yao Wenjie
*
*   This library is free software; you can redistribute it and/or
*   modify it under the terms of the GNU Lesser General Public
*   License as published by the Free Software Foundation; either
*   version 3 of the License, or (at your option) any later version.
*
*   This library is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   Lesser General Public License for more details.
*
*   You should have received a copy of the GNU Lesser General Public
*   License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/
///:global.h
#ifndef GLOBAL_H
#define GLOBAL_H

#ifdef BINGO_NAMESPACE

# define BINGO_PREPEND_NAMESPACE(name) ::BINGO_NAMESPACE::name
# define BINGO_USE_NAMESPACE using namespace ::BINGO_NAMESPACE;
# define BINGO_BEGIN_NAMESPACE namespace BINGO_NAMESPACE {
# define BINGO_END_NAMESPACE }
# define BINGO_FORWARD_DECLARE_CLASS(name) \
    BINGO_BEGIN_NAMESPACE class name; BINGO_END_NAMESPACE \
    using BINGO_PREPEND_NAMESPACE(name);

# define BINGO_FORWARD_DECLARE_STRUCT(name) \
    BINGO_BEGIN_NAMESPACE struct name; BINGO_END_NAMESPACE \
    using BINGO_PREPEND_NAMESPACE(name);

namespace BINGO_NAMESPACE {}

#else /* BINGO_NAMESPACE */

# define BINGO_PREPEND_NAMESPACE(name) ::name
# define BINGO_USE_NAMESPACE
# define BINGO_BEGIN_NAMESPACE
# define BINGO_END_NAMESPACE
# define BINGO_FORWARD_DECLARE_CLASS(name) class name;
# define BINGO_FORWARD_DECLARE_STRUCT(name) struct name;

#endif /* BINGO_NAMESPACE */

#if defined BINGO_SHARED || defined BINGO_DLL

# ifdef __cplusplus
#  define BINGO_BEGIN_HEADER extern "C++" {
#  define BINGO_END_HEADER }
# else /* __cplusplus */
#  define BINGO_BEGIN_HEADER
#  define BINGO_END_HEADER
# endif /* __cplusplus */

# ifdef BINGO_SHARED
#  define B_LIB_EXPORT __declspec(dllexport)
#  undef BINGO_DLL
# endif /* BINGO_SHARED */

# ifdef BINGO_DLL
#  define B_LIB_EXPORT __declspec(dllimport)
#  undef BINGO_SHARED
# endif /* BINGO_DLL */

#else /* BINGO_SHARED OR BINGO_DLL */

# define BINGO_BEGIN_HEADER
# define BINGO_END_HEADER
# define B_LIB_EXPORT 

#endif /* BINGO_SHARED OR BINGO_DLL */

#endif
///:~