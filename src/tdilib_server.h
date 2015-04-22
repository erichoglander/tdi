/*

	Library used for server

*/

#ifndef TDILIB_SERVER_H_
#define TDILIB_SERVER_H_

#include <string>
#include <vector>
#include <fstream>

using namespace std;


class ConfigHost {
	public:
		string name, host, root, index;
		vector<string> alias;

		bool matchHost(string str);
};
class Config {
	public:
		vector<ConfigHost> hosts;

		int loadFile(string fpath);
		string getStringValue(string str, string field);
};

#endif