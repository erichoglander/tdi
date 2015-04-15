#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include "tdilib.h"

using namespace std;


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
	a++;
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