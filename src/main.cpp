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
#include "utils.h"

/**
 * Todo :
 * 	1. Multi-Thead
 *  2. Delete, Cas Command
 */
using namespace std;
class Command
{
private:
	bool isSet;
	vector<string> variable;
	map<string, string> dataset;

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
			} else if (this->variable[0] == "get")
			{
				ret = dataset[variable[1]];
			} else if (this->variable[0] == "del")
			{
				ret = "";
			} else {
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

	string setArgument(string input)
	{
		if (this->isSet)
		{
			return this->execCommand(input);
		}

		this->variable = split(input, " ");

		if (this->variable[0] == "set" && this->variable.size() == 5)
		{
			this->isSet = true;
			return "";
		}
		if (this->variable[0] == "get" && this->variable.size() == 2)
		{
			this->isSet = true;
			return this->execCommand("");
		}
		return "Error";
	};
};


int main(int argc, char **argv)
{
	map<string, string> dataset;
	int sock = socket(AF_INET, SOCK_STREAM, 0);
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
			exit(0);
		}

		Command command;
		while (true)
		{
			char *buffer = new char[1024];
			int length = recv(sock_client, buffer, 1024, 0);
			removeEnter(buffer);
			string s = string(buffer);
			if (s == "quit")
			{
				close(sock_client);
				break;
			}
			string res = command.setArgument(s);
			cout << res << endl;
			if(res != "") {
				char send_buffer[res.size()+2];
				res.copy(send_buffer, res.size());
				send_buffer[res.size()+1] = '\n';
				send_buffer[res.size()+2] = '\0';
				send(sock_client, send_buffer, strlen(send_buffer)+2, 0);
			}
		}
	}
	return 0;
}
