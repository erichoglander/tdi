#include <string>
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
	string key, value;
	int a,b,c;
	bool more_values = true;

	for (a = 0; more_values; a = b+1) {
		b = body.find("&", a);
		if (b == string::npos) {
			b = body.size();
			more_values = false;
		}
		c = body.find("=", a);
		if (c == string::npos || c > b)
			continue;
		key = body.substr(a, c-a);
		value = body.substr(c+1, b-c-1);
		data[key] = urlDecode(value);
	}

	return data;

}
Json::Value parsePostDataMultipart(string body) {

	Json::Value data;

}
Json::Value parsePostData(string content_type, string body) {

	Json::Value data;

	if (content_type == "application/x-www-form-urlencoded") {
		data = parsePostDataUrlencoded(body);
	}
	else if (content_type == "multipart/form-data") {
		data = parsePostDataMultipart(body);
	}

	return data;
}