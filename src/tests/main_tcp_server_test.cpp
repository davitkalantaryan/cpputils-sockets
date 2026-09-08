//
// repo:			cpputils-sockets
// file:            main_tcp_server_test.cpp
// path:			src/tests/main_tcp_server_test.cpp
// created on:		2023 Jul 01
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#include <cpputils/sockets/export_symbols.h>
#include <cpputils/sockets/socket_data.hpp>
#include <cpputils/sockets/tcp_server.hpp>
#include <cinternal/signals.h>
#include <cinternal/disable_compiler_warnings.h>
#include <stdio.h>
#include <cinternal/undisable_compiler_warnings.h>


static void ServerAcceptFunctionStatic(::cpputils::sockets::tcp_socket& a_sock, const sockaddr_in* CPPUTILS_ARG_NN a_addr);


int main(void)
{
    ::cpputils::sockets::StopperData stpData;

    ::cpputils::sockets::blocking_tcp_server aServerBlk;
    //const int cnPort = aServerBlk.CreateBlockingServer();  // port = 0 will be used, so system will allocate one
    const int cnPort = aServerBlk.CreateBlockingServer(9030);  // port = 0 will be used, so system will allocate one
    if (cnPort < 0) {
        fprintf(stderr, "negative port = %d\n", cnPort);
        return 1;
    }
    aServerBlk.GetStopperData(&stpData, 1);
    printf("Sync Server port is %d.Going to infinite loop. If client connected, then server will be stopped\n", cnPort);
    aServerBlk.RunBlockingServer([&aServerBlk](::cpputils::sockets::tcp_socket& a_sock, const sockaddr_in* CPPUTILS_ARG_NN a_addr) {
        ServerAcceptFunctionStatic(a_sock, a_addr);
        aServerBlk.StoptServer();
    });

	::cpputils::sockets::tcp_server aServer;
	const int nRet = aServer.StartAsyncServerOnOtherThreadAndReturn(9030, [&aServer](::cpputils::sockets::tcp_socket& a_sock, const sockaddr_in* CPPUTILS_ARG_NN a_addr) {
        ServerAcceptFunctionStatic(a_sock,a_addr);
        aServer.StoptServer();
	});
	if (nRet) {
		fprintf(stderr, "Unable to start server!\n");
		return 1;
	}

    printf("Async Server port is %d. Waiting 100s. If client connected, then server will be stopped\n",aServer.getPortNumber());

    // we wait for 100 seconds, so one can connect to server by using telnet or netcat
    // also we stop the server from callback
    aServerBlk.GetStopperData(&stpData, 1);
    CinternalSleepInterruptableMs(15000);
	aServer.StoptServer(); // if callback stopped, then this call will not do anything

	return 0;
}


static void ServerAcceptFunctionStatic(::cpputils::sockets::tcp_socket& a_sock, const sockaddr_in* CPPUTILS_ARG_NN a_addr)
{
    char vcBuffer[128];
    ::cpputils::sockets::tcp_socket aSocket;
    const char* cpcHostName = ::cpputils::sockets::GetIPV4Address(a_addr);
    if (cpcHostName) {
        fprintf(stdout, "Client host %s!\n", cpcHostName);
        fflush(stdout);
    }
    aSocket.ReplaceWithOtherSocket(&a_sock); // after this one can keep aNewSock permanently
    aSocket.Send("ping", 4);
    const int nRcv = aSocket.receiveAll(vcBuffer, 4);
    fprintf(stdout, "nRcv = %d\n", nRcv);

    if (nRcv > 0) {
        vcBuffer[nRcv] = 0;
        fprintf(stdout, "dataReceived = \"%s\"\n", vcBuffer);
    }
    fflush(stdout);
    // aNewSock.Close(); // no need for this, because destructor will do this
}
