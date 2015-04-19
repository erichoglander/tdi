#include <cstdlib>
#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>
#include <vector>
#include <map>
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

int Config::loadFile(string fpath) {

	ifstream file(fpath.c_str(), ios::in | ios::binary);
	if (!file)
		return -1;

	string content, part;
	int x = 0;
	int a, end;

	content.assign((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
	hosts.clear();

	for (int i=0; true; i++) {

		// Find [name]
		x = content.find("[", x);
		if (x == string::npos)
			break;
		a = content.find("]", x);
		if (a == string::npos)
			break;

		// Find end of host
		end = content.find("\n\n", x);
		if (end == string::npos)
			end = content.length();
		part = content.substr(x, end-x);

		hosts.push_back(ConfigHost());
		hosts[i].name = content.substr(x+1, a-x-1);
		hosts[i].host = this->getStringValue(part, "host");
		hosts[i].alias = this->getStringValue(part, "alias");
		hosts[i].root = this->getStringValue(part, "root");
		hosts[i].index = this->getStringValue(part, "index");

		if (end == content.length())
			break;
		x = end+2;

	}

	return 0;
}
string Config::getStringValue(string str, string field) {

	string row;
	int x, endline, eq;

	x = str.find(field);
	if (x == string::npos)
		return "";

	endline = str.find("\n", x);
	if (endline == string::npos)
		endline = str.length();

	row = str.substr(x, endline-x);

	eq = row.find("=");
	if (eq == string::npos)
		return "";

	for (eq++; row[eq] == ' '; eq++);

	return row.substr(eq);
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

	// Content type
	a = header.find("Content-Type: ");
	if (a != string::npos) {
		a+= 14;
		b = header.find("\r\n", a);
		content_type = header.substr(a, b-a);
	}

	// Content length
	a = header.find("Content-Length: ");
	if (a != string::npos) {
		a+= 16;
		b = header.find("\r\n", a);
		content_length = header.substr(a, b-a);
	}

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

void die (string str) {
	cout << str << endl;
	exit(0);
}

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