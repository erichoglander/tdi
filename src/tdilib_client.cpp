#include "tdilib_client.h"


/*
* HTTP
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

HttpHandler::HttpHandler() {
	sessionPath = "sessions";
}
HttpHandler::~HttpHandler() {
	sessionSave();
}
void HttpHandler::init() {
	request.parse();
	if (request.method == "POST")
		parsePost();
	if (request.query.size())
		get = parseDataUrlencoded(request.query);
}
void HttpHandler::parsePost() {
	if (request.content_type == "application/x-www-form-urlencoded") {
		post = parseDataUrlencoded(request.body);
	}
	else if (request.content_type.find("multipart/form-data") != string::npos) {
		int x = request.content_type.find("boundary=");
		if (x != string::npos) {
			string boundary = "--"+request.content_type.substr(x+9);
			post = parsePostDataMultipart(boundary, request.body);
		}
	}
}
void HttpHandler::sessionStart() {
	if (request.cookies.count("SESSID") != 0) {
		if (sessionLoad(request.cookies["SESSID"]) == 0)  
			sessionId = request.cookies["SESSID"];
	}
	if (!sessionId.size()) {
		sessionId = randomString(24);
		response.setCookie(new HttpCookie("SESSID", sessionId));
	}
}
int HttpHandler::sessionLoad(string sessid) {
	string fpath = sessionPath+"/"+sessid;
	if (access(fpath.c_str(), F_OK) < 0)
		return -1;
	string content = fileLoad(fpath);
	if (content.size()) {
		Json::Reader reader;
		reader.parse(content, session);
		sessionLoaded = session.toStyledString();
	}
	return 0;
}
void HttpHandler::sessionSave() {
	if (!sessionId.size())
		return;
	string content = session.toStyledString();
	if (sessionLoaded == content)
		return;
	string fpath = sessionPath+"/"+sessionId;
	ofstream out(fpath.c_str(), ofstream::binary);
	out.write(content.c_str(), sizeof(char)*content.size());
	out.close();
}


/*
* FUNCTIONS
*/
string trim(string str) {
	string whitespace = " \r\n\t";
	size_t size = str.size();
	size_t wsize = whitespace.size();
	int i, j, k;

	// Leading whitespace
	j = 0;
	for (i=0; i<size && j != wsize; i++) {
		for (j=0; j<wsize; j++) {
			if (str[i] == whitespace[j])
				break;
		}
	}
	if (i == size)
		return str.substr(i);

	// Trailing whitespace
	j = 0;
	for (k=size-1; k>=0 && j != wsize; k--) {
		for (j=0; j<wsize; j++) {
			if (str[k] == whitespace[j])
				break;
		}
	}
	return str.substr(i, k-i);
}
string strReplace(string haystack, string needle, string replace) {
	string str = haystack;
	int x = str.find(needle);
	while (x != string::npos) {
		str.replace(x, needle.size(), replace);
		str.find(needle);
	}
	return str;
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
string urlDecode(string encoded) {
	
	string decoded = encoded;
	int size = decoded.size();
	string hex, replace;
	for (int i=0; i<size; i++) {
		if (decoded[i] == '%') {
			hex = decoded.substr(i+1, 2);
			replace = stoul(hex, 0, 16);
			decoded.replace(i, 3, replace);
			size = decoded.size();
		}
		else if (decoded[i] == '+') {
			decoded[i] = ' ';
		}
	}

	return decoded;

}
void keyToData(Json::Value *obj, string key, string value) {

	string k;
	int a,b;

	if (key.find("[") == string::npos)
		(*obj)[key] = value;
	else {
		// Turn ab[cd][ef][gh] into ab[cd[ef[gh so it's easy to split
		key.erase(remove(key.begin(), key.end(), ']'), key.end());
		a = 0;
		while (true) {
			b = key.find("[", a); // Find next occurence
			if (b == string::npos) {
				(*obj)[key.substr(a)] = value;
				break;
			}
			else if (b+1 == key.size()) {
				k = key.substr(a, b-a);
				if (!(*obj).isMember(k))
					(*obj)[k] = Json::Value(Json::arrayValue);
				(*obj)[k].append(value);
				break;
			}
			else {
				k = key.substr(a, b-a);
				if (!(*obj).isMember(k))
					(*obj)[k] = Json::Value(Json::objectValue);
				obj = &((*obj)[k]);
				a = b+1;
			}
		}
	}

}
Json::Value parseDataUrlencoded(string body) {

	Json::Value data;
	string key, value;
	int start, end, eq;
	bool more_values = true;

	for (start = 0; more_values; start = end+1) {
		end = body.find("&", start);
		if (end == string::npos) {
			end = body.size();
			more_values = false;
		}
		eq = body.find("=", start);
		if (eq == string::npos || eq > end)
			continue;
		key = urlDecode(body.substr(start, eq-start));
		value = urlDecode(body.substr(eq+1, end-eq-1));
		keyToData(&data, key, value);
	}

	return data;

}
Json::Value parsePostDataMultipart(string boundary, string body) {

	Json::Value data;
	string key, value;
	int start, end;
	int a,b;

	for (start = 0; start != string::npos; start = body.find(boundary, end)) {
		end = body.find(boundary, start+boundary.size());
		if (end == string::npos)
			break;
		a = body.find("name=\"", start);
		if (a == string::npos || a > end)
			continue;
		a+= 6;
		b = body.find("\"", a);
		if (b == string::npos)
			continue;
		key = body.substr(a, b-a);
		a = body.find("\n", b);
		if (a == string::npos)
			continue;
		a = body.find("\n", a+1);
		if (a == string::npos)
			continue;
		a++;
		value = body.substr(a, end-a);
		a = value.size()-1;
		if (value[a] == '\n') {
			value.pop_back();
			a--;
		}
		if (value[a] == '\r')
			value.pop_back();
		keyToData(&data, key, value);
	}

	return data;

}