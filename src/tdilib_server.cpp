#include "tdilib_server.h"

// Match request string to host
bool ConfigHost::matchHost(string str) {
	if (host == str)
		return true;
	if (alias.size()) {
		for (int i=0; i<alias.size(); i++)
			if (alias[i] == str)
				return true;
	}
	return false;
}

int Config::loadFile(string fpath) {

	ifstream file(fpath.c_str(), ios::in | ios::binary);
	if (!file)
		return -1;

	string content, part, alias;
	int x = 0;
	int a, b, end;

	content.assign((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
	hosts.clear();

	for (int i=0; true; i++) {

		// Find [name]
		x = content.find("[", x);
		if (x == string::npos)
			break;
		a = content.find("]", x);
		if (a == string::npos)
			break;

		// Find end of host
		end = content.find("\n\n", x);
		if (end == string::npos)
			end = content.length();
		part = content.substr(x, end-x);

		hosts.push_back(ConfigHost());
		hosts[i].name = content.substr(x+1, a-x-1);
		hosts[i].host = this->getStringValue(part, "host");
		hosts[i].root = this->getStringValue(part, "root");
		hosts[i].index = this->getStringValue(part, "index");

		// Load aliases
		alias = this->getStringValue(part, "alias");
		if (alias.size()) {
			a = b = 0;
			while (b != string::npos) {
				b = alias.find(",", a);
				if (b == string::npos)
					hosts[i].alias.push_back(alias.substr(a));
				else {
					hosts[i].alias.push_back(alias.substr(a, b-a));
					a = b+1;
					while (alias[a] == ' ')
						a++;
				}
			}
		}

		if (end == content.length())
			break;
		x = end+2;

	}

	return 0;
}
// Get config value
string Config::getStringValue(string str, string field) {

	string row;
	int x, endline, eq;

	x = str.find(field);
	if (x == string::npos)
		return "";

	endline = str.find("\n", x);
	if (endline == string::npos)
		endline = str.length();

	row = str.substr(x, endline-x);

	eq = row.find("=");
	if (eq == string::npos)
		return "";

	for (eq++; row[eq] == ' '; eq++);

	return row.substr(eq);
}