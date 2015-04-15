#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <vector>
#include "tdilib.h"

using namespace std;


/*
* CONFIG
*/
bool ConfigHost::matchHost(string str) {
	if (host == str)
		return true;
	if (alias.length()) {
		if (alias == str)
			return true;
		str+= ",";
		int x = alias.find(str);
		if (x != string::npos) {
			if (x == 0 || alias[x-1] == ' ')
				return true;
		}
		str = ", "+str.substr(-1);
		x = alias.find(str);
		if (x != string::npos) {
			if (x+str.length() == alias.length())
				return true;
		}
	}
	return false;
}


/*
* REQUEST
*/
void HttpRequest::split() {
	int x = full.find("\r\n\r\n");
	header = full.substr(0, x);
	body = full.substr(x+4);
}
void HttpRequest::parseHeader() {

	int a,b,c;

	// Method
	a = header.find(" ");
	method = header.substr(0, a);

	// Path and query
	a+= 2;
	b = header.find(" ", a);
	c = header.find("?", a);
	if (c != string::npos && c < b) {
		query = header.substr(c+1, b-c-1);
		b = c;
	}
	path = header.substr(a, b-a);

	// Host
	a = header.find("Host: ")+6;
	b = header.find("\r\n", a);
	c = header.find(":", a);
	if (c != string::npos && c < b) 
		b = c;
	host = header.substr(a, b-a);

}
void HttpRequest::parse() {
	this->split();
	this->parseHeader();
}



/*
* RESPONSE
*/
HttpResponseHeader::HttpResponseHeader() {
	protocol = "HTTP/1.1";
	code = "200 OK";
	server = "TDI/0.1 (linux)";
	connection = "Keep-Alive";
	content_type = "text/html; charset=utf-8";
}

string HttpResponseHeader::toString(int content_length) {

	string str;

	str = 
		protocol+" "+code+"\r\n"+
		"Server: "+server+"\r\n"+
		"Connection: "+connection+"\r\n"+
		"Content-type: "+content_type+"\r\n"+
		"Content-Length: "+to_string(content_length);

	return str;

}


string HttpResponse::toString() {
	string str;
	str = 
		header.toString(document.length())+
		"\r\n\r\n"+
		document;
	return str;
}



/*
* FUNCTIONS 
*/

int utf8Length(string str) {
	int bytes = str.length();
	int len = bytes;
	for (int i=0; i<bytes; i++) {
		if ((str[i] >> 7) & 1 && (str[i] >> 6) & 1)
			len--;
	}
	return len;
}

string fileExtension(string fpath) {
	string extension;
	int x = fpath.find_last_of(".");
	if (x != string::npos)
		extension = fpath.substr(x+1);
	return extension;
}
string fileType(string fpath) {

	string extension = fileExtension(fpath);
	string texts[4] = {"html", "htm", "css", "js"};
	string type;

	for (int i; i<4; i++) {
		if (texts[i] == extension) {
			type = "text/";
			type+= extension;
			type+= ";charset=utf-8";
			return type;
		}
	}

	type = "application/";
	type+= extension;

	return type;
}