#include <stdio.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>
#include <map>
#include <vector>
#include <pthread.h>
#include <signal.h>
#include "utils.h"

/**
 * Todo :
 * 	1. Multi-Thead
 *  2. Delete, Cas Command
 */
using namespace std;

map<string, string> dataset;
class Command
{
private:
	bool isSet;
	vector<string> variable;

	string execCommand(string data)
	{
		string ret = "";
		if (!this->isSet)
		{
			ret = "";
		}
		else
		{
			if (this->variable[0] == "set")
			{
				dataset[variable[1]] = data;
				ret = "Store";
			}
			else if (this->variable[0] == "get")
			{
				ret = dataset[variable[1]];
			}
			else if (this->variable[0] == "del")
			{
				ret = "";
			}
			else
			{
				ret = "Error";
			}
		}
		this->isSet = false;
		return ret;
	}

public:
	Command()
	{
		this->isSet = false;
	};

	string setArgument(vector<string>& input)
	{
		if (this->isSet)
		{
			return this->execCommand(input[0]);
		}

		this->variable = split(input[0], " ");

		if (this->variable[0] == "set" && this->variable.size() == 5)
		{
			this->isSet = true;
			if(input.size() > 1) {
				return this->execCommand(input[1]);
			} else {
				return "";
			}
		}
		if (this->variable[0] == "get" && this->variable.size() == 2)
		{
			this->isSet = true;
			return this->execCommand("");
		}
		return "Error";
	};
};

class ConnectThreadPool
{
private:
	vector<pthread_t> pools;

public:
	ConnectThreadPool()
	{
	}

	static void *worker(void *arg)
	{
		Command command;
		int sock_client = *(int *)arg;
		while (true)
		{
			char *buffer = new char[1024];
			int length = recv(sock_client, buffer, 1024, 0);
			string s = string(buffer);
			vector<string> commands = split(s, "\r\n");
			if (commands[0] == "quit")
			{
				close(sock_client);
				break;
			}
			string res = command.setArgument(commands);
			cout << res << endl;
			if (res != "")
			{
				char send_buffer[res.size() + 2];
				res.copy(send_buffer, res.size());
				send_buffer[res.size()] = '\n';
				send_buffer[res.size() + 1] = '\0';
				send(sock_client, send_buffer, strlen(send_buffer), 0);
			}
		}
		return (void *)NULL;
	}

	void newConnect(int sock_client)
	{
		pthread_t thread_handler;
		pthread_create(&thread_handler, NULL, &ConnectThreadPool::worker, &sock_client);
		//detach release resource when thread completed
		pthread_detach(thread_handler);
	}
};

int sock;

void close_sock(int sig) {
	if(sock >= 0) {
		close(sock);
	}
	exit(1);
}

int main(int argc, char **argv)
{
	ConnectThreadPool thread_pool;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		/* TODO ソケットが生成されない場合のエラー処理 */
		printf("socket failure!!! (%d)", errno);
		exit(0);
	}
	if (argv[1] == 0)
	{
		printf("Usage : \n $> You need input the port number\n");
		exit(0);
	}

	signal(SIGINT, close_sock);

	int port = atoi(argv[1]);
	struct sockaddr_in addr = {
			0,
	};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	const size_t addr_size = sizeof(addr);
	if (bind(sock, (struct sockaddr *)&addr, addr_size) == -1)
	{
		printf("bind error!!!(%d)\n", errno);
		close(sock);
		exit(0);
	}
	if (listen(sock, 10) == -1)
	{
		printf("listen error!!!(%d)\n", errno);
		close(sock);
		exit(0);
	}
	printf("listen success!!!\n");

	while (true)
	{
		struct sockaddr_in accept_addr = {
				0,
		};
		socklen_t accpet_addr_size = sizeof(accept_addr);
		int sock_client = accept(sock, (struct sockaddr *)&accept_addr, &accpet_addr_size);

		if (sock_client == 0 || sock_client < 0)
		{
			continue;
		}
		thread_pool.newConnect(sock_client);
	}
	return 0;
}
