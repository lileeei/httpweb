//
//server_base.hpp
//

#ifndef SERVER_BASE_HPP
#define SERVER_BASE_HPP

#include <boost/asio.hpp>

#include <regex>
#include <unordered_map>
#include <thread>

namespace http {
	//http request sent by client
	struct Request {
		std::string method;  //request method like GET,POST
		std::string path;    //request path
		std::string http_version;   //request http_version like HTTP/1.1

		std::shared_ptr<std::istream> content;
		//request header
		std::unordered_map<std::string, std::string> header;
	};


	//resource type
	typedef std::map<std::string, std::unordered_map<std::string,
		std::function<void(std::ostream&, Request&)>>> resource_type;


	//base server class
	template <typename socket_type>
	class ServerBase {
	public:
		ServerBase(unsigned short port, size_t num_thread=1) :
			endpoint(boost::asio::ip::tcp::v4(), port),
			acceptor(m_io_service, endpoint),
			num_threads(num_thread) {}

		//start server
		void listenAndServer();


		//handle function to implement the routing
		typedef std::function<void(std::ostream&, Request&)> HandleType;
		void HandleFunc(std::string path, HandleType handler, std::string method = "GET") {
			resource[method][path] = handler;
		}
	
	protected:
		//different machine diferent method
		virtual void accept() {}

		//handle request and respond
		void process_request_and_respond(std::shared_ptr<socket_type> socket) const;

		void respond(std::shared_ptr<socket_type> socket, std::shared_ptr<Request> request) const;


		Request parse_request(std::istream& stream) const;


	protected:
		
        	// io_service
		boost::asio::io_service m_io_service;
		
        	// endpoint
		boost::asio::ip::tcp::endpoint endpoint;

		// acceptor
		boost::asio::ip::tcp::acceptor acceptor;

		//server thread
		size_t num_threads;
		std::vector<std::thread> threads;

		//handler all resource
		resource_type resource;
	};



	template<typename socket_type>
	class Server : public ServerBase<socket_type> {};

}
#endif
