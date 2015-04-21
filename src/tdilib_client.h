#ifndef TDILIB_CLIENT_H_
#define TDILIB_CLIENT_H_

#include <string>
#include <algorithm>
#include <jsoncpp/json/json.h>

using namespace std;

string urlDecode(string encoded);
void keyToData(Json::Value *obj, string key, string value);
Json::Value parseDataUrlencoded(string body);
Json::Value parsePostDataMultipart(string boundary, string body);
Json::Value parsePostData(string content_type, string body);


#endif