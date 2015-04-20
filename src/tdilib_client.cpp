#include <string>
#include <algorithm>
#include <jsoncpp/json/json.h>
#include "tdilib_client.h"

using namespace std;


/*
* FUNCTIONS
*/

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
Json::Value parsePostData(string content_type, string body) {

	Json::Value data;

	if (content_type == "application/x-www-form-urlencoded") {
		data = parseDataUrlencoded(body);
	}
	else if (content_type.find("multipart/form-data") != string::npos) {
		int x = content_type.find("boundary=");
		if (x != string::npos) {
			string boundary = "--"+content_type.substr(x+9);
			data = parsePostDataMultipart(boundary, body);
		}
	}

	return data;

}