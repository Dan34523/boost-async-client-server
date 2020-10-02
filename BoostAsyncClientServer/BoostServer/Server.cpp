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

					const string serverResponse; // TODO : Assign processCommand()
					const string sendType = serverResponse.substr(0, 3);

					if (sendType == "001") {
						do_write(serverResponse.size(), serverResponse.c_str());
					}
					else if (sendType == "000" || sendType == "002") {
						broadcast_to_all();
					}
				}
			}
		);
	}

	void do_write(std::size_t length, const char* reply) {

	}

	void broadcast_to_all(string const& message) {

	}

	tcp::socket socket_;
	enum {max_length = 1024};
	char data_[max_length];

};

vector<session*> session::sessionList;

int main() {
	return 0;
}