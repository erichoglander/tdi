#ifndef TDILIB_H_
#define TDILIB_H_

#include <cstdlib>
#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>
#include <vector>
#include <map>

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


class HttpRequest {
	
	public:
		string full, header, body;
		string host, method, path, query, content_type, content_length;

		void split();
		void parseHeader();
		void parse();

};

class HttpCookie {
	public:
		string name, value, expires, domain, path;
		bool secure, http;

		HttpCookie(string name_string, string value_string);
		string toString();
};

class HttpResponseHeader {

	public:
		string protocol, code, server, connection, content_type;
		map<string, HttpCookie*> cookies;

		HttpResponseHeader();
		~HttpResponseHeader();
		string toString(int content_length);
		void setCookie(HttpCookie *cookie);

};

class HttpResponse {
	
	public:
		HttpResponseHeader header;
		string document;

		string toString();
		void setCookie(HttpCookie *cookie);

};


/*
* FUNCTIONS
*/
void die(string str);

int utf8Length(string str);

string fileExtension(string fpath);
string fileType(string fpath);


#endif