#include "IniParser.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

std::string IniParser::trim(const std::string& str) {
	size_t start = str.find_first_not_of(" \t\r\n");
	if (start == std::string::npos) return "";
	size_t end = str.find_last_not_of(" \t\r\n");
	return str.substr(start, end - start + 1);
}

std::string IniParser::removeComment(const std::string& line) {
	// Only treat ; as a comment marker (not # since # is used in map data)
	size_t pos = line.find(';');
	if (pos == std::string::npos) return line;
	return line.substr(0, pos);
}

ConfigData IniParser::parse(const std::string& path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		throw std::runtime_error("Cannot open file: " + path);
	}

	ConfigData data;
	std::string currentSection = "global";
	std::string line;
	std::string pendingKey;
	std::string pendingValue;

	auto commitPending = [&]() {
		if (!pendingKey.empty()) {
			ValueType type = ValueType::String;
			if (pendingValue == "true" || pendingValue == "false") {
				type = ValueType::Boolean;
			} else {
				bool isNumber = !pendingValue.empty();
				bool hasDot = false;
				for (size_t i = 0; i < pendingValue.size() && isNumber; ++i) {
					if (pendingValue[i] == '.' && !hasDot) hasDot = true;
					else if (pendingValue[i] == '-' && i == 0) continue;
					else if (!std::isdigit(pendingValue[i])) isNumber = false;
				}
				if (isNumber && !pendingValue.empty()) type = ValueType::Number;
			}
			data[currentSection][pendingKey] = ConfigValue(pendingValue, type);
			pendingKey.clear();
			pendingValue.clear();
		}
	};

	while (std::getline(file, line)) {
		std::string rawLine = line;
		line = trim(removeComment(line));

		// Check if this is a continuation line (starts with whitespace and no '=' sign)
		bool isContinuation = !rawLine.empty() && 
							  (rawLine[0] == ' ' || rawLine[0] == '\t') && 
							  !pendingKey.empty() &&
							  line.find('=') == std::string::npos &&
							  !line.empty() &&
							  line.front() != '[';

		if (isContinuation) {
			// Append continuation line to pending value (no separator - direct concatenation)
			pendingValue += line;
			continue;
		}

		// Commit any pending key-value before processing new content
		commitPending();

		if (line.empty()) continue;

		if (line.front() == '[' && line.back() == ']') {
			currentSection = trim(line.substr(1, line.size() - 2));
			continue;
		}

		size_t eqPos = line.find('=');
		if (eqPos != std::string::npos) {
			pendingKey = trim(line.substr(0, eqPos));
			pendingValue = trim(line.substr(eqPos + 1));
		}
	}

	// Commit final pending key-value
	commitPending();

	return data;
}

void IniParser::serialize(const ConfigData& data, const std::string& path) {
	std::ofstream file(path);
	if (!file.is_open()) {
		throw std::runtime_error("Cannot write to file: " + path);
	}

	for (const auto& [section, keys] : data) {
		if (section != "global") {
			file << "[" << section << "]\n";
		}
		for (const auto& [key, value] : keys) {
			file << key << " = " << value.raw << "\n";
		}
		file << "\n";
	}
}
