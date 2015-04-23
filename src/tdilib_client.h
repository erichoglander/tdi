/*

	Library used for client

*/


#ifndef TDILIB_CLIENT_H_
#define TDILIB_CLIENT_H_

#include <string>
#include <algorithm>
#include <jsoncpp/json/json.h>
#include <tdilib.h>

using namespace std;



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
		Json::Value get, post;
		string sessionPath, sessionId, sessionLoaded;
		Json::Value session;

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
string trim(string str);
string strReplace(string haystack, string needle, string replace);
string randomString(size_t length);
string urlDecode(string encoded);
void keyToData(Json::Value *obj, string key, string value);
Json::Value parseDataUrlencoded(string body);
Json::Value parsePostDataMultipart(string boundary, string body);

#endif