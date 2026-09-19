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
#include <cinternal/threading.h>
#include <cinternal/disable_compiler_warnings.h>
#include <stdio.h>
#include <string.h>
#include <cinternal/undisable_compiler_warnings.h>


static void ServerAcceptFunctionStatic(::cpputils::sockets::tcp_socket& a_sock, const sockaddr_in& a_addr);
static void InterruptFunction(CinternalInterruptArgType1);


int main(void)
{
    CinternalLoggerSetCurrentLogLevel(10);

#ifdef _WIN32
    const CinternalSimpleSignalHandlerPointer initialSigintPointer = signal(CinternalSignalForNetworkingFailedPipe, [](int) {});
#else
    struct sigaction initialSigpipeAction;
    struct sigaction newAction;
    memset(&newAction, 0, sizeof(struct sigaction));
    sigemptyset(&newAction.sa_mask);
    newAction.sa_flags = 0;
    newAction.sa_handler = [](int){};
    sigaction(CinternalSignalForNetworkingFailedPipe, &newAction, &initialSigpipeAction);
#endif

    int nIteration;
    const cinternal_thread_t curThreadHandle = cinternal_thread_get_current();
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
    nIteration = 0;
    aServerBlk.StartSyncServer([&aServerBlk,&nIteration](::cpputils::sockets::tcp_socket& a_sock, const sockaddr_in& a_addr) {
        ServerAcceptFunctionStatic(a_sock, a_addr);
        if((++nIteration)>5){
            aServerBlk.StopServer();
            aServerBlk.DestroyServer();
            return;
        }
        ::cpputils::sockets::StopperData stpData[100];
        int nRet = aServerBlk.GetStopperData(stpData, (size_t)nIteration);
        CInternalLogDebug("int nRet[%d] = aServerBlk.GetStopperData(stpData, nIteration[%d]);",nRet,nIteration);
    });

    ::cpputils::sockets::tcp_server_async aServer;
    nIteration = 0;
    const int nRet = aServer.CreateAndStartAsyncServerOnOtherThreadAndReturn(9030, [curThreadHandle,&aServer,&nIteration](::cpputils::sockets::tcp_socket& a_sock, const sockaddr_in& a_addr) {
        ServerAcceptFunctionStatic(a_sock,a_addr);
        if((++nIteration)>1){
            aServer.StopServer();
            aServer.DestroyServer();
            CinternalInterruptThread(curThreadHandle, CinternalSignalToSendUsr1, &InterruptFunction);
        }
        else{
            ::cpputils::sockets::StopperData stpData;
            aServer.GetStopperData(&stpData, 1);
        }
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
    CinternalSleepInterruptableMs(1000000);
    aServer.StopServer();
    aServer.DestroyServer();
    cinternal_thread_close_cur_thread_handle(curThreadHandle);

#ifdef _WIN32
    signal(CinternalSignalForNetworkingFailedPipe, initialSigintPointer);
#else
    sigaction(CinternalSignalForNetworkingFailedPipe, &initialSigpipeAction, nullptr);
#endif

	return 0;
}


static void InterruptFunction(CinternalInterruptArgType1)
{
    CInternalLogDebug(" ");
}


static void ServerAcceptFunctionStatic(::cpputils::sockets::tcp_socket& a_sock, const sockaddr_in& a_addr)
{
    char vcBuffer[128];
    char vcIp4addr[128];
    char vcIp6addr[128];
    ::cpputils::sockets::tcp_socket aSocket;
    const char* cpcIp4Addr = ::cpputils::sockets::GetIPV4Address(a_addr, vcIp4addr,127);
    if (cpcIp4Addr) {
        fprintf(stdout, "Client host ipv4: %s\n", cpcIp4Addr);
        fflush(stdout);
    }
    const char* cpcIp6Addr = ::cpputils::sockets::GetIpV6Address((const sockaddr_in6&)a_addr, vcIp6addr, 127);
    if (cpcIp6Addr) {
        fprintf(stdout, "Client host ipv6: %s\n", cpcIp6Addr);
        fflush(stdout);
    }
    const char* cpcHostname = ::cpputils::sockets::GetHostName((const sockaddr&)a_addr, sizeof(const sockaddr_in), vcBuffer, 127);
    if (cpcHostname) {
        fprintf(stdout, "Client host hostname: %s\n", cpcHostname);
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
