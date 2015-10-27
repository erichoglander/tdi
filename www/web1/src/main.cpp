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


/**
* PAGE HANDLER
*/
class PageHandler {
	public:
		string title, head, content;
		PageHandler(HttpHandler *ht);
		void redirect(string location, string code = "302");
		void execute();
	private:
		HttpHandler *http;
		void pageIndex();
};

PageHandler::PageHandler(HttpHandler *ht) {
	http = ht;
}
void PageHandler::redirect(string location, string code) {
	if (code.size())
		http->response.header.code = code;
	http->response.header.location = location;
	http->response.document = "";
	die(http->response.toString());
}
void PageHandler::execute() {
	if (http->request.params[0].size() == 0) 
		pageIndex();
}

// Index page
void PageHandler::pageIndex() {
	head = "";
	title = "Front page";
	if (http->request.method == "POST") {
		redirect("?title="+title);
	}
	else {
		content = 
			"<form method=\"POST\" action=\"\">"
				"<label>Title</label>"
				"<input type=\"text\" name=\"title\">"
				"<input type=\"submit\" name=\"submit\" value=\"Spara\">"
			"</form>";
	}
}


/**
* MAIN
*/
int main(int argc, char *argv[]) {
	
	HttpHandler http;
	PageHandler page(&http);

	if (argc < 2)
		die("No request headers");

	srand(time(NULL));
	http.request.full.assign(argv[1]);	
	http.init();

	http.sessionStart();
	// http.session["user_id"] = "7";

	page.execute();

	http.response.document = 
		"<!DOCTYPE html><html>"
		"<head>"
			"<meta charset=\"utf-8\">"
			"<title>"+page.title+"</title>"
		"</head>"
		"<body>"
			"<h1>"+http.request.host+"</h1>"
			"URI: "+http.request.uri+"<br>"
			"Path: "+http.request.path+"<br>"
			"Query: "+http.request.query+"<br>"
			"Params: "+strJoin(http.request.params, ", ")+"<br>"
			"GET: "+http.get.toString()+"<br>"
			"POST: "+http.post.toString()+"<br>"
			"<div id=\"page\">"+
				page.content+
			"</div>"
		"</body>"
		"</html>";

	cout << http.response.toString();

	return 0;
}