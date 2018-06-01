#include <string>
#include <vector>
#include "utils.h"

using namespace std;

vector<string> split(string str, string delim)
{
	std::vector<std::string> items;
	std::size_t dlm_idx;
	if (str.npos == (dlm_idx = str.find_first_of(delim)))
	{
		items.push_back(str.substr(0, dlm_idx));
	}
	while (str.npos != (dlm_idx = str.find_first_of(delim)))
	{
		if (str.npos == str.find_first_not_of(delim))
		{
			break;
		}
		items.push_back(str.substr(0, dlm_idx));
		dlm_idx++;
		str = str.erase(0, dlm_idx);
		if (str.npos == str.find_first_of(delim) && "" != str)
		{
			items.push_back(str);
			break;
		}
	}
	return items;
}

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

