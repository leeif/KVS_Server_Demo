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
using namespace std;


std::vector<std::string> split(std::string str, std::string delim) {
  std::vector<std::string> items;
  std::size_t dlm_idx;
  if(str.npos == (dlm_idx = str.find_first_of(delim))) {
    items.push_back(str.substr(0, dlm_idx));
  }
  while(str.npos != (dlm_idx = str.find_first_of(delim))) {
    if(str.npos == str.find_first_not_of(delim)) {
      break;
    }
    items.push_back(str.substr(0, dlm_idx));
    dlm_idx++;
    str = str.erase(0, dlm_idx);
    if(str.npos == str.find_first_of(delim) && "" != str) {
      items.push_back(str);
      break;
    }
  }
  return items;
}
class Command
{
private:
	bool isSet;
	vector<string> commands{"set", "get", "del"};
	string currentCommand;
	vector<string> variable;

	bool checkCommands(string argument)
	{
		for (int i = 0; i < sizeof(commands) / sizeof(char *); i++)
		{
			if (commands[i] == argument)
			{
				currentCommand = argument;
				this->isSet = true;
			}
		}
		return false;
	}

	char *execCommand()
	{
	}

public:
	Command()
	{
		this->isSet = false;
	};

	void setArgument(string argument)
	{
		if (checkCommands(argument))
		{
			if (!this->isSet)
			{
				this->execCommand();
			}
			this->currentCommand = argument;
			this->isSet = true;
		}
	}

	~Command();
};

void moveFoward(char *array, int removeIndex)
{
	for (int i = removeIndex; i < strlen(array); i++)
	{
		array[i] = array[i + 1];
	}
}

void removeEnter(char *array)
{
	for (int i = 0; i < strlen(array); i++)
	{
		if (array[i] == '\n' || array[i] == '\r')
		{
			moveFoward(array, i);
			i--;
		}
	}
}

int main(int argc, char **argv)
{
	map<string, string> dataset;
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	Command command();
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

		while (true)
		{
			char *buffer = new char[4048];
			int length = recv(sock_client, buffer, sizeof(buffer), 0);
			removeEnter(buffer);
			string s = string(buffer);
			vector<string> items = split(s, " ");
			for(int i = 0; i < items.size(); i++) {
				cout << items[i] << endl;
			}
			cout << buffer << flush;
			
			if (strcmp(buffer, "quit") == 0)
			{
				close(sock_client);
				break;
			}
			delete[] buffer;
			// send(sock_client, send_buffer, strlen(send_buffer)+1, 0);
		}
	}
	return 0;
}
