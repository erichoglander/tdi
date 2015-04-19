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
	Json::Value _POST;

	if (argc < 2)
		die("No request headers");

	request.full.assign(argv[1]);
	request.parse();

	response.document = 
		"<!DOCTYPE html><html>"
		"<head><meta charset=\"utf-8\"><title>Child test</title></head><body>";
	response.document+= "<h1>"+request.host+"</h1>";
	response.document+= "<pre>"+request.full+"</pre>";
	response.document+= "</body></html>";

	cout << response.toString();

	return 0;
}