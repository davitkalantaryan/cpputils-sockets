//
// repo:			cpputils-sockets
// file:            cpputils_sockets_core_tcp_socket_p.hpp
// path:			src/core/cpputils_sockets_core_tcp_socket_p.hpp
// created on:		2023 Jul 01
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#pragma once

#include <cpputils/sockets/tcp_socket.hpp>
#include <cpputils/sockets/socket_data.hpp>
#include <cinternal/bistateflags.h>


namespace cpputils { namespace sockets{


class CPPUTILS_DLL_PRIVATE tcp_socket_p
{
public:
	socket_t	sock;
    int         timeoutMs;
    int         reserved01;
    CPPUTILS_BISTATE_FLAGS_UN(
        isBlocking
    )flags;
};


#ifdef GetSocketAddressInline_needed
static inline int GetSocketAddressInline(socket_t a_sock, sockaddr_in* CPPUTILS_ARG_NN a_sockAddr_p) noexcept{
    cpputils_socklen_t sock_name_addr_len = static_cast<cpputils_socklen_t>(sizeof(struct sockaddr_in));
    const int rtn = ::getsockname(a_sock, (struct sockaddr*)a_sockAddr_p, &sock_name_addr_len);
    if (CHECK_FOR_SOCK_ERROR(rtn)) {
        return rtn;
    }
    return 0;
}
#endif


#ifdef GetPortNumberFromSocketAddressInline_needed
static inline int GetPortNumberFromSocketAddressInline(const sockaddr_in& a_sockAddr) noexcept{
    return ntohs(a_sockAddr.sin_port);
}
#endif


#ifdef MakeSocketNonBlockingInline_needed
static inline void MakeSocketNonBlockingInline(socket_t a_sock) noexcept{
#ifdef	_WIN32
	unsigned long on = 1;
	ioctlsocket(a_sock, FIONBIO, &on);
#else
	int status;
	if ((status = fcntl(a_sock, F_GETFL, 0)) != -1) {
		status |= O_NONBLOCK;
		fcntl(a_sock, F_SETFL, status);
	}
#endif
}
#endif


enum class DeskType {
	read,
	write,
	err,
	read_and_write,
	read_and_err,
	write_and_err,
	all
};


#ifdef WaitForDataOnSocketInline_needed
// 1,2,3 => data, 0 => timeout, -1 => error, socket should be closed
static inline int WaitForDataOnSocketInline(socket_t a_sock, int a_timeoutMs, const DeskType& a_desc) noexcept {
	fd_set* pRdFds = nullptr, * pWrFds = nullptr, * pErFds = nullptr;
	fd_set rdfds, wrfds, errfds;
	struct timeval  aTimeout;
	struct timeval* pTimeout;

	const int maxsd = static_cast<int>(a_sock) + 1;

	if (a_timeoutMs >= 0) {
		aTimeout.tv_sec = a_timeoutMs / 1000L;
		aTimeout.tv_usec = (a_timeoutMs % 1000L) * 1000L;
		pTimeout = &aTimeout;
	}
	else { pTimeout = nullptr; }

	switch (a_desc) {
	case DeskType::read:
		FD_ZERO(&rdfds);
		FD_SET(a_sock, &rdfds);
		pRdFds = &rdfds;
		break;
	case DeskType::write:
		FD_ZERO(&wrfds);
		FD_SET(a_sock, &wrfds);
		pWrFds = &wrfds;
		break;
	case DeskType::err:
		FD_ZERO(&errfds);
		FD_SET(a_sock, &errfds);
		pErFds = &errfds;
		break;
	case DeskType::read_and_write:
		FD_ZERO(&rdfds);
		FD_SET(a_sock, &rdfds);
		pRdFds = &rdfds;
		FD_ZERO(&wrfds);
		FD_SET(a_sock, &wrfds);
		pWrFds = &wrfds;
		break;
	case DeskType::read_and_err:
		FD_ZERO(&rdfds);
		FD_SET(a_sock, &rdfds);
		pRdFds = &rdfds;
		FD_ZERO(&errfds);
		FD_SET(a_sock, &errfds);
		pErFds = &errfds;
		break;
	case DeskType::write_and_err:
		FD_ZERO(&wrfds);
		FD_SET(a_sock, &wrfds);
		pWrFds = &wrfds;
		FD_ZERO(&errfds);
		FD_SET(a_sock, &errfds);
		pErFds = &errfds;
		break;
	default:
		FD_ZERO(&rdfds);
		FD_SET(a_sock, &rdfds);
		pRdFds = &rdfds;
		FD_ZERO(&wrfds);
		FD_SET(a_sock, &wrfds);
		pWrFds = &wrfds;
		FD_ZERO(&errfds);
		FD_SET(a_sock, &errfds);
		pErFds = &errfds;
		break;
	}  //  switch (a_desc) {

	const int rtn = ::select(maxsd, pRdFds, pWrFds, pErFds, pTimeout);

	switch (rtn) {
	case 0:	/* time out */
		return 0;
	case SOCKET_ERROR:
		if (errno == EINTR) {/*interrupted by signal*/return -1; }  // interrupt
		return -1;  // select error
	default:
		break;
	}  //  switch (rtn){

	int nCount = 0;
	if (pRdFds && FD_ISSET(a_sock, pRdFds)) { ++nCount; }
	if (pWrFds && FD_ISSET(a_sock, pWrFds)) { ++nCount; }
	if (pErFds && FD_ISSET(a_sock, pErFds)) { ++nCount; }

	// if nCount == 0 , we have fatal error, else we have data
	return nCount ? nCount : (-1);
}
#endif  //  #ifdef WaitForDataOnSocketInline_needed


}}  //  namespace cpputils { namespace sockets{
