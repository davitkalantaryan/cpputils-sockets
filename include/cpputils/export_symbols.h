//
// file:			export_symbols.h
// path:			include/cpputils/export_symbols.h
// created on:		2022 Sep 21
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#ifndef CPPUTILS_INCLUDE_CPPUTILS_EXPORT_SYMBOLS_H
#define CPPUTILS_INCLUDE_CPPUTILS_EXPORT_SYMBOLS_H

#include <cinternal/internal_header.h>


#ifndef CPPUTILS_EXPORT
#if defined(CPPUTILS_COMPILING_SHARED_LIB)
#define CPPUTILS_EXPORT CPPUTILS_DLL_PUBLIC
#elif defined(CPPUTILS_USING_STATIC_LIB_OR_OBJECTS)
#define CPPUTILS_EXPORT
#elif defined(CPPUTILS_LOAD_FROM_DLL)
#define CPPUTILS_EXPORT CPPUTILS_IMPORT_FROM_DLL
#else
#define CPPUTILS_EXPORT CPPUTILS_DLL_PRIVATE
#endif
#endif


#endif  // #ifndef CPPUTILS_INCLUDE_CPPUTILS_EXPORT_SYMBOLS_H
