//
// repo:			cpputils-sockets
// file:            cpputils_sockets_core_tcp_socket.cpp
// path:			src/core/cpputils_sockets_core_tcp_socket.cpp
// created on:		2023 Jul 01
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#include "cpputils_sockets_core_tcp_socket_p.hpp"
#include <string.h>
#include <stdlib.h>

namespace cpputils { namespace sockets{


tcp_socket::~tcp_socket()
{
	Close();
}


tcp_socket::tcp_socket()
	:
	m_sock_data_p(new tcp_socket_p())
{
	m_sock_data_p->sock = CPPUTILS_SOCKS_CLOSE_SOCK;
}


tcp_socket::tcp_socket(const SysSocket* a_createdSocket)
	:
	m_sock_data_p(new tcp_socket_p())
{
	m_sock_data_p->sock = a_createdSocket->sock;
}


tcp_socket::tcp_socket(tcp_socket&& a_mM) noexcept
	:
	m_sock_data_p(a_mM.m_sock_data_p)
{
	a_mM.m_sock_data_p = new tcp_socket_p();
	a_mM.m_sock_data_p->sock = CPPUTILS_SOCKS_CLOSE_SOCK;
}


tcp_socket& tcp_socket::operator=(tcp_socket&& a_mM) noexcept
{
	tcp_socket_p* pThisData = m_sock_data_p;
	m_sock_data_p = a_mM.m_sock_data_p;
	a_mM.m_sock_data_p = pThisData;
	return *this;
}


void tcp_socket::ReplaceWithOtherSocket(tcp_socket* CPPUTILS_ARG_NN a_pMM) noexcept
{
	tcp_socket_p* pThisData = m_sock_data_p;
	m_sock_data_p = a_pMM->m_sock_data_p;
	a_pMM->m_sock_data_p = pThisData;
}


static bool SOCKET_INPROGRESS_INLINE(void) {
#ifdef _WIN32
	return (WSAGetLastError() == WSAEWOULDBLOCK);
#else
	const int cnErrno = errno;
	return SOCKET_INPROGRESS(cnErrno);
#endif
}


int tcp_socket::Connect(const char* CPPUTILS_ARG_NN a_svrName, int a_port, int a_connectionTimeoutMs)
{
	if (m_sock_data_p->sock != CPPUTILS_SOCKS_CLOSE_SOCK) {
		closesocketn(m_sock_data_p->sock);
	}

	m_sock_data_p->sock = ::socket(AF_INET, SOCK_STREAM, 0);
	if (CHECK_FOR_SOCK_INVALID(m_sock_data_p->sock)) {
		m_sock_data_p->sock = CPPUTILS_SOCKS_CLOSE_SOCK;
		return -1;
	}

	unsigned long ha;
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons((u_short)a_port);

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable:4996)
#endif
	if ((ha = inet_addr(a_svrName)) == INADDR_NONE) {
		struct hostent* hostent_ptr = gethostbyname(a_svrName);  // making DNS querry
		if (!hostent_ptr) { return -1;}
		a_svrName = inet_ntoa(*(struct in_addr*)hostent_ptr->h_addr_list[0]);
		if ((ha = inet_addr(a_svrName)) == INADDR_NONE) { return -1; }
	}
#ifdef _MSC_VER
#pragma warning (pop)
#endif

	memcpy(&addr.sin_addr, &ha, sizeof(ha));

	// let's make socket non blocking
	MakeSocketNonBlocking();

	const socklen_t addr_len = sizeof(addr);
	int rtn = ::connect(m_sock_data_p->sock, (struct sockaddr*)&addr, addr_len);

	if (rtn) {
		if (!SOCKET_INPROGRESS_INLINE()) { return -1; }  // connect error
		rtn = WaitForDataOnSocketInline(m_sock_data_p->sock, a_connectionTimeoutMs, DeskType::write);
		if (rtn > 0) {
			return 0;
		}
	}  //  if (rtn) {

