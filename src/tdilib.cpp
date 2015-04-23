#include "tdilib.h"


void HttpRequest::split() {
	int x = full.find("\r\n\r\n");
	header = full.substr(0, x);
	body = full.substr(x+4);
}
void HttpRequest::parseHeader() {

	int a,b,c,d;

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

	// Cookies
	a = header.find("Cookie: ");
	if (a != string::npos) {
		a+= 8;
		b = 0;
		c = header.find("\r\n", a);
		while (b<c) {
			b = header.find(";", a);
			if (b == string::npos)
				b = c;
			else if (b > c)
				break;
			d = header.find("=", a);
			if (d == string::npos || d > b)
				continue;
			cookies[header.substr(a, d-a)] = header.substr(d+1, b-d-1);
			a = b+1;
			while (header[a] == ' ');
				a++;
		}
	}

}
void HttpRequest::parse() {
	this->split();
	this->parseHeader();
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
string fileLoad(string fpath) {
	string content;
	ifstream file(fpath.c_str(), ios::in | ios::binary);
	if (file)
		content.assign((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
	return content;
}