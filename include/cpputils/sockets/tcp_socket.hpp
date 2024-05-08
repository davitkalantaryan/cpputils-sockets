//
// repo:			cpputils-sockets
// file:            tcp_socket.hpp
// path:			include/cpputils/sockets/tcp_socket.hpp
// created on:		2023 Jul 01
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#pragma once

#include <cpputils/sockets/export_symbols.h>
#include <stddef.h>

struct sockaddr_in;

namespace cpputils { namespace sockets{

struct SysSocket;
class CPPUTILS_DLL_PRIVATE tcp_socket_p;

class CSOCKETS_EXPORT tcp_socket
{
public:
	virtual ~tcp_socket();
	tcp_socket();
	tcp_socket(const SysSocket* a_createdSocket);
	tcp_socket(tcp_socket&& a_mM) noexcept;
	tcp_socket(const tcp_socket&)=delete;
	
	tcp_socket& operator=(tcp_socket&& a_mM) noexcept;
	tcp_socket& operator=(const tcp_socket&) = delete;
	void ReplaceWithOtherSocket(tcp_socket* CPPUTILS_ARG_NN a_pMM) noexcept;
	int  Connect(const char* CPPUTILS_ARG_NN a_svrName, int a_port, int a_connectionTimeoutMs);
	void Close();
	void MakeSocketBlocking();
	void MakeSocketNonBlocking();
	int  receiveAll(void* a_pBuffer, size_t a_nSize)const;
    int  receiveSngl(void* a_pBuffer, size_t a_nSize)const;
	int  Send(const void* a_cpBuffer, size_t a_nSize);
	int  SendSimple(const void* a_cpBuffer, size_t a_nSize);
	int  SetTimeout(int a_nTimeoutMs);
	int  waitForReadData(int a_timeoutMs)const;  // 1,2,3 => data, 0 => timeout, -1 => error, socket should be closed
    void GetSysSocketAndReset(SysSocket* CPPUTILS_ARG_NN a_pSysSocket);
    void getSysSocket(SysSocket* CPPUTILS_ARG_NN a_pSysSocket)const;
    void Reset();

protected:
	tcp_socket_p* m_sock_data_p;
};


CSOCKETS_EXPORT const char* GetIPV4Address(const sockaddr_in* CPPUTILS_ARG_NN a_addr);


}}  //  namespace cpputils { namespace sockets{
