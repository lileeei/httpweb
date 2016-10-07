//
// main_http.cpp
//


#include "./net/server_http.hpp"
#include <fstream>

using namespace std;
using namespace http;


/**/
void home(ostream& response, Request& request) {
	string filename = "web/";

	ifstream ifs;

	filename += "index.html";

	ifs.open(filename, ifstream::in);

	if(ifs) {
		ifs.seekg(0, ios::end);
		size_t length = ifs.tellg();

		ifs.seekg(0, ios::beg);

		response << "HTTP/1.1 200 ok\r\nContent-Length: " <<length << "\r\n\r\n" << ifs.rdbuf();
			
		ifs.close();
	} else {
		string content = "Could not open file " + filename;

		response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << content.length() << "\r\n\r\n" << content;
	}
}


void match(ostream& response, Request& request) {
	string content = "match content";
		
	response << "HTTP/1.1 200 OK\r\nContent-Length: " << content.length() << "\r\n\r\n" << content;
}



void post_string(ostream& response, Request& request) {
	stringstream ss;
	*request.content >> ss.rdbuf();
	string content = ss.str();
		
	response << "HTTP/1.1 200 OK\r\nContent-Length: " << content.length() << "\r\n\r\n" << content;
}

int main() {
	Server<HTTP> server(12345, 1);

	server.HandleFunc("/match", match);
	server.HandleFunc("/", home);
	
	server.HandleFunc("/string", post_string);
	server.listenAndServer();

	return 0;
}
