//
// file:			cpputils_unnamedsemaphore.cpp
// path:			src/core/cpputils_unnamedsemaphore.cpp
// created on:		2022 Jan 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <cpputils/unnamedsemaphore.hpp>

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#elif defined(__APPLE__)
#include <dispatch/dispatch.h>
#else
#include <semaphore.h>
#include <sys/time.h>
#define UNNAMED_SHARING_TYPE	0/* 0 means semaphores is shared between threads in same process */
#endif


namespace cpputils {

#ifdef _WIN32
typedef :: HANDLE TYPE_SEMA;
#elif defined(__APPLE__)
typedef :: dispatch_semaphore_t TYPE_SEMA;
#else
typedef :: sem_t TYPE_SEMA;
#endif


class CPPUTILS_DLL_PRIVATE UnnamedSemaphore_p
{
public:
    TYPE_SEMA m_sema;
};

/*///////////////////////////////////////////////////////////////////////////*/

UnnamedSemaphore::UnnamedSemaphore(size_t a_initialValue)
    :
      m_sema_data_p(new UnnamedSemaphore_p())
{
#ifdef _WIN32
        m_sema_data_p->m_sema = CreateSemaphore( nullptr, static_cast<LONG>(a_initialValue), static_cast<LONG>(1024), nullptr );
#elif defined(__APPLE__)
        m_sema_data_p->m_sema = dispatch_semaphore_create(static_cast<intptr_t>(a_initialValue));
#else
        sem_init( &(m_sema_data_p->m_sema), UNNAMED_SHARING_TYPE, static_cast<unsigned int>(a_initialValue) );
#endif
}


UnnamedSemaphore::~UnnamedSemaphore()
{
#ifdef _WIN32
        CloseHandle( m_sema_data_p->m_sema );
#elif defined(__APPLE__)
        dispatch_release(m_sema_data_p->m_sema);
#else
        sem_destroy( &(m_sema_data_p->m_sema) );
#endif
    delete m_sema_data_p;
}


void UnnamedSemaphore::Post()
{
#ifdef _WIN32
    ReleaseSemaphore( m_sema_data_p->m_sema, 1, nullptr  );
#elif defined(__APPLE__)
    dispatch_semaphore_signal(m_sema_data_p->m_sema);
#else
    sem_post( &(m_sema_data_p->m_sema) );
#endif
}


void UnnamedSemaphore::Post(size_t a_postCount)
{
#ifdef _WIN32
    ReleaseSemaphore( m_sema_data_p->m_sema, static_cast<LONG>(a_postCount), nullptr  );
#elif defined(__APPLE__)
    for(size_t i(0); i<a_postCount; ++i){
        dispatch_semaphore_signal(m_sema_data_p->m_sema);
    }
#else
    for(size_t i(0); i<a_postCount; ++i){
        sem_post( &(m_sema_data_p->m_sema) );
    }
#endif
}


void UnnamedSemaphore::Wait()
{
#ifdef _WIN32
        WaitForSingleObjectEx( m_sema_data_p->m_sema, INFINITE,TRUE );
#elif defined(__APPLE__)
        dispatch_semaphore_wait(m_sema_data_p->m_sema, DISPATCH_TIME_FOREVER);
#else
        sem_wait( &(m_sema_data_p->m_sema) );
#endif
}


void UnnamedSemaphore::Wait(size_t a_waitTimeMs)
{
#ifdef _WIN32
        WaitForSingleObjectEx( m_sema_data_p->m_sema, static_cast<DWORD>(a_waitTimeMs),TRUE );
#elif defined(__APPLE__)
        dispatch_semaphore_wait(m_sema_data_p->m_sema, static_cast<dispatch_time_t>(a_waitTimeMs));
#else
    struct timespec finalAbsTime;
    struct timeval currentTime;
    long long int nExtraNanoSeconds;
    gettimeofday(&currentTime,NULL);
    nExtraNanoSeconds = (long long int)currentTime.tv_usec*1000 + (long long int)((a_waitTimeMs%1000)*1000000);
    finalAbsTime.tv_sec = currentTime.tv_sec + a_waitTimeMs / 1000 + nExtraNanoSeconds/1000000000;
    finalAbsTime.tv_nsec = nExtraNanoSeconds%1000000000;
    sem_timedwait(&(m_sema_data_p->m_sema),&finalAbsTime);
#endif
}


}  // namespace cpputils {
