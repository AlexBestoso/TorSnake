#include <dlfcn.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/un.h>

class TorSnake{
	private:
		string torSocksSoLoc = "/usr/lib/x86_64-linux-gnu/torsocks/libtorsocks.so";
		void *soFd = NULL;

		void openLazy(void){
			soFd = dlopen(torSocksSoLoc.c_str(), RTLD_LAZY);
		}

		void closeSo(void){
			dlclose(soFd);
		}

		void *loadSymbol(string sym){
			void *ret = dlsym(soFd, sym.c_str());
			return ret;
		}


		/*
		 * Torified libc functions
		 * */
		int(*_socket)(int, int, int);
		int(*_getaddrinfo)(const char *, const char *, const struct addrinfo *, struct addrinfo **);
		struct hostent *(*_gethostbyname)(const char *);
		void(*_close)(int);
		int(*_connect)(int, const struct sockaddr *, socklen_t);
		ssize_t(*_send)(int, const void*, size_t, int);
		ssize_t(*_recv)(int, void *, size_t, int);
		/*
		 * Socket Variables
		 * */
		string host = "";
		int port = -1;
		int socketDescriptor  = -1;
		struct sockaddr_in sockAddr;
	public:
		int recvSize;
                int sendSize;

		TorSnake(){
			openLazy();
			_socket = (int(*)(int, int, int))loadSymbol("socket");
			_getaddrinfo = (int(*)(const char *, const char *, const struct addrinfo *, struct addrinfo **))loadSymbol("getaddrinfo");
			_gethostbyname = (struct hostent *(*)(const char *))loadSymbol("gethostbyname");
			_close = (void(*)(int))loadSymbol("close");
			_connect = (int(*)(int, const struct sockaddr *, socklen_t))loadSymbol("connect");
			_send = (ssize_t(*)(int, const void*, size_t, int))loadSymbol("send");
			_recv = (ssize_t(*)(int, void *, size_t, int))loadSymbol("recv");
		}

		~TorSnake(){
			closeSo();
		}

		void setSoLoc(string loc){
			torSocksSoLoc = loc;
		}

		bool createClient(string host, int port){
			this->host = host;
			this->port = port;
			if((socketDescriptor = _socket(AF_INET, SOCK_STREAM, 0)) < 0){
                                return false;
                        }

                        sockAddr.sin_family = AF_INET;
                        sockAddr.sin_port = htons(this->port);

                        if(inet_pton(AF_INET, this->host.c_str(), &sockAddr.sin_addr) <= 0){
                                /*struct addrinfo* res = NULL;
                                if(_getaddrinfo(host.c_str(), to_string(port).c_str(), 0, &res) != 0)
                                        return false;
                                sockAddr.sin_addr = ((struct sockaddr_in *) res->ai_addr)->sin_addr;*/
				struct hostent *host_domain;
				host_domain = _gethostbyname(host.c_str());
				if(host_domain == NULL){
					this->socketDescriptor = -1;
					return false;
				}
				sockAddr.sin_addr = *((struct in_addr *)host_domain->h_addr);
                        }

                        return true;
		}

		bool connectClient(){
                        if(_connect(socketDescriptor, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) < 0){
                                return false;
                        }
                        return true;
                }

		void closeSocket(){
                        _close(socketDescriptor);
                }

		bool recvClient(char *buffer, size_t bufferSize, int posRecv){
                        memset(buffer, 0x00, bufferSize);
                        if((recvSize = _recv(socketDescriptor, buffer, bufferSize, posRecv)) < 0){
                                return false;
                        }

                        return true;
                }

                bool sendClient(const char *buffer, size_t bufferSize){
                        if((sendSize = _send(socketDescriptor, buffer, bufferSize, 0)) < 0){
                                return false;
                        }
                        return true;
                }
};
