#ifndef TDILIB_H_
#define TDILIB_H_

using namespace std;


class ConfigHost {

	public:
		string name, host, alias, root, index;

		bool matchHost(string str);

};

class Config {

	public:
		vector<ConfigHost> hosts;

		int loadFile(string fpath);
		string getStringValue(string str, string field);

};


class HttpRequest {
	
	public:
		string full, header, body;
		string host, method, path, query, content_type, content_length;

		void split();
		void parseHeader();
		void parse();

};


class HttpResponseHeader {

	public:
		string protocol, code, server, connection, content_type;

		// Constructor
		HttpResponseHeader();

		// Joins all header parameters into a string
		string toString(int content_length);

};

class HttpResponse {
	
	public:
		HttpResponseHeader header;
		string document;

		string toString();

};


/*
* FUNCTIONS
*/
void die(string str);

int utf8Length(string str);

string fileExtension(string fpath);
string fileType(string fpath);


#endif