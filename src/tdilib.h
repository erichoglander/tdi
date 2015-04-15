#ifndef TDILIB_H_
#define TDILIB_H_

using namespace std;


class ConfigHost {

	public:
		string host, alias, name, root, index;

		bool matchHost(string str);

};

class Config {

	public:
		vector<ConfigHost> hosts;

};


class HttpRequest {
	
	public:
		string full, header, body;
		string host, method, path, query;

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



int utf8Length(string str);

string fileExtension(string fpath);
string fileType(string fpath);


#endif