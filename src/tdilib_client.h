#ifndef TDILIB_CLIENT_H_
#define TDILIB_CLIENT_H_

using namespace std;

string urlDecode(string encoded);
Json::Value parsePostDataUrlencoded(string body);
Json::Value parsePostDataMultipart(string boundary, string body);
Json::Value parsePostData(string content_type, string body);


#endif