#include "Client.h"

std::mutex mu;
bool terminateProgram = false;

struct dataStruct {
	string data;
	bool ready;
};

queue<dataStruct> clientSendQueue;
queue<dataStruct> clientRecvQueue;

string read_(tcp::socket& sock, streambuf& buf, boost::system::error_code& err) {

	char buffer[512];
	const size_t len = sock.read_some(boost::asio::buffer(buffer), err);

	/*	socket.read_some changes the buffer to an array but doesn't add the null terminating
		character. Line 21 adds it on so it can be correctly used with iostream and string objects */
	buffer[len] = '\0';

	if (err && err != error::eof) {
		cerr << err.message() << endl;
		return "-1";
	}

	else {
		string data = buffer_cast<const char*> (buf.data());
		return buffer;
	}
}

void recvThread(tcp::socket& sock, streambuf& buf, boost::system::error_code& err, string const& username) {
	while (!terminateProgram) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		const string serverMsg = read_(sock, buf, err);

		if (serverMsg == "-1") {
			break;
		}

		dataStruct serverOut;
		serverOut.data = serverMsg;
		serverOut.ready = true;

		clientRecvQueue.push(serverOut);

		if (serverMsg.substr(0, serverMsg.find(" ")) == "002" + username) {
			cout << "Received Termination Message. Quitting..." << endl;
			terminateProgram = true;
		}
	}
}

void sendThread(tcp::socket& sock, boost::system::error_code& ec, string const& username) {
	while (!terminateProgram) {

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		while (!clientSendQueue.empty()) {

			const dataStruct front = clientSendQueue.front();

			if (front.ready) {

				write(sock, buffer(username + " " + front.data), ec);
				
				if (!ec) {
					clientSendQueue.pop();
				}

				else {
					cout << "Send failed. Error Message: " << ec.message() << endl;
				}

			}

		}

	}
}

int main() {
	return 0;
}