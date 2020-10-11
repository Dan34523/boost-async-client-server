#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <boost/asio.hpp>

using namespace boost::asio;
using ip::tcp;

using std::cerr;
using std::endl;
using std::cout;
using std::cin;
using std::getline;

using std::string;
using std::to_string;
using std::vector;

extern string param1;
extern string param2;
extern int parameterChanges;

void saveParams();
bool changeParam(const string& param, const string& value, const bool forceChange);
string getParam(const string& param);
void resetParam();
string processCommand(string const& command, string const& clientName);
void initialiseParams();
vector<string> parseCommand(string const& command);