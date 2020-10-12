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

void inputThread() {
	while (!terminateProgram) {

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		string clientInput;
		getline(cin, clientInput);

		mu.lock();
		dataStruct inp;
		inp.data = clientInput;
		inp.ready = true;
		clientSendQueue.push(inp);
		mu.unlock();
	}
}

void outputThread() {
	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		while (!clientRecvQueue.empty()) {

			const dataStruct front = clientRecvQueue.front();

			if (front.ready) {
				cout << "<Server> " + front.data.substr(3, -1) << endl;
				clientRecvQueue.pop();
			}
		}
	}
}

void getConnectionInfo(string& ip, string& port, string& username) {
	cout << "Enter Server IP address. \n> ";
	getline(cin, ip);

	if (ip == "localhost") ip = "127.0.0.1";

	cout << "\nEnter Server Port.\n> ";
	getline(cin, port);

	cout << "\nEnter your username. This will be displayed to other users when you join\n> ";
	getline(cin, username);
}

int main() {
	string ip_address, port, username;
	io_service ioservice;
	tcp::socket socket(ioservice);

	getConnectionInfo(ip_address, port, username);
	bool inputIsValid = false;

	while (!inputIsValid) {

		// Assume the user has entered valid details
		inputIsValid = true;

		try {
			socket.connect(tcp::endpoint(ip::address::from_string(ip_address), std::stoi(port)));
		}
		catch (...) {
			cout << "\nInvalid connection data. Please enter details again\n" << endl;
			inputIsValid = false;
			getConnectionInfo(ip_address, port, username);
		}
	}

	streambuf receiveBuffer;
	boost::system::error_code error;

	write(socket, buffer(username + " clientconnect"), error);

	std::thread serverSendThread([&] {sendThread(socket, error, username); });
	std::thread serverRecvThread([&] {recvThread(socket, receiveBuffer, error, username); });
	std::thread userInputThread(inputThread);
	std::thread userOutputThread(outputThread);

	serverRecvThread.join();

	serverSendThread.detach();
	userInputThread.detach();
	userOutputThread.detach();

	socket.close();

	Sleep(2500);

	return 0;
}