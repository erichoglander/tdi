/*
	
	TDI Client
	Töcke danne internet

	Minimalistic web client in C++

*/


#include <iostream>
#include <fstream>
#include <streambuf>
#include <cstdlib>
#include <vector>
#include <string>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <jsoncpp/json/json.h>
#include <tdilib.h>
#include <tdilib_client.h>


using namespace std;


int main(int argc, char *argv[]) {
	
	HttpRequest request;
	HttpResponse response;
	Json::Value _POST, _GET;

	if (argc < 2)
		die("No request headers");

	request.full.assign(argv[1]);
	request.parse();

	if (request.method == "POST")
		_POST = parsePostData(request.content_type, request.body);
	if (request.query.size())
		_GET = parseDataUrlencoded(request.query);

	response.document = 
		"<!DOCTYPE html><html>"
		"<head><meta charset=\"utf-8\"><title>Child test</title></head><body>";
	response.document+= "<h1>"+request.host+" "+request.method+"</h1>";
	response.document+= "<pre>"+request.full+"</pre>";
	response.document+= "<pre>"+_POST.toStyledString()+"</pre>";
	response.document+=
		"<form method=\"post\" action=\"\" enctype=\"multipart/form-data\">"
			"<input type=\"checkbox\" name=\"categories[]\" value=\"1\">"
			"<input type=\"checkbox\" name=\"categories[]\" value=\"2\">"
			"<input type=\"text\" name=\"address[line]\">"
			"<input type=\"text\" name=\"address[postal_code]\">"
			"<input type=\"text\" name=\"title\" placeholder=\"Title\"><br>"
			"<textarea name=\"message\" placeholder=\"Message\"></textarea><br>"
			"<input type=\"submit\" name=\"submit\" value=\"Send\">"
		"</form>";
	response.document+= "</body></html>";

	cout << response.toString();

	return 0;
}