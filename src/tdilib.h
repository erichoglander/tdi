/*

	Library used for server and client

*/

#ifndef TDILIB_H_
#define TDILIB_H_

#include <cstdlib>
#include <ctime>
#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>
#include <vector>
#include <map>
#include <sys/stat.h>

using namespace std;


class HttpRequest {
	public:
		string full, header, body;
		string host, method, uri, path, query, content_type, content_length;
		vector<string> params;
		map<string, string> cookies;

		void split();
		void parseHeader();
		void parse();
};


/*
* FUNCTIONS
*/
void die(string str);
vector<string> strSplit(string str, string delim);
string fileExtension(string fpath);
string fileType(string fpath);
string fileLoad(string fpath);
bool fileExists(string &fpath);


#endif