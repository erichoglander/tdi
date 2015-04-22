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
		map<string, string> cookies;

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
		void markDelete();
};

class HttpResponseHeader {

	public:
		string protocol, code, server, connection, content_type;
		map<string, HttpCookie*> cookies;

		HttpResponseHeader();
		~HttpResponseHeader();
		string toString(int content_length);
		void setCookie(HttpCookie *cookie);
		void deleteCookie(string name);

};

class HttpResponse {
	
	public:
		HttpResponseHeader header;
		string document;

		string toString();
		void setCookie(HttpCookie *cookie);
		void deleteCookie(string name);
		void sessionStart();

};

class HttpHandler {

	public:
		HttpRequest request;
		HttpResponse response;

		void sessionStart();

};


/*
* FUNCTIONS
*/
void die(string str);

int utf8Length(string str);
string randomString(size_t length);

string fileExtension(string fpath);
string fileType(string fpath);


#endif