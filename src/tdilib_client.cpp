#include "tdilib_client.h"


/*
* MIXED MAP
*/
// Allows for construction through: data["mykey"] = *(new MixedMap("myvalue"));
MixedMap::MixedMap(string val) {
	value = val;
}
MixedMap::~MixedMap() {
	clear();
}
// Allows get through: data["mykey"]
MixedMap& MixedMap::operator[](string key) {
	if (!isset(key))
		init(key);
	return *children[key];
}
// Allows set through: data["mykey"] = "myvalue";
void MixedMap::operator=(string val) {
	value = val;
}

void MixedMap::clear() {
	for (auto itr = children.begin(); itr != children.end(); itr++)
		delete itr->second;
	children.clear();
}
void MixedMap::init(string key) {
	children[key] = new MixedMap();
}
bool MixedMap::isset(string key) {
	return children.count(key) == 1;
}
MixedMap* MixedMap::get(string key) {
	return children.at(key);
}
void MixedMap::set(string key, string val) {
	if (!isset(key))
		init(key);
	children[key]->value = val;
}
string MixedMap::toString(int d) {
	if (children.size()) {
		string str;
		str+= "{\n";
		for (auto itr = children.begin(); itr != children.end(); itr++) {
			str+= indent(d+1) + "\"" + itr->first + "\": " + itr->second->toString(d+1);
			if (itr != (--children.end()))
				str+= ",";
			str+= "\n";
		}
		str+= indent(d)+"}";
		if (d == 0)
			str+= "\n";
		return str;
	}
	else if (value.size()) {
		return "\""+value+"\"";
	}
	else {
		return "null";
	}
}
void MixedMap::fromString(string str) {
	int a, b, c, i;
	string key, val;
	clear();
	str = strTrim(str);
	if (str[0] == '"') {
		a = str.find_last_of('"');
		if (a == string::npos) 
			throw "Missing closing \"";
		else 
			value = str.substr(1, a-1);
	}
	else if (str[0] == '{') {
		if (str.back() != '}')
			throw "Missing closing }";
		a = str.find('"');
		while (a != string::npos) {
			a++;
			b = str.find('"', a);
			key = str.substr(a, b-a);
			init(key);
			for (c = b+1; c<str.size(); c++) {
				if (str[c] != ':' && str[c] != ' ' && str[c] != '\t' && str[c] != '\r' && str[c] != '\n')
					break;
			}
			if (c == str.size())
				throw "Unexpected end, key found but no value";
			i = findEnd(str, c);
			if (str[c] == '"') 
				set(key, str.substr(c+1, i-c-1));
			else if (str[c] == '{' || str[c] == '[') 
				children[key]->fromString(str.substr(c, i-c+1));
			else 
				set(key, str.substr(c, i-c+1));
			a = str.find('"', i+1);
		}
	}
	else if (str[0] == '[') {
		if (str.back() != ']')
			throw "Missing closing ]";
		for (a = 1, b = 0; a<str.size(); a = i+1, b++) {
			// Find start of next value
			for (c = a; c<str.size(); c++) {
				if (str[c] != ',' && str[c] != ' ' && str[c] != '\t' && str[c] != '\r' && str[c] != '\n')
					break;
			}
			if (str[c] == ']')
				break;
			key = to_string(b);
			i = findEnd(str, c);
			if (str[c] == '"') {
				set(key, str.substr(c+1, i-c-1));
			}
			else if (str[c] == '{' || str[c] == '[') {
				children[key]->fromString(str.substr(c, i-c+1));
			}
			else {
				val = str.substr(c, i-c+1);
				if (val == "null")
					init(key);
				else
					set(key, str.substr(c, i-c+1));
			}
		}
	}
}
string MixedMap::indent(int d) {
	string str;
	for (int i=0; i<d; i++)
		str+= "  ";
	return str;
}
int MixedMap::findEnd(string str, int c) {
	int i;
	bool inq = false;
	if (str[c] == '"') {
		c++;
		for (i=c; i<str.size(); i++) {
			if (str[i] == '\\')
				i++;
			else if (str[i] == '"')
				break;
		}
		if (i == str.size())
			throw "Missing closing \"";
	}
	else if (str[c] == '{') {
		int bracket = 1;
		for (i=c+1; i<str.size(); i++) {
			if (str[i] == '\\')
				i++;
			else if (str[i] == '"')
				inq = !inq;
			else if (str[i] == '{' && !inq)
				bracket++;
			else if (str[i] == '}' && !inq) {
				bracket--;
				if (bracket == 0)
					break;
			}
		}
		if (i == str.size())
			throw "Missing closing }";
	}
	else if (str[c] == '[') {
		int bracket = 1;
		for (i=c+1; i<str.size(); i++) {
			if (str[i] == '\\')
				i++;
			else if (str[i] == '"')
				inq = !inq;
			else if (str[i] == '[' && !inq)
				bracket++;
			else if (str[i] == ']' && !inq) {
				bracket--;
				if (bracket == 0)
					break;
			}
		}
		if (i == str.size())
			throw "Missing closing ]";
	}
	else {
		i = str.find(',', c);
		if (i == string::npos) 
			i = str.find(' ', c);
		if (i != string::npos)
			i--;
	}
	return i;
}


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
	for (auto itr = cookies.begin(); itr != cookies.end(); itr++)
		delete itr->second;
}
string HttpResponseHeader::toString(int content_length) {

	string str;
	time_t now = time(0)-1000;
	string date = ctime(&now);

	str = 
		protocol+" "+code+"\r\n"
		"Server: "+server+"\r\n"
		"Connection: "+connection+"\r\n"
		"Content-type: "+content_type+"\r\n"
		"Date: "+date+
		"Content-Length: "+to_string(content_length);

	if (location.size())
		str+= "\r\nLocation: "+location;

	if (cookies.size()) {
		for (auto itr = cookies.begin(); itr != cookies.end(); itr++) 
			str+= "\r\nSet-Cookie: "+itr->second->toString();
	}

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
		this->parsePost();
	if (request.query.size())
		parseDataUrlencoded(&get, request.query);
}
void HttpHandler::parsePost() {
	if (request.content_type == "application/x-www-form-urlencoded") {
		parseDataUrlencoded(&post, request.body);
	}
	else if (request.content_type.find("multipart/form-data") != string::npos) {
		int x = request.content_type.find("boundary=");
		if (x != string::npos) {
			string boundary = "--"+request.content_type.substr(x+9);
			parsePostDataMultipart(&post, boundary, request.body);
		}
	}
}
void HttpHandler::sessionStart() {
	if (request.cookies.count("SESSID") != 0) {
		if (sessionLoad(request.cookies["SESSID"]) == 0)
			sessionId = request.cookies["SESSID"];
	}
	if (!sessionId.size()) {
		sessionId = strRandom(24);
		response.setCookie(new HttpCookie("SESSID", sessionId));
	}
}
int HttpHandler::sessionLoad(string sessid) {
	string fpath = sessionPath+"/"+sessid;
	if (!fileExists(fpath))
		return -1;
	sessionLoaded = fileLoad(fpath);
	if (sessionLoaded.size()) 
		session.fromString(sessionLoaded);
	return 0;
}
void HttpHandler::sessionSave() {
	if (!sessionId.size())
		return;
	string content = session.toString();
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
int utf8Length(string str) {
	int bytes = str.length();
	int len = bytes;
	for (int i=0; i<bytes; i++) {
		if ((str[i] >> 7) & 1 && (str[i] >> 6) & 1)
			len--;
	}
	return len;
}
string strJoin(vector<string> vec, string delim) {
	string str;
	for (int i=0; i<vec.size(); i++) {
		if (i != 0)
			str+= delim;
		str+= vec[i];
	}
	return str;
}
string strTrim(string str) {
	string whitespace = " \r\n\t";
	size_t size = str.size();
	size_t wsize = whitespace.size();
	int i, j, k;

	// Leading whitespace
	j = 0;
	for (i=0; i<size; i++) {
		for (j=0; j<wsize; j++) {
			if (str[i] == whitespace[j])
				break;
		}
		if (j == wsize)
			break;
	}
	if (i == size)
		return str.substr(i);

	// Trailing whitespace
	j = 0;
	for (k=size-1; k>=0; k--) {
		for (j=0; j<wsize; j++) {
			if (str[k] == whitespace[j])
				break;
		}
		if (j == wsize)
			break;
	}
	return str.substr(i, k-i+1);
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
string strRandom(size_t length) {

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
void keyToData(MixedMap *data, string key, string value) {

	string k;
	int a,b;

	if (key.find("[") == string::npos) {
		data->set(key, value);
	}
	else {
		// Turn ab[cd][ef][gh] into ab[cd[ef[gh so it's easy to split
		key.erase(remove(key.begin(), key.end(), ']'), key.end());
		a = 0;
		while (true) {
			b = key.find("[", a); // Find next occurence
			if (b == string::npos) {
				data->set(key.substr(a), value);
				break;
			}
			else if (b+1 == key.size()) {
				k = key.substr(a, b-a);
				data->set(k, value);
				break;
			}
			else {
				k = key.substr(a, b-a);
				if (!data->isset(k))
					data->init(k);
				data = data->get(k);
				a = b+1;
			}
		}
	}

}
void parseDataUrlencoded(MixedMap *data, string body) {

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
		keyToData(data, key, value);
	}

}
void parsePostDataMultipart(MixedMap *data, string boundary, string body) {

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
		keyToData(data, key, value);
	}

}