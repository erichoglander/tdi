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
		string host, method, path, query, content_type, content_length;
		map<string, string> cookies;

		void split();
		void parseHeader();
		void parse();
};


/*
* FUNCTIONS
*/
void die(string str);
int utf8Length(string str);
string fileExtension(string fpath);
string fileType(string fpath);
string fileLoad(string fpath);
bool fileExists(string &fpath);


#endif