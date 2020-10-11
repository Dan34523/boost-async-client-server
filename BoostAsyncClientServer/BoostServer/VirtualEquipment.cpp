#include "InputHandler.h"
#include <fstream>

using std::ifstream;
using std::ofstream;

string param1;
string param2;
int parameterChanges;

bool changeParam(const string& param, const string& value, const bool forceChange) {
	if (param == "param1") {
		param1 = value;
	}

	else if (param == "param2") {
		param2 = value;
	}

	else if (param == "parameterChanges" && forceChange) {
		parameterChanges = std::stoi(value);
		return true;
	}

	else {
		return false;
	}

	parameterChanges++;
	return true;
}

string getParam(const string& param) {
	if (param == "param1") return ("Parameter 1 Value: " + param1);
	else if (param == "param2") return ("Parameter 2 Value: " + param2);
	else if (param == "parameterChanges") return (to_string(parameterChanges) + " changes committed");
	else return "Parameter doesn't exist";
}

void resetParam() {
	param1 = "foo";
	param2 = "bar";
	parameterChanges = 0;
}

void initialiseParams() {
	ifstream ParameterFile("Parameters.txt");

	if (ParameterFile.good()) {
		for (size_t index = 0; index < 3; index++)
		{
			string line = "";
			getline(ParameterFile, line);

			auto parsedLine = parseCommand(line);
			changeParam(parsedLine[0], parsedLine[1], true);
		}
		ParameterFile.close();
	}
	else {
		ParameterFile.close();
		resetParam();

		ofstream ParameterFile("Parameters.txt");
		ParameterFile << "param1 foo\nparam2 bar\nparameterChanges 0";

		ParameterFile.close();
	}
}

void saveParams() {
	ofstream ParameterFile("Parameters.txt");
	ParameterFile << "param1 " << param1 << "\nparam2 " << param2 << "\nparameterChanges " << parameterChanges;
	ParameterFile.close();
}