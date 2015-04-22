#include "tdilib.h"


/*
* CONFIG
*/
bool ConfigHost::matchHost(string str) {
	if (host == str)
		return true;
	if (alias.size()) {
		for (int i=0; i<alias.size(); i++)
			if (alias[i] == str)
				return true;
	}
	return false;
}

int Config::loadFile(string fpath) {

	ifstream file(fpath.c_str(), ios::in | ios::binary);
	if (!file)
		return -1;

	string content, part, alias;
	int x = 0;
	int a, b, end;

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
		hosts[i].root = this->getStringValue(part, "root");
		hosts[i].index = this->getStringValue(part, "index");

		alias = this->getStringValue(part, "alias");
		if (alias.size()) {
			a = b = 0;
			while (b != string::npos) {
				b = alias.find(",", a);
				if (b == string::npos)
					hosts[i].alias.push_back(alias.substr(a));
				else {
					hosts[i].alias.push_back(alias.substr(a, b-a));
					a = b+1;
					while (alias[a] == ' ')
						a++;
				}
			}
		}

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
* RESPONSE
*/
HttpCookie::HttpCookie(string name_string, string value_string) {
	name = name_string;
	value = value_string;
	path = "/";
	secure = false;
	http = false;
}
string HttpCookie::toString() {
	string str = name+"="+value;
	if (expires.size())
		str+= "; Expires="+expires;
	if (domain.size())
		str+= "; Domain="+domain;
	if (path.size())
		str+= "; Path="+path;
	if (secure)
		str+= "; Secure";
	if (http)
		str+= "; HttpOnly";
	return str;
}
void HttpCookie::markDelete() {
	value = "delete";
	expires = "Thu Jan 1 01:00:00 1970";
}

HttpResponseHeader::HttpResponseHeader() {
	protocol = "HTTP/1.1";
	code = "200 OK";
	server = "TDI/0.1 (linux)";
	connection = "Keep-Alive";
	content_type = "text/html; charset=utf-8";
}
HttpResponseHeader::~HttpResponseHeader() {
	for (map<string, HttpCookie*>::iterator itr = cookies.begin();
			 itr != cookies.end(); itr++)
		delete itr->second;
}
string HttpResponseHeader::toString(int content_length) {

	string str;
	time_t now = time(0)-1000;
	string date = ctime(&now);

	str = 
		protocol+" "+code+"\r\n"+
		"Server: "+server+"\r\n"+
		"Connection: "+connection+"\r\n"+
		"Content-type: "+content_type+"\r\n"+
		"Date: "+date+
		"Content-Length: "+to_string(content_length);

	for (auto itr = cookies.begin(); itr != cookies.end(); itr++) 
		str+= "\r\nSet-Cookie: "+itr->second->toString();

	return str;

}
void HttpResponseHeader::setCookie(HttpCookie *cookie) {
	cookies[cookie->name] = cookie;
}
void HttpResponseHeader::deleteCookie(string name) {
	if (cookies.count(name) != 0) {
		cookies[name]->markDelete();
	}
}


string HttpResponse::toString() {
	string str = 
		header.toString(document.length())+
		"\r\n\r\n"+
		document;
	return str;
}
void HttpResponse::setCookie(HttpCookie *cookie) {
	header.setCookie(cookie);
}
void HttpResponse::deleteCookie(string name) {
	header.deleteCookie(name);
}


/*
* HANDLER
*/
void HttpHandler::sessionStart() {

	if (request.cookies.count("SESSID") == 0)
		response.setCookie(new HttpCookie("SESSID", randomString(24)));

	// TODO: Store session

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
string randomString(size_t length) {

	string chars = 
		"abcdefghijklmnopqrstuvwxyz"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"0123456789-_";
	string str = "";
	size_t size = chars.size();

	for (size_t i = 0; i<length; i++) 
		str+= chars[rand()%size];

	return str;
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