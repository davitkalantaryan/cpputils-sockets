
// common_servertcp.hpp
// 2017 Jul 06

#ifndef COMMON_IO_SOCKET_TCPSERVER_HPP
#define COMMON_IO_SOCKET_TCPSERVER_HPP

#include <common/io/socket/tcp.hpp>
#ifndef _WIN32
#include <pthread.h>
#endif

struct sockaddr_in;
typedef void(COMMON_SOCKETS_THISCALL*TypeAccept)(void* owner,common::io::socket::Tcp& clientSock,const sockaddr_in* remoteAddr);

namespace common{ namespace io{ namespace socket{

class COMMON_SOCKETS_HANDLER_EXPORT TcpServer : public Base
{
public:
	TcpServer();
	virtual ~TcpServer();

	template <typename Type>
	int	StartServer(
		Type* owner, 
		void(Type::*fpAddClient)(Tcp& clientSock,const sockaddr_in*remoteAddr),
                int a_nPort, bool bReuse = true, bool a_bLoopback = false, int a_lnTimeout = 1000,int* retCode=NULL);

	int	StartServerS(
		TypeAccept fpAddClient,void* owner, 
                int a_nPort, bool bReuse = true, bool a_bLoopback = false, int a_lnTimeout = 1000, int* retCode=NULL);
	
	void StopServer(void);

        int  InitServer(int a_nPort, bool bReuse = true, bool a_bLoopback = false);
        int  WaitForConnection(int timeoutMs, sockaddr_in* a_pRemoteAddr); // >0 is socket, 0 means timeout, -1 means othe error

private:
	//virtual void AddClient(common::SocketTCP& clientSocket, const sockaddr_in* remoteAddress)= 0;

protected:
	/*
	 * ServerAccept: server waiting for new connection
	 * Parameter:
	 *	a_ppClient:	accepted client socket
	 * Return:
	 *    < 0:	error
	 *   	0:	timeout
	 *	1:	ok
	 */
	int	ServerAccept(int& a_nClientSocket, int a_lnTimeout, sockaddr_in* remAddress);

	/*
	 * CreateServer: create server
	 * Parameter:
	 *	port: 	port number
	 * Return:
	 * 	0:	ok
	 *    < 0:	error
	 */
	int	 CreateServer(int a_nPort, bool bReuse,bool a_bLoopback);
	void RunServer(int a_lnTimeout,TypeAccept addClientFnc, void* owner);

private:
	// closed functions
	int		readC(void*, int)const { return 0; }
	int		writeC(const void*, int) { return 0; }

protected:
	volatile int	m_nWorkStatus;
	int				m_nServerThreadId;
#ifndef _WIN32
    pthread_t       m_serverThread;
#endif
};

}}}  // namespace common{ namespace io{ namespace socket{


#ifndef COMMON_IO_SOCKET_TCPSERVER_IMPL_HPP
#include "tcpsetver.impl.hpp"
#endif


#endif // #ifndef COMMON_IO_SOCKET_TCPSERVER_HPP
