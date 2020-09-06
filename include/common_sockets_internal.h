/**
 * \file    common_sockets_internal.h
 * \brief   Definition of macros used internally by the Device PnP library.
 * \authors Davit Kalantaryan
 * \date    Created on 20 Nov 2011
 *
 * \copyright Copyright 2020 Deutsches Elektronen-Synchrotron (DESY), Zeuthen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 2.1 of the license, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef COMMON_SOCKETS_INTERNAL_H
#define COMMON_SOCKETS_INTERNAL_H

#ifdef _MSC_VER
    #define COMMON_SOCKETS_HANDLER_DLL_PUBLIC		__declspec(dllexport)
    #define COMMON_SOCKETS_HANDLER_DLL_PRIVATE
    #define COMMON_SOCKETS_HANDLER_IMPORT_FROM_DLL	__declspec(dllimport)
#elif defined(__GNUC__) || defined(__clang__)
    //#define COMMON_SOCKETS_HANDLER_DLL_PUBLIC		__attribute__((visibility("default")))
    #define COMMON_SOCKETS_HANDLER_DLL_PUBLIC
    #define COMMON_SOCKETS_HANDLER_DLL_PRIVATE		__attribute__((visibility("hidden")))
    #define COMMON_SOCKETS_HANDLER_IMPORT_FROM_DLL
#elif defined(__CYGWIN__)
    #define COMMON_SOCKETS_HANDLER_DLL_PUBLIC		__attribute__((dllexport))
    #define COMMON_SOCKETS_HANDLER_DLL_PRIVATE
    #define COMMON_SOCKETS_HANDLER_IMPORT_FROM_DLL	__attribute__((dllimport))
#elif defined(__MINGW64__) || defined(__MINGW32__)
    #define COMMON_SOCKETS_HANDLER_DLL_PUBLIC		_declspec(dllexport)
    #define COMMON_SOCKETS_HANDLER_DLL_PRIVATE
    #define COMMON_SOCKETS_HANDLER_IMPORT_FROM_DLL	_declspec(dllimport)
#elif defined(__SUNPRO_CC)
    #define COMMON_SOCKETS_HANDLER_DLL_PUBLIC
    #define COMMON_SOCKETS_HANDLER_DLL_PRIVATE		__hidden
    #define COMMON_SOCKETS_HANDLER_IMPORT_FROM_DLL
#endif  // #ifdef _MSC_VER

#if defined(COMMON_SOCKETS_HANDLER_COMPILING_SHARED_LIB)
    #define COMMON_SOCKETS_HANDLER_EXPORT COMMON_SOCKETS_HANDLER_DLL_PUBLIC
#elif defined(COMMON_SOCKETS_HANDLER_USING_STATIC_LIB_OR_OBJECTS)
    #define COMMON_SOCKETS_HANDLER_EXPORT
#else
    #define COMMON_SOCKETS_HANDLER_EXPORT COMMON_SOCKETS_HANDLER_IMPORT_FROM_DLL
#endif

#ifdef __cplusplus
#define  COMMON_SOCKETS_HANDLER_BEGIN_C_DECL	extern "C" {
#define  COMMON_SOCKETS_HANDLER_END_C_DECL		}
#else
#define  DEVICE_PNP_HANDLER_BEGIN_C_DECL
#define  DEVICE_PNP_HANDLER_END_C_DECL	
#endif


#ifdef _MSC_VER
#define COMMON_SOCKETS_THISCALL		__thiscall
#else
#define COMMON_SOCKETS_THISCALL
#endif


#endif  // #ifndef COMMON_SOCKETS_INTERNAL_H
