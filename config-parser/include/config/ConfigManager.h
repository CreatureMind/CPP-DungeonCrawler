#pragma once

#include "ConfigData.h"
#include "IConfigParser.h"
#include <memory>
#include <string>
#include <optional>

class ConfigManager {
public:
	void load(const std::string& path);
	void save();
	void listAll() const;
	std::optional<std::string> get(const std::string& key) const;
	void set(const std::string& key, const std::string& value);
	bool hasUnsavedChanges() const;

private:
	std::pair<std::string, std::string> splitKey(const std::string& key) const;

	std::unique_ptr<IConfigParser> parser_;
	ConfigData data_;
	std::string filePath_;
	bool modified_ = false;
};
