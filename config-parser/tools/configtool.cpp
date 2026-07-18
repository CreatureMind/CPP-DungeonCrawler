#include "config/ConfigManager.h"
#include <iostream>
#include <sstream>
#include <string>

void printUsage(const char* programName) {
	std::cout << "Usage: " << programName << " <config_file>\n";
	std::cout << "Supported formats: .ini, .json\n";
}

void printHelp() {
	std::cout << "Commands:\n";
	std::cout << "  get / list       - List all keys and values\n";
	std::cout << "  get <key>        - Get value for a key (e.g., get graphics.width)\n";
	std::cout << "  set <key> <val>  - Set or add a key-value pair\n";
	std::cout << "  save             - Save changes to file\n";
	std::cout << "  quit / exit      - Exit (warns if unsaved changes)\n";
	std::cout << "  help             - Show this help\n";
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		printUsage(argv[0]);
		return 1;
	}

	ConfigManager manager;

	try {
		manager.load(argv[1]);
		std::cout << "Loaded configuration from " << argv[1] << "\n";
	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << "\n";
		return 1;
	}

	printHelp();
	std::cout << "\n";

	std::string line;
	while (true) {
		std::cout << "> ";
		if (!std::getline(std::cin, line)) break;

		std::istringstream iss(line);
		std::string command;
		iss >> command;

		if (command.empty()) continue;

		if (command == "quit" || command == "exit") {
			if (manager.hasUnsavedChanges()) {
				std::cout << "Warning: You have unsaved changes. Save before quitting? (y/n): ";
				std::string response;
				std::getline(std::cin, response);
				if (response == "y" || response == "Y" || response == "yes") {
					try {
						manager.save();
					} catch (const std::exception& e) {
						std::cerr << "Error saving: " << e.what() << "\n";
					}
				}
			}
			break;
		}

		if (command == "help") {
			printHelp();
			continue;
		}

		if (command == "list") {
			manager.listAll();
			continue;
		}

		if (command == "get") {
			std::string key;
			if (iss >> key) {
				auto value = manager.get(key);
				if (value) {
					std::cout << key << " = " << *value << "\n";
				} else {
					std::cout << "Key not found: " << key << "\n";
				}
			} else {
				manager.listAll();
			}
			continue;
		}

		if (command == "set") {
			std::string key, value;
			if (iss >> key) {
				std::getline(iss, value);
				size_t start = value.find_first_not_of(" \t");
				if (start != std::string::npos) {
					value = value.substr(start);
					manager.set(key, value);
				} else {
					std::cout << "Usage: set <key> <value>\n";
				}
			} else {
				std::cout << "Usage: set <key> <value>\n";
			}
			continue;
		}

		if (command == "save") {
			try {
				manager.save();
			} catch (const std::exception& e) {
				std::cerr << "Error saving: " << e.what() << "\n";
			}
			continue;
		}

		std::cout << "Unknown command: " << command << ". Type 'help' for available commands.\n";
	}

	return 0;
}
