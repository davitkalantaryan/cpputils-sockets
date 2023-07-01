//
// repo:			cpputils-sockets
// file:            cpputils_sockets_core_tcp_socket_p.hpp
// path:			src/core/cpputils_sockets_core_tcp_socket_p.hpp
// created on:		2023 Jul 01
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#pragma once

#include <cpputils/sockets/tcp_socket.hpp>
#include <cinternal/disable_compiler_warnings.h>
#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#define CPPUTILS_SOCKS_CLOSE_SOCK			static_cast<SOCKET>(-1)
#define closesocketn						closesocket
#define SWITCH_SCHEDULING(_t_)				SleepEx((_t_),TRUE)
#define CHECK_FOR_SOCK_INVALID(_a_socket_)	((_a_socket_) == INVALID_SOCKET)
#define CHECK_FOR_SOCK_ERROR(_a_return_)	((_a_return_) == SOCKET_ERROR)
#else
#define CPPUTILS_SOCKS_CLOSE_SOCK			(-1)
#define closesocketn						close
#define SWITCH_SCHEDULING(_t_)				usleep(900*(_t_))
#define CHECK_FOR_SOCK_INVALID(_a_socket_)	((_a_socket_) < 0)
#define CHECK_FOR_SOCK_ERROR(_a_return_)	((_a_return_) < 0)
// SOCKET_INPROGRESS
#if defined(EALREADY) && defined(EAGAIN)
#define	SOCKET_INPROGRESS(e)	(e == EINPROGRESS || e == EALREADY || e == EAGAIN)
#elif defined( EALREADY)
#define	SOCKET_INPROGRESS(e)	(e == EINPROGRESS || e == EALREADY)
#elif defined(EAGAIN)
#define	SOCKET_INPROGRESS(e)	(e == EINPROGRESS || e == EAGAIN)
#else
#define	SOCKET_INPROGRESS(e)	(e == EINPROGRESS)
#endif
#endif

namespace cpputils { namespace sockets{

#ifdef _WIN32
typedef SOCKET		socket_t;
typedef int			sndrcv_inp_cnt;
typedef int			sndrcv_ret_cnt;
#else
typedef int			socket_t;
typedef size_t		sndrcv_inp_cnt;
typedef ssize_t		sndrcv_ret_cnt;
#endif

class CPPUTILS_DLL_PRIVATE tcp_socket_p
{
public:
	socket_t	sock;
};


static inline void MakeSocketNonBlockingInline(socket_t a_sock){
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


}}  //  namespace cpputils { namespace sockets{
