/*
	
	TDI Server
	Töcke danne internet

	Minimalistic web server in C++


	Usage:
		tdi [port]

*/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
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
void die (string str) {
	cout << str << endl;
	exit(0);
}
void error(string str) {
	// TODO: Error logging
	cout << "Error: " << str << endl << strerror(errno) << endl;
	exit(0);
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

	// Other variables
	HttpRequest request;
	HttpResponse response;
	string str;


	// Set exit callbacks
	signal(SIGINT, signalHandler);
	atexit(onExit);

	// Parse arguments
	if (argc > 1)
		port = atoi(argv[1]);


	// TODO: Load config


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

		request.full.assign(buffer);
		request.parse();

		cout << "Received " << bytes << " bytes" << endl;
		cout << "Host: " << request.host;
		cout << ", Method: " << request.method;
		cout << ", Path: " << request.path;
		cout << ", Query: " << request.query << endl;

		response.document = "<!DOCTYPE html><head><title>TDI test</title></head><body><pre>";
		response.document+= request.header;
		response.document+= "</pre></body></html>";

		str = response.toString();

		write(acceptfd, str.c_str(), str.size());

		close(acceptfd);

	} // End main loop


	cout << "Ran to completion" << endl;

	return 0;

}
