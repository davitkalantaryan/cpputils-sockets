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
#include <stdio.h>

int main(void)
{
	::cpputils::sockets::tcp_server aServer;
	const int nRet = aServer.StartServer(9030, [](::cpputils::sockets::tcp_socket& a_sock, const sockaddr_in* CPPUTILS_ARG_NN a_addr) {
		char vcBuffer[128];
		::cpputils::sockets::tcp_socket aSocket;
		const char* cpcHostName = ::cpputils::sockets::GetIPV4Address(a_addr);
		if (cpcHostName) {
			fprintf(stdout, "Client host %s!\n", cpcHostName);
			fflush(stdout);
		}
		aSocket.ReplaceWithOtherSocket(&a_sock); // after this one can keep aNewSock permanently
		aSocket.Send("ping",4);
		const int nRcv = aSocket.receive(vcBuffer, 4);
		fprintf(stdout, "nRcv = %d\n", nRcv);

		if (nRcv > 0) {
			vcBuffer[nRcv] = 0;
			fprintf(stdout, "dataReceived = \"%s\"\n", vcBuffer);
		}
		fflush(stdout);
		// aNewSock.Close(); // no need for this, because destructor will do this
	});
	if (nRet) {
		fprintf(stderr, "Unable to start server!\n");
		return 1;
	}
	SWITCH_SCHEDULING(1000000);
	aServer.StoptServer();

	return 0;;
}
