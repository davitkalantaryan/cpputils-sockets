//
// repo:			cpputils-sockets
// file:            export_symbols.h
// path:			include/cpputils/sockets/export_symbols.h
// created on:		2023 Jul 01
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef CSOCKETS_INCLUDE_CSOCKETS_EXPORT_SYMBOLS_H
#define CSOCKETS_INCLUDE_CSOCKETS_EXPORT_SYMBOLS_H

#include <cinternal/internal_header.h>


#ifndef CSOCKETS_EXPORT
#if defined(CSOCKETS_COMPILING_SHARED_LIB)
#define CSOCKETS_EXPORT CPPUTILS_DLL_PUBLIC
#elif defined(CSOCKETS_USING_STATIC_LIB_OR_OBJECTS)
#define CSOCKETS_EXPORT
#elif defined(CSOCKETS_LOAD_FROM_DLL)
#define CSOCKETS_EXPORT CPPUTILS_IMPORT_FROM_DLL
#else
#define CSOCKETS_EXPORT CPPUTILS_DLL_PRIVATE
#endif
#endif


#endif  // #ifndef CSOCKETS_INCLUDE_CSOCKETS_EXPORT_SYMBOLS_H
