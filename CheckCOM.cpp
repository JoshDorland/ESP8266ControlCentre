#include <iostream>
#include <string>
#include <fstream>

//This function will extract the output from the console.
std::string execute(const std::string& command) {
	system((command + " > temp.txt").c_str());

	std::ifstream ifs("temp.txt");
	std::string ret{ std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>() };
	ifs.close(); // must close the inout stream so the file can be cleaned up
	if (std::remove("temp.txt") != 0) {
		perror("Error deleting temporary file");
	}
	return ret;
}

void checkCOM(int number) {
	std::string stuff, systemCommand, command, portNumber, status, systemOutput;

	//Displays system status, changes system settings, or reconfigures ports or devices
	command = "mode ";

	//Specifies the name of the device for which you want to display the status.
	portNumber = "com" + std::to_string(number); //Converts the integer to a string

	//Requests the status of any redirected parallel printers.
	status = " /status";

	systemCommand = command + portNumber + status;

	//This will convert the string to a constant character for use in system();
	const char* systemCommandChar = systemCommand.c_str();


	systemOutput = execute(systemCommandChar);

	//This will remove the whitespaces from the above command.
	systemOutput.erase(remove_if(systemOutput.begin(), systemOutput.end(), isspace), systemOutput.end());

	//This sets two conditions, general for valid inptus of numbers, and default if the user inputs a value like 'a' the console automatically outputs Illegaldevicename-COM0.
	std::string conditionGeneral = "Illegaldevicename-COM" + std::to_string(number);
	std::string conditionDefault = "Illegaldevicename-COM0";

	//This will check if the system ouput is equal to the two conditions. If one condition is true it will say "Device not connected". If the device is connected it will say "Device Connected"
	if ((systemOutput == conditionGeneral) || (systemOutput == conditionDefault)) {
		std::cout << "Device not connected.";
	}
	else {
		std::cout << "Device connected.";
	}
}