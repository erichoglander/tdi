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
		"<head>"
			"<meta charset=\"utf-8\">"
			"<title>Child test</title>"
		"</head>"
		"<body>"
			"<h1>"+http.request.host+"</h1>"
			"URI: "+http.request.uri+"<br>"
			"Path: "+http.request.path+"<br>"
			"Query: "+http.request.query+"<br>"
			"Params: "+strJoin(http.request.params, ", ")+"<br>"
			"GET: "+http.get.toString()+"<br>"
			"POST: "+http.post.toString()+"<br>"
			"<pre>"+http.request.full+"</pre>"
		"</body>"
		"</html>";

	cout << http.response.toString();

	return 0;
}