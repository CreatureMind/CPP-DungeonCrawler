#include "ConfigManager.h"
#include "ParserFactory.h"
#include <iostream>

void ConfigManager::load(const std::string& path) {
	parser_ = makeParser(path);
	data_ = parser_->parse(path);
	filePath_ = path;
	modified_ = false;
}

void ConfigManager::save() {
	parser_->serialize(data_, filePath_);
	modified_ = false;
	std::cout << "Configuration saved to " << filePath_ << "\n";
}

std::pair<std::string, std::string> ConfigManager::splitKey(const std::string& key) const {
	size_t dotPos = key.find('.');
	if (dotPos == std::string::npos) {
		return {"global", key};
	}
	return {key.substr(0, dotPos), key.substr(dotPos + 1)};
}

void ConfigManager::listAll() const {
	if (data_.empty()) {
		std::cout << "(no configuration data)\n";
		return;
	}

	for (const auto& [section, keys] : data_) {
		for (const auto& [key, value] : keys) {
			if (section == "global") {
				std::cout << key << " = " << value.raw << "\n";
			} else {
				std::cout << section << "." << key << " = " << value.raw << "\n";
			}
		}
	}
}

std::optional<std::string> ConfigManager::get(const std::string& key) const {
	auto [section, keyPart] = splitKey(key);

	auto secIt = data_.find(section);
	if (secIt != data_.end()) {
		auto keyIt = secIt->second.find(keyPart);
		if (keyIt != secIt->second.end()) {
			return keyIt->second.raw;
		}
	}
	return std::nullopt;
}

void ConfigManager::set(const std::string& key, const std::string& value) {
	auto [section, keyPart] = splitKey(key);

	ValueType type = ValueType::String;
	if (value == "true" || value == "false") {
		type = ValueType::Boolean;
	} else {
		bool isNumber = !value.empty();
		bool hasDot = false;
		for (size_t i = 0; i < value.size() && isNumber; ++i) {
			if (value[i] == '.' && !hasDot) hasDot = true;
			else if (value[i] == '-' && i == 0) continue;
			else if (!std::isdigit(value[i])) isNumber = false;
		}
		if (isNumber) type = ValueType::Number;
	}

	data_[section][keyPart] = ConfigValue(value, type);
	modified_ = true;
	std::cout << "Set " << key << " = " << value << "\n";
}

bool ConfigManager::hasUnsavedChanges() const {
	return modified_;
}
