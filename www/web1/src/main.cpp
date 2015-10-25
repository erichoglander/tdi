/*
	
	TDI Client
	Töcke danne internet

	Minimalistic web client in C++

	This program is executed by the TDI server

*/


#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cerrno>

#include <iostream>
#include <fstream>
#include <streambuf>
#include <vector>
#include <string>
#include <unistd.h>

#include <tdilib.h>
#include <tdilib_client.h>


using namespace std;


int main(int argc, char *argv[]) {
	
	HttpHandler http;

	if (argc < 2)
		die("No request headers");

	srand(time(NULL));
	http.request.full.assign(argv[1]);	
	http.init();

	http.sessionStart();
	// http.session["user_id"] = "7";

	http.response.document = 
		"<!DOCTYPE html><html>"
		"<head><meta charset=\"utf-8\"><title>Child test</title></head><body>";
	http.response.document+= "\n<h1>"+http.request.host+" "+http.request.method+"</h1>";
	http.response.document+= "\n<pre>"+http.request.full+"</pre>";
	http.response.document+= "\n<h2>Session</h2>";
	http.response.document+= "\n<pre>"+http.session.toString()+"</pre>";
	http.response.document+= "\n<h2>POST</h2>";
	http.response.document+= "\n<pre>"+http.post.toString()+"</pre>";
	http.response.document+= "\n<h2>GET</h2>";
	http.response.document+= "\n<pre>"+http.get.toString()+"</pre>";
	http.response.document+= "</body></html>\n";

	cout << http.response.toString();

	return 0;
}