	return -1;  // most probably timeout

}


void tcp_socket::Close()
{
	if (m_sock_data_p->sock != CPPUTILS_SOCKS_CLOSE_SOCK) {
		closesocketn(m_sock_data_p->sock);
		m_sock_data_p->sock = CPPUTILS_SOCKS_CLOSE_SOCK;
	}
}


void tcp_socket::MakeSocketBlocking()
{
#ifdef	_WIN32
	unsigned long on = 0;
	ioctlsocket(m_sock_data_p->sock, FIONBIO, &on);
#else 
	int status;
	if ((status = fcntl(m_sock_data_p->sock, F_GETFL, 0)) != -1) {
		status &= ~O_NONBLOCK;
		fcntl(m_sock_data_p->sock, F_SETFL, status);
	}
#endif 
}


void tcp_socket::MakeSocketNonBlocking()
{
	MakeSocketNonBlockingInline(m_sock_data_p->sock);
}


int tcp_socket::receive(void* a_pBuffer, size_t a_nSize)const
{
	return (int)recv(m_sock_data_p->sock,(char*)a_pBuffer,(sndrcv_inp_cnt)a_nSize, MSG_WAITALL);
}


int tcp_socket::Send(const void* a_cpBuffer, size_t a_nSize)
{
#define MAX_NUMBER_OF_ITERS	100000
	const char* pcBuffer = (const char*)a_cpBuffer;
	const char* cp = NULL;
	int len_to_write = 0;
	int len_wrote = 0;
	int n = 0;

	len_to_write = (int)a_nSize;
	cp = pcBuffer;
	for (int i(0); len_to_write > 0; ++i){
		n = ::send(m_sock_data_p->sock, cp, (sndrcv_inp_cnt)len_to_write, 0);
		if (CHECK_FOR_SOCK_ERROR(n)) {
			if (SOCKET_INPROGRESS_INLINE()) {
				if (i < MAX_NUMBER_OF_ITERS) { SWITCH_SCHEDULING(1); continue; }
				else { return -1; }  // timeout
			}
			else { return -1; }  // send error
		}
		else {
			cp += n;
			len_to_write -= n;
			len_wrote += n;
		}
	}

	return len_wrote;
}


int tcp_socket::SendSimple(const void* a_cpBuffer, size_t a_nSize)
{
	return (int)::send(m_sock_data_p->sock, (const char*)a_cpBuffer, (sndrcv_inp_cnt)a_nSize, 0);
}


int tcp_socket::SetTimeout(int a_nTimeoutMs)
{
	char* pInput;
	int nInputLen;
#ifdef _WIN32
	DWORD dwTimeout;
	if (a_nTimeoutMs >= 0) { dwTimeout = (DWORD)a_nTimeoutMs; }
	else { dwTimeout = 2147483647; }
	pInput = (char*)&dwTimeout;
	nInputLen = sizeof(DWORD);
#else
	struct timeval tv;
	if (a_nTimeoutMs >= 0) {
		tv.tv_sec = a_nTimeoutMs / 1000;
		tv.tv_usec = (a_nTimeoutMs % 1000) * 1000;
		pInput = (char*)&tv;
		nInputLen = sizeof(struct timeval);
	}
	else {
		tv.tv_sec = 21474836;
		tv.tv_usec = 1000;
		pInput = (char*)&tv;
		nInputLen = sizeof(struct timeval);
	}
#endif

	if (setsockopt(m_sock_data_p->sock, SOL_SOCKET, SO_RCVTIMEO, pInput, nInputLen) < 0) { return -1; }

	return 0;
}


int tcp_socket::waitForReadData(int a_timeoutMs)const
{
	return WaitForDataOnSocketInline(m_sock_data_p->sock, a_timeoutMs, DeskType::read);
}


/*--------------------------------------------------------------------------------------------------------------*/

CSOCKETS_EXPORT const char* GetIPV4Address(const sockaddr_in* CPPUTILS_ARG_NN a_addr)
{
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable:4996)
#endif
	return ::inet_ntoa(a_addr->sin_addr);
#ifdef _MSC_VER
#pragma warning (pop)
#endif
}


}}  //  namespace cpputils { namespace sockets{


CPPUTILS_BEGIN_C


static void cpputils_sockets_core_tcp_socket_clean(void) {
#ifdef _WIN32
	WSACleanup();
#endif
}


CPPUTILS_C_CODE_INITIALIZER(cpputils_sockets_core_tcp_socket_initialize) {

#ifdef _WIN32
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD(2, 2);

	if (WSAStartup(wVersionRequested, &wsaData) != 0) {
		return; // todo: think on error handling
	}

	/* Confirm that the WinSock DLL supports 2.2.		*/
	/* Note that if the DLL supports versions greater	*/
	/* than 2.2 in addition to 2.2, it will still return*/
	/* 2.2 in wVersion since that is the version we		*/
	/* requested.										*/

	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		return; // todo: think on error handling
	}

#endif    // #ifdef _WIN32

	atexit(&cpputils_sockets_core_tcp_socket_clean);

}


CPPUTILS_END_C
