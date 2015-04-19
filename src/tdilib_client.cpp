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
Json::Value parsePostDataUrlencoded(string body) {

	Json::Value data;
	Json::Value *obj;
	string key, value, k;
	int start, end, eq;
	int a,b,c;
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
		if (key.find("[") == string::npos)
			data[key] = value;
		else {
			obj = &data;
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

	return data;

}
Json::Value parsePostDataMultipart(string boundary, string body) {

	Json::Value data;

	data["boundary"] = boundary;

	return data;

}
Json::Value parsePostData(string content_type, string body) {

	Json::Value data;

	if (content_type == "application/x-www-form-urlencoded") {
		data = parsePostDataUrlencoded(body);
	}
	else if (content_type.find("multipart/form-data") != string::npos) {
		int x = content_type.find("boundary=");
		if (x != string::npos) {
			string boundary = content_type.substr(x+9);
			data = parsePostDataMultipart(boundary, body);
		}
	}

	return data;

}