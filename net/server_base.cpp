
#include "server_base.hpp"

namespace http {

	template <typename socket_type>
	void ServerBase<socket_type>::listenAndServer() {

		accept();

		for(size_t c=1; c < num_threads; c++) {
			threads.emplace_back([this]() {
				m_io_service.run();
			});
		}
			
		m_io_service.run();
			
		for(auto& t: threads)
			t.join();
	}


	template <typename socket_type>
	void ServerBase<socket_type>::respond(std::shared_ptr<socket_type> socket, std::shared_ptr<Request> request) const {
		auto method = request->method;
		auto path = request->path;
		auto iter_map = resource.find(method);
		// todo  add handle error
		if(iter_map == resource.end()) {
			return ;
		}

		auto iter_func = iter_map->second.find(path);
		if(iter_func != iter_map->second.end()) {
			auto write_buffer = std::make_shared<boost::asio::streambuf>();
			std::ostream response(write_buffer.get());     //shared_ptr's get() will return the stored pointer.
			
			iter_func->second(response, *request);		//call handle function
				
			//send response
			boost::asio::async_write(*socket, *write_buffer,
				[this, socket, request, write_buffer](const boost::system::error_code& ec, 
						size_t bytes_transfferred) {

						//keep-alive connection
				if(!ec && stof(request->http_version) > 1.05)
						process_request_and_respond(socket);
				});

			return;
		}
	}

	template <typename socket_type>
	http::Request ServerBase<socket_type>::parse_request(std::istream& stream) const {
		Request request;

		std::regex e("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");

		std::smatch sub_match;

		std::string line;
		getline(stream, line);
		line.pop_back();
		if(std::regex_match(line, sub_match, e)) {
			request.method		 = sub_match[1];
			request.path		 = sub_match[2];
			request.http_version	 = sub_match[3];

			bool matched;

			e = "^([^:]*): ?(.*)$";
			do{
				getline(stream, line);
				line.pop_back();
				matched = std::regex_match(line, sub_match, e);

				if(matched) {
					request.header[sub_match[1]] = sub_match[2];
				}
			}while(matched == true);
		}

		return request;
	}	

	template <typename socket_type>
	void ServerBase<socket_type>::process_request_and_respond(std::shared_ptr<socket_type> socket) const {
		auto read_buffer = std::make_shared<boost::asio::streambuf> ();

		boost::asio::async_read_until(*socket, *read_buffer, "\r\n\r\n",
		[this, socket, read_buffer](const boost::system::error_code& ec, size_t bytes_transferred) {
				if(!ec) {
					size_t total = read_buffer->size();

					std::istream stream(read_buffer.get());

					auto request = std::make_shared<Request>();
					*request = parse_request(stream);
					size_t num_additional_bytes = total - bytes_transferred;

					if(request->header.count("Content-Length") > 0) {
						boost::asio::async_read(*socket, *read_buffer,
						boost::asio::transfer_exactly(stoull(request->header["Content-Length"]) - 								num_additional_bytes), 
						[this, socket, read_buffer, request](const boost::system::error_code& ec,
							size_t bytes_transferred) {
							if(!ec) {
								request->content = std::shared_ptr<std::istream>(
										new std::istream(read_buffer.get()));
								respond(socket, request);
							}
						});
					} else {
						respond(socket, request);
					}
				}
			});
	}
}

