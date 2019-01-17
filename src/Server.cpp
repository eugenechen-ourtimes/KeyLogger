#include "SocketAddr.hpp"
#include "option.h"

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <map>
#include <algorithm>

using namespace std;

static Option options[] = {
	{ OPT_UINT, "port", &port, "port number"}
};

template<typename KeyType, typename ValueType>
pair<KeyType,ValueType> maxValue(const map<KeyType,ValueType> &x)
{
	return *max_element(x.begin(), x.end(),
	[] (const pair<KeyType,ValueType> & p1, const pair<KeyType,ValueType> & p2)
	{
		return p1.second < p2.second;
	});
}

class Server {
	public:
		static const int maxConnection = 200000;
		FILE *logFile;

		fd_set readFds, copy;
		unsigned listenPort;

		Server();
		Server(unsigned listenPort);
		map < SocketAddr, int > connections;

		void run() {
			FD_SET(listenFd, &readFds);

			while (true) {
				struct timeval timeout;
				memcpy(&copy, &readFds, sizeof(fd_set));
				unsigned maxFd = getMaxFd();
				timeout.tv_sec = 0; timeout.tv_usec = 200000;
				int ret = select(maxFd + 1, &copy, NULL, NULL, &timeout);
				if (ret < 0) {
					perror("select");
					continue;
				}

				if (FD_ISSET(listenFd, &copy)) {
					addConnection();
					continue;
				}

				/* connections: socketAddr -> connFd */
				/* onlineUsers:  username -> socketAddr */

				checkConnections();
			}
		}

	private:
		int listenFd;

		unsigned getMaxFd();
		void addConnection();
		void checkConnections();
};

int main(int argc, char *argv[])
{
	Opt_Parse(argc, argv, options, Opt_Number(options), 0);
	Server server(port);
	server.run();
}

Server::Server()
{
	listenFd = 0;
	listenPort = 0;
	FD_ZERO(&readFds);
	FD_ZERO(&copy);
}

Server::Server(unsigned listenPort)
{
	this->listenPort = listenPort;

	listenFd = socket(PF_INET, SOCK_STREAM, 0);
	if (listenFd < 0) {
		fprintf(stderr, "socket open error\n");
		exit(-1);
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	if (bind(listenFd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		close(listenFd);
		fprintf(stderr, "port bind error\n");
		exit(-1);
	}

	if (listen(listenFd, 1024) < 0) {
		close(listenFd);
		fprintf(stderr, "port listen error\n");
		exit(-1);
	}

	FD_ZERO(&readFds);
	FD_ZERO(&copy);

	fprintf(stderr, "server init success\n");
}

unsigned Server::getMaxFd()
{
	if (connections.empty()) return listenFd;
	auto max = maxValue(connections);/* max in client's fd */
	int maxFd = listenFd;			/* server's listen fd */
	if (max.second > maxFd) maxFd = max.second;
	return maxFd;
}

void Server::addConnection()
{
	if (connections.size() == maxConnection) {
		fprintf(stderr, "connection full\n");
		return;
	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	const size_t connLen = sizeof(addr);

	int connFd = accept(listenFd,
					(struct sockaddr*)&addr,
					(socklen_t*)&connLen);
	if (connFd < 0) {
		if (errno == ENFILE) {
			fprintf(stderr, "out of file descriptor table\n");
			return;
		}

		if (errno == EINTR) return;

		fprintf(stderr, "accept err\n");
		fprintf(stderr, "code: %s\n", strerror(errno));
		return;
	}

	char ip[40];

	if (inet_ntop(addr.sin_family, &(addr.sin_addr), ip, 40) == NULL) {
		close(connFd);
		fprintf(stderr, "inet_ntop: %s\n", strerror(errno));
	}

	FD_SET(connFd, &readFds);
	SocketAddr socketAddr(string(ip), addr.sin_port);
	fprintf(stderr, "add %s:%u to connection\n", ip, addr.sin_port);
	connections[socketAddr] = connFd;
}

void Server::checkConnections()
{
	static char buf[100000];
	map < SocketAddr, int >::iterator c_it; /* c: connection */
	for (c_it = connections.begin(); c_it != connections.end(); c_it++) {
		if (FD_ISSET(c_it->second, &copy)) {
			/*
				char addr[100];
				sprintf(addr, "%s:%u.log", c_it->first.host().c_str(),
					c_it->first.port());
				*/
			char msgLen;
			int connFd = c_it->second;
			int ret = recv(connFd, &msgLen, 1, 0);

			if (ret == 0) {
				/* handle connection close */
				fprintf(stderr, "remove connection to %s:%u\n",
					c_it->first.host().c_str(),
					c_it->first.port()
					);
				connections.erase(c_it);
				close(connFd);
				FD_CLR(connFd, &readFds);
				return;
			}

			ret = recv(connFd, buf, msgLen, 0);

			if (ret == 0) {
				fprintf(stderr, "remove connection to %s:%u\n",
					c_it->first.host().c_str(),
					c_it->first.port()
					);
				connections.erase(c_it);
				close(connFd);
				FD_CLR(connFd, &readFds);
				return;
			}

			if (msgLen > 15) {
				fprintf(stderr, "[Error]\n");
				exit(-1);
			}

			buf[msgLen] = '\0';
			fprintf(stderr, "%s", buf);
		}
	}
}