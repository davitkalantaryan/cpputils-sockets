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
#include <cinternal/logger.h>
#include <cinternal/disable_compiler_warnings.h>
#include <stdio.h>
#include <cinternal/undisable_compiler_warnings.h>


static void ServerAcceptFunctionStatic(::cpputils::sockets::tcp_socket& a_sock, const sockaddr_in* CPPUTILS_ARG_NN a_addr);


int main(void)
{
    CinternalLoggerSetCurrentLogLevel(10);

    ::cpputils::sockets::StopperData stpData;

    ::cpputils::sockets::tcp_server_sync aServerBlk;
    //const int cnPort = aServerBlk.CreateServer(0,false);  // port = 0 will be used, so system will allocate one
    const int cnPort = aServerBlk.CreateServer(9030,false);  // port = 0 will be used, so system will allocate one
    if (cnPort < 0) {
        fprintf(stderr, "negative port = %d\n", cnPort);
        return 1;
    }
    aServerBlk.GetStopperData(&stpData, 1);
    fprintf(stdout,"Sync Server port is %d. Going to infinite loop. If client connected, then server will be stopped\n", cnPort);
    fflush(stdout);
    aServerBlk.StartSyncServer([&aServerBlk](::cpputils::sockets::tcp_socket& a_sock, const sockaddr_in* CPPUTILS_ARG_NN a_addr) {
        ServerAcceptFunctionStatic(a_sock, a_addr);
        aServerBlk.StopServer();
        aServerBlk.DestroyServer();
    });

    ::cpputils::sockets::tcp_server_async aServer;
    const int nRet = aServer.CreateAndStartAsyncServerOnOtherThreadAndReturn(9030, [&aServer](::cpputils::sockets::tcp_socket& a_sock, const sockaddr_in* CPPUTILS_ARG_NN a_addr) {
        ServerAcceptFunctionStatic(a_sock,a_addr);
        aServer.StopServer();
        aServer.DestroyServer();
	});
	if (nRet) {
		fprintf(stderr, "Unable to start server!\n");
		return 1;
	}

    fprintf(stdout,"Async Server port is %d. Waiting 100s. If client connected, then server will be stopped\n",aServer.getPortNumber());
    fflush(stdout);
    // we wait for 100 seconds, so one can connect to server by using telnet or netcat
    // also we stop the server from callback
    aServer.GetStopperData(&stpData, 1);
    CinternalSleepInterruptableMs(15000);
    aServer.StopServer();
    aServer.DestroyServer();

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
    aSocket.send("ping", 4);
    const int nRcv = aSocket.receiveAll(vcBuffer, 4);
    fprintf(stdout, "nRcv = %d\n", nRcv);

    if (nRcv > 0) {
        vcBuffer[nRcv] = 0;
        fprintf(stdout, "dataReceived = \"%s\"\n", vcBuffer);
    }
    fflush(stdout);
    // aNewSock.Close(); // no need for this, because destructor will do this
}
