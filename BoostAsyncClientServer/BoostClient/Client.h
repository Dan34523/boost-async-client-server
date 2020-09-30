#pragma once

#include <iostream>
#include <boost/asio.hpp>
#include <string>
#include <queue>
#include <mutex>
#include <chrono>

#ifdef _WIN32 // Adds necessary headers for Sleep function
#include <Windows.h> // If on a Windows OS
#else
#include <unistd.h> // If on a POSIX/Linux derived OS
#endif

using namespace boost::asio;

// Using tags for iostream
using std::cerr;
using std::endl;
using std::cout;
using std::cin;
using std::getline;

// Using tags for other libraries
using std::string;
using std::queue;

using ip::tcp;
