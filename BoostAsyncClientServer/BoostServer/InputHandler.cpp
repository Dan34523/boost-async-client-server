#include "InputHandler.h"

const string permittedCommands[] = { "configure", "monitor", "resetall", "disconnect", "help", "clientconnect" };
const string permittedParameters[] = { "param1", "param2", "parameterChanges" };

// Takes the first word of the command and checks against array
bool commandIsValid(string const& command) {
	for (auto element : permittedCommands) {
		if (command == element || command.substr(0, command.find(" ")) == element) {
			return true;
		}
	}

	return false;
}

// Splits the string into whitespace separated substrings i.e. separate words
vector<string> parseCommand(string const& command) {
	vector<string> result;
	string currentWord = "";
	const int commandLength = static_cast<int>(command.length());

	for (int index = 0; index < commandLength; ++index) {
		char currentLetter = command[index];
		if (currentLetter == ' ') {
			result.push_back(currentWord);
			currentWord = "";
		}
		else {
			currentWord += currentLetter;
		}
	}

	result.push_back(currentWord);
	return result;
}

/*	Returns the message that will be sent to the client. First 3 characters of
	return value are metadata indicating type of response:
		- 000 = Broadcast to all connected clients
		- 001 = Reply to client who sent the message
		- 002 = Broadcast disconnect message to all clients
*/
string processCommand(string const& command, string const& clientName) {
	if (!commandIsValid(command)) {
		return "001ERROR: " + command + " is not defined. Use help for a list of valid commands";
	}

	if (command.substr(0, command.find(" ")) == "clientconnect") {
		return "000" + clientName + " has joined the server.";
	}

	else if (command.substr(0, command.find(" ")) == "configure") {
		/*	Passed a substring to this function. Removed "configure " from the string as we already know that
			the first word. Passing a full string would require another 10 executions of the for loop.
			Could perhaps parse before this if block, but may be a waste of time for single word commands.
			Performance with boths methods needs to be tested */
		const vector<string> parsedCommand = parseCommand(command.substr(10, -1));

		if (parsedCommand.capacity() < 2) {
			return "001ERROR: Configure unsuccesful - configure takes 2 arguments, only " + 
				to_string(parsedCommand.capacity()) + " provided";
		}

		// Send the data to VirtualEquipment.cpp to change the parameter. Returns whether change happened
		bool successful = changeParam(parsedCommand[0], parsedCommand[1], false);

		if (successful) {
			return "000" + clientName + " changed " + parsedCommand[0] + " to " + parsedCommand[1];
		}
		else {
			return "001Change was not committed. Unchangeable parameter";
		}
	}

	else if (command.substr(0, command.find(" ")) == "monitor") {
		/*	Don't need to parse this command as we know there is only one argument for this function.
			Just pass in the final word to the getParam function */
		string value = getParam(command.substr(8, -1));
		return "001" + value;
	}
}