/*
	
	TDI Client
	Töcke danne internet

	Minimalistic web client in C++

	This program is executed by the TDI server

*/


#include <cstdlib>
#include <cstring>
#include <cerrno>

#include <iostream>
#include <fstream>
#include <streambuf>
#include <vector>
#include <string>
#include <unistd.h>

#include <jsoncpp/json/json.h>

#include <tdilib.h>
#include <tdilib_client.h>


using namespace std;


int main(int argc, char *argv[]) {
	
	HttpHandler http;
	Json::Value _POST, _GET;

	if (argc < 2)
		die("No request headers");

	http.request.full.assign(argv[1]);
	http.request.parse();

	if (http.request.method == "POST")
		_POST = parsePostData(http.request.content_type, http.request.body);
	if (http.request.query.size())
		_GET = parseDataUrlencoded(http.request.query);

	http.sessionStart();

	http.response.document = 
		"<!DOCTYPE html><html>"
		"<head><meta charset=\"utf-8\"><title>Child test</title></head><body>";
	http.response.document+= "<h1>"+http.request.host+" "+http.request.method+"</h1>";
	http.response.document+= "<pre>"+http.request.full+"</pre>";
	http.response.document+= "<h2>Cookies</h2>";
	for (auto itr = http.request.cookies.begin();
			 itr != http.request.cookies.end(); 
			 itr++) {
		http.response.document+= itr->first+" = "+itr->second+"<br>";
	}
	http.response.document+= "<h2>POST</h2>";
	http.response.document+= "<pre>"+_POST.toStyledString()+"</pre>";
	http.response.document+= "<h2>GET</h2>";
	http.response.document+= "<pre>"+_GET.toStyledString()+"</pre>";
	http.response.document+=
		"<form method=\"post\" action=\"\" enctype=\"multipart/form-data\">"
			"<input type=\"checkbox\" name=\"categories[]\" value=\"1\">"
			"<input type=\"checkbox\" name=\"categories[]\" value=\"2\">"
			"<input type=\"text\" name=\"address[line]\">"
			"<input type=\"text\" name=\"address[postal_code]\">"
			"<input type=\"text\" name=\"title\" placeholder=\"Title\"><br>"
			"<textarea name=\"message\" placeholder=\"Message\"></textarea><br>"
			"<input type=\"submit\" name=\"submit\" value=\"Send\">"
		"</form>";
	http.response.document+= "</body></html>";

	cout << http.response.toString();

	return 0;
}