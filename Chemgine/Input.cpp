#include "Input.hpp"

std::istream& Input::inputStream = std::cin;
std::ostream& Input::outputStream = std::cout;

std::string Input::get(const std::string& msg)
{
	if (msg.empty() == false)
		outputStream << msg << '\n';

	outputStream << ">>> ";

	std::string line;
	std::cin >> line;
	return line;
}
