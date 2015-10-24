/*

	Library used for client

*/


#ifndef TDILIB_CLIENT_H_
#define TDILIB_CLIENT_H_

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <tdilib.h>

using namespace std;



/*
* MIXED MAP
*/
class MixedMap {
	public:
		map<string, MixedMap*> children;
		string value;

		MixedMap(string val = "");
		~MixedMap();
		MixedMap& operator[](string key);
		void operator=(string val);
		void clear();
		void init(string key);
		bool isset(string key);
		MixedMap* get(string key);
		void set(string key, string val);
		string toString(int d = 0);
		string indent(int d);
};


/*
* HTTP
*/
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
		MixedMap get, post;
		string sessionPath, sessionId, sessionLoaded;
		MixedMap session;

		HttpHandler();
		~HttpHandler();
		void init();
		void parsePost();
		void sessionStart();
		int sessionLoad(string sessid);
		void sessionSave();
};


/* 
* FUNCTIONS
*/
int utf8Length(string str);
vector<string> splitString(string str, string delim);
string joinString(vector<string> vec, string delim);
string trim(string str);
string strReplace(string haystack, string needle, string replace);
string randomString(size_t length);
string urlDecode(string encoded);
void keyToData(MixedMap *data, string key, string value);
void parseDataUrlencoded(MixedMap *data, string body);
void parsePostDataMultipart(MixedMap *data, string boundary, string body);

#endif