/*
	
	TDI Server
	Töcke danne internet

	Minimalistic web server in C++


	Usage:
		tdi [port]

*/

#include <cstdlib>
#include <cstring>
#include <csignal>
#include <cerrno>

#include <iostream>
#include <fstream>
#include <streambuf>
#include <vector>
#include <map>
#include <string>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include "tdilib.h"

using namespace std;


int sockfd, acceptfd;


void onExit() {
	cout << "Stopping server..." << endl;
	close(acceptfd);
	close(sockfd);
}
void signalHandler(int signal) {
	if (signal == 2)
		exit(0);
}
void error(string str) {
	// TODO: Error logging
	cout << "Error: " << str << endl << strerror(errno) << endl;
	exit(0);
}

int sendFile(int sockfd, string fpath, string code = "200 OK") {

	ifstream file(fpath.c_str(), ios::in | ios::binary);
	string content, ftype, header;

	content.assign((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

	ftype = fileType(fpath);

	header = 
		"HTTP/1.1 "+code+"\r\n"
		"Server: tdi/0.1 (linux)\r\n"
		"Connection: Keep-Alive\r\n"
		"Content-Type: "+ftype+"\r\n"
		"Content-Length: "+to_string(content.length())+
		"\r\n\r\n";

	write(sockfd, header.c_str(), header.size());
	write(sockfd, content.c_str(), content.size());

	return 0;
}

void htmlError(int sockfd, int code) {

	string code_string, fpath;

	if (code == 403)
		code_string = "403 Forbidden";
	else if (code == 404)
		code_string = "404 Not found";
	else if (code == 500)
		code_string = "500 Internal error";
	else
		code_string = to_string(code);

	fpath = "error/"+to_string(code)+".html";

	if (sendFile(sockfd, fpath, code_string) < 0)
		cout << "An error occured while trying to display an error document" << endl;
}



/*
* MAIN FUNCTION
*/
int main(int argc, char *argv[]) {

	// Socket stuff
	struct sockaddr_in server_addr, client_addr;
	socklen_t client_len = sizeof(client_addr);
	char buffer[1024*100]; // 100kB
	int buffer_len = sizeof(buffer);
	int port, bytes, optval;
	bool spawn_child;

	// Child process stuff
	char child_buffer[1024*100]; // 100kB
	int child_buffer_len = sizeof(child_buffer);
	pid_t pid;
	int child_pipe[2];
	fd_set readfds;
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;

	// Other variables
	HttpRequest request;
	HttpResponse response;
	Config config;
	string config_path = "tdi.conf";
	string str, fpath;
	int host;


	// Set exit callbacks
	signal(SIGINT, signalHandler);
	atexit(onExit);

	// Parse arguments
	if (argc > 1)
		port = atoi(argv[1]);


	if (access(config_path.c_str(), F_OK) < 0)
		error("Couldn't find config file");

	if (config.loadFile(config_path) < 0)
		die("Failed to load config");

	// Open socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("Failed to open socket");

	// Reuse port if in use
	optval = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) < 0)
		error("Failed setsockopt");


	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);

	if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) 
		error("Failed to bind socket");

	// Start listening to the socket for connections
	listen(sockfd, 5);
	cout << "Now listening on port " << port << endl;


	// Main loop
	while (1) {

		// Wait for connections
		acceptfd = accept(sockfd, (struct sockaddr *) &client_addr, &client_len);
		if (acceptfd < 0)
			error("Failed to accept connection");

		// Read from socket
		bzero(buffer, buffer_len);
		bytes = read(acceptfd, buffer, buffer_len-1);

		// Parse request
		request.full.assign(buffer);
		request.parse();

		cout << "Host: " << request.host;
		cout << ", Method: " << request.method;
		cout << ", Path: " << request.path;
		cout << ", Query: " << request.query << endl;

		// Find host
		host = 0;
		for (int i=0; i<config.hosts.size(); i++) {
			if (config.hosts[i].matchHost(request.host)) {
				host = i;
				break;
			}
		}

		// Check path for file
		spawn_child = false;
		if (request.path.size() == 0) {
			if (config.hosts[host].index.size() == 0 || config.hosts[host].index == "tdi")
				spawn_child = true;
			else {
				fpath = config.hosts[host].root;
				fpath+= "/public/";
				fpath+= config.hosts[host].index;
			}
		}
		else {
			fpath = config.hosts[host].root;
			fpath+= "/public/";
			fpath+= request.path;
		}

		if (!spawn_child) {
			if (access(fpath.c_str(), F_OK) < 0)
				spawn_child = true;
			else if (sendFile(acceptfd, fpath) < 0) {
				cout << "Failed to send file: "+fpath << endl;
				htmlError(acceptfd, 404);
			}
		}

		if (spawn_child) {

			fpath = config.hosts[host].root;
			fpath+= "tdi";
			if (access(fpath.c_str(), F_OK) < 0) {
				cout << "Couldn't find child executable" << endl;
				htmlError(acceptfd, 404);
			}
			else {

				pipe(child_pipe); // child_pipe[0] = read, child_pipe[1] = write
				pid = fork();

				// Child
				if (pid == 0) {

					close(child_pipe[0]);
					dup2(child_pipe[1], STDOUT_FILENO);
					close(child_pipe[1]);

					if (execl(fpath.c_str(), fpath.c_str(), request.full.c_str(), NULL) < 0) {
						cout << "Failed to execute child process" << endl;
						htmlError(acceptfd, 500);
					}
					_exit(0);

				}
				// Parent
				else {

					close(child_pipe[1]);
					FD_ZERO(&readfds);
					FD_SET(child_pipe[0], &readfds);
					select(child_pipe[0]+1, &readfds, NULL, NULL, &tv);

					if (FD_ISSET(child_pipe[0], &readfds)) {
						bzero(child_buffer, child_buffer_len);
						bytes = read(child_pipe[0], child_buffer, child_buffer_len-1);
						str.assign(child_buffer);
						while (bytes == child_buffer_len-1) {
							bytes = read(child_pipe[0], child_buffer, child_buffer_len-1);
							str+= child_buffer;
						}
						close(child_pipe[0]);
						write(acceptfd, str.c_str(), str.size());
					}

				}

			}

		}

		close(acceptfd);

	} // End main loop


	cout << "Ran to completion" << endl;

	return 0;

}
