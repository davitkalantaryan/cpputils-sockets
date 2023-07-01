//
// file:			unnamedsemaphore.hpp
// path:			include/cpputils/unnamedsemaphore.hpp
// created on:		2021 Mar 31
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#ifndef CPPUTILS_INCLUDE_CPPUTILS_UNNAMEDSEMAPHORE_HPP
#define CPPUTILS_INCLUDE_CPPUTILS_UNNAMEDSEMAPHORE_HPP


#include <cpputils/export_symbols.h>
#include <stddef.h>
#include <cinternal/disable_compiler_warnings.h>


namespace cpputils {

class CPPUTILS_DLL_PRIVATE UnnamedSemaphore_p;

class CPPUTILS_EXPORT UnnamedSemaphore
{
public:
    UnnamedSemaphore(size_t initialValue=0);
    ~UnnamedSemaphore();

    void Post();
    void Post(size_t postCount);
    void Wait();
    void Wait(size_t waitTimeMs);

private:
    UnnamedSemaphore(const UnnamedSemaphore&) CPPUTILS_DELETE
    UnnamedSemaphore& operator=(const UnnamedSemaphore&) CPPUTILS_DELETE

private:
    UnnamedSemaphore_p*const m_sema_data_p;
};


}  //  namespace cpputils {


#endif  // #ifndef CPPUTILS_INCLUDE_CPPUTILS_UNNAMEDSEMAPHORE_HPP
