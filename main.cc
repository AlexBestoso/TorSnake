#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <string>

using namespace std;

#include "./torSnake.class.h"

int main(){
	string host = "6nhmgdpnyoljh5uzr5kwlatx2u3diou4ldeommfxjz3wkhalzgjqxzqd.onion";
	int port = 80;

	string request = "GET / HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n";
	char response[1000000];
	size_t responseSize = 1000000;

	TorSnake ts;

	ts.setSoLoc("/usr/lib/x86_64-linux-gnu/torsocks/libtorsocks.so");
	if(!ts.createClient(host, port)){
		printf("Failed to create client\n");
		return 1;
	}else{
		printf("Client successfully created.\n");
	}

	if(!ts.connectClient()){
		printf("Failed to connect to server.\n");
		return false;
	}else{
		printf("Successfully connected.\n");
	}

	if(!ts.sendClient(request.c_str(), request.length())){
		printf("Failed to sent HTTP Request.\n");
		ts.closeSocket();
		return 1;
	}else{
		printf("Send successful!\n");
	}
	
	if(!ts.recvClient(response, responseSize, 0)){
		printf("Failed to receive response.\n");
		ts.closeSocket();
		return 1;
	}else{
		printf("Response : \n");
		for(int i=0; i<ts.recvSize; i++){
			printf("%c", response[i]);
		}printf("\n\n");
	}
	ts.closeSocket();
	return 0;
}
