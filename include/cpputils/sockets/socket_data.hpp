//
// repo:			cpputils-sockets
// file:            socket_data.hpp
// path:			include/cpputils/sockets/socket_data.hpp
// created on:		2023 Jul 01
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#pragma once

#include <cpputils/sockets/export_symbols.h>
#include <stddef.h>
#include <cinternal/disable_compiler_warnings.h>

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#define CpputilsPoll                        WSAPoll
#define CPPUTILS_SOCKS_CLOSE_SOCK			static_cast<SOCKET>(-1)
#define CpputilsCloseSocket     			closesocket
#define CHECK_FOR_SOCK_INVALID(_a_socket_)	((_a_socket_) == INVALID_SOCKET)
#define CHECK_FOR_SOCK_ERROR(_a_return_)	((_a_return_) == SOCKET_ERROR)
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#define CpputilsPoll                        poll
#define CPPUTILS_SOCKS_CLOSE_SOCK			(-1)
#define CpputilsCloseSocket 				close
#ifndef SOCKET_ERROR
#define SOCKET_ERROR (-1)
#endif
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

#include <cinternal/undisable_compiler_warnings.h>

namespace cpputils { namespace sockets{

#ifdef _WIN32
typedef SOCKET		socket_t;
typedef int			sndrcv_inp_cnt;
typedef int			sndrcv_ret_cnt;
typedef int			cpputils_socklen_t;
typedef ULONG		cpputils_poll_arg2;
#else
typedef int			socket_t;
typedef size_t		sndrcv_inp_cnt;
typedef ssize_t		sndrcv_ret_cnt;
typedef socklen_t   cpputils_socklen_t;
typedef nfds_t 		cpputils_poll_arg2;
#endif


struct SysSocket {
	socket_t  sock;
};


struct StopperData {
    SysSocket   stp;
    SysSocket   pol;
};


}}  //  namespace cpputils { namespace sockets{
