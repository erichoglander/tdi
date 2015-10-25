#include "tdilib.h"


// Split request into header and body
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

	// Uri, path, query and params
	a+= 2;
	b = header.find(" ", a);
	c = header.find("?", a);
	uri = header.substr(a, b-a);
	if (c != string::npos && c < b) {
		query = header.substr(c+1, b-c-1);
		b = c;
	}
	path = header.substr(a, b-a);
	params = strSplit(path, "/");

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

// Split string into smaller strings
// Ex: strSplit("some,separated,words", ",") becomes "some", "separated", "words"
vector<string> strSplit(string str, string delim) {
	vector<string> vec;
	int a = 0, b = 0;
	while (true) {
		b = str.find(delim, a);
		if (b == string::npos) {
			vec.push_back(str.substr(a));
			break;
		}
		else {
			vec.push_back(str.substr(a, b-a));
			a = b + delim.size();
		}
	}
	return vec;
}

// Get file extension from a file path or name
string fileExtension(string fpath) {
	string extension;
	int x = fpath.find_last_of(".");
	if (x != string::npos)
		extension = fpath.substr(x+1);
	return extension;
}
// Get file type, so we can send certain files as text
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
// Load file contents into string
string fileLoad(string fpath) {
	string content;
	ifstream file(fpath.c_str(), ios::in | ios::binary);
	if (file)
		content.assign((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
	return content;
}
bool fileExists(string &fpath) {
	struct stat buffer;
	return (stat(fpath.c_str(), &buffer) == 0);
}