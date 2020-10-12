#include "InputHandler.h"

#include <cstdlib>
#include <memory>
#include <utility>
#include <mutex>

std::mutex mu;

class session : public std::enable_shared_from_this<session>
{
public:
	session(tcp::socket socket) : socket_(std::move(socket)) {
		
		mu.lock();
		sessionList.push_back(this);
		mu.unlock();
	}

	void start() {
		do_read();
	}

	static vector<session*> sessionList;

private:

	void do_read() {
		memset(data_, 0, 1024);
		auto self(shared_from_this());
		socket_.async_read_some(boost::asio::buffer(data_, max_length),
			[this, self](boost::system::error_code ec, std::size_t length)
			{
				if (!ec) {
					const string clientInput = string(data_, 0, length);
					const string clientName = clientInput.substr(0, clientInput.find(" "));
					const string clientCommand = clientInput.substr(clientInput.find(" ") + 1, -1);

					const string serverResponse = processCommand(clientCommand, clientName);
					const string sendType = serverResponse.substr(0, 3);

					if (sendType == "001") {
						do_write(serverResponse.size(), serverResponse.c_str());
					}
					else if (sendType == "000" || sendType == "002") {
						broadcast_to_all(serverResponse);
					}
				}
			}
		);
	}

	void do_write(std::size_t length, const char* reply) {
		auto self(shared_from_this());
		async_write(socket_, buffer(reply, length), 
			[this, self](boost::system::error_code ec, size_t /*length*/)
			{
				if (!ec) {
					do_read();
				}
			});
	}

	void broadcast_to_all(string const& message) {
		for (size_t i = 0; i < sessionList.size(); ++i) {
			sessionList[i]->do_write(message.length(), message.c_str());
		}
	}

	tcp::socket socket_;
	enum {max_length = 1024};
	char data_[max_length];

};

vector<session*> session::sessionList;

class server
{
public:
	server(io_context& io_context, short port) : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)){
		do_accept();
	}

private:

	void do_accept() {
		acceptor_.async_accept(
			[this](boost::system::error_code ec, tcp::socket socket)
			{
				if (!ec) {
					std::make_shared<session>(std::move(socket))->start();
				}

				do_accept();
			}
		);
	}

	tcp::acceptor acceptor_;
};

int main() {

	initialiseParams();

	try {
		io_context io_context;
		server s(io_context, 47000);
		io_context.run();
	}
	catch (std::exception& e) {
		cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}