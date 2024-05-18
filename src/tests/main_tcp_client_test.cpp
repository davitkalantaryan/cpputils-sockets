//
// repo:			cpputils-sockets
// file:            main_tcp_client_test.cpp
// path:			src/tests/main_tcp_client_test.cpp
// created on:		2023 Jul 01
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#include <cpputils/sockets/export_symbols.h>
#include <cpputils/sockets/tcp_socket.hpp>
#include <stdio.h>

int main(int a_argc, char* a_argv[])
{
    const char* cpcServerHost = "localhost";
	char vcBuffer[128];
	cpputils::sockets::tcp_socket aSocket;
    
    if(a_argc>1){
        cpcServerHost = a_argv[1];
    }
    
	if (aSocket.Connect(cpcServerHost, 9030, 1000)) {
		fprintf(stderr, "unable to connect\n");
		return 1;
	}

	const int nRcv = aSocket.receiveAll(vcBuffer,4);
	fprintf(stdout, "nRcv = %d\n",nRcv);

	if (nRcv > 0) {
		vcBuffer[nRcv] = 0;
		fprintf(stdout, "dataReceived = \"%s\"\n", vcBuffer);
	}
	fflush(stdout);

	aSocket.Send("pong",4);

	// aSocket.Close(); // no need for this, because destructor will do this

	return 0;;
}
