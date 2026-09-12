//
// repo:			cpputils-sockets
// file:            tcp_socket.hpp
// path:			include/cpputils/sockets/tcp_socket.hpp
// created on:		2023 Jul 01
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#pragma once

#include <cpputils/sockets/export_symbols.h>
#include <cinternal/disable_compiler_warnings.h>
#include <stddef.h>
#include <cinternal/undisable_compiler_warnings.h>

struct sockaddr_in;

namespace cpputils { namespace sockets{

struct SysSocket;
class CPPUTILS_DLL_PRIVATE tcp_socket_p;

class CSOCKETS_EXPORT tcp_socket
{
public:
    ~tcp_socket() noexcept;
	tcp_socket();
    tcp_socket(ptrdiff_t a_createdRawSock);
    tcp_socket(const SysSocket* CPPUTILS_ARG_NN a_createdSysSock);
    tcp_socket(tcp_socket&& a_mM);
	tcp_socket& operator=(tcp_socket&& a_mM) noexcept;

    int  Connect(const char* CPPUTILS_ARG_NN a_svrName, int a_port, int a_connectionTimeoutMs) noexcept;
    void Close() noexcept;
    int  receiveAll(void* a_pBuffer, size_t a_nSize)const noexcept;
    int  receiveSngl(void* a_pBuffer, size_t a_nSize)const noexcept;
    int  send(const void* a_cpBuffer, size_t a_nSize)const noexcept;
    int  sendSimple(const void* a_cpBuffer, size_t a_nSize)const noexcept;
    void MakeSocketBlocking() noexcept;
    void MakeSocketNonBlocking() noexcept;
    int  SetTimeout(int a_nTimeoutMs) noexcept;
    int  waitForReadData(int a_timeoutMs)const noexcept;  // 1,2,3 => data, 0 => timeout, -1 => error, socket should be closed
    void ReplaceWithOtherSocket(tcp_socket* CPPUTILS_ARG_NN a_pMM) noexcept;
    void GetSysSocketAndRelease(SysSocket* CPPUTILS_ARG_NN a_pSysSocket) noexcept;
    void getSysSocket(SysSocket* CPPUTILS_ARG_NN a_pSysSocket)const noexcept;
    ptrdiff_t GetRawSocketAndRelease() noexcept;
    ptrdiff_t getRawSock()const noexcept;
    void Release()noexcept;
    void ReleaseFromSysSock(const SysSocket* a_createdSysSock=nullptr) noexcept;
    void ReleaseFromRawSock(ptrdiff_t a_createdRawSock=-1) noexcept;
    void ResetFromSysSock(const SysSocket* a_createdSysSock=nullptr) noexcept;
    void ResetFromRawSock(ptrdiff_t a_createdRawSock=-1) noexcept;
    int  SetKeepAliveTimeouts(int a_idleTimeSec, int a_intervalSec, int a_maxProbes) noexcept;
    bool isValid()const noexcept;
    bool isBlocking()const noexcept;
    int timeoutMs()const noexcept;

private:
	tcp_socket_p* m_sock_data_p;

private:
    tcp_socket(const tcp_socket&)=delete;
    tcp_socket& operator=(const tcp_socket&) = delete;
};


CSOCKETS_EXPORT const char* GetIPV4Address(const sockaddr_in* CPPUTILS_ARG_NN a_addr) noexcept;


}}  //  namespace cpputils { namespace sockets{
