#pragma once

#include "IConfigParser.h"

class IniParser : public IConfigParser {
public:
	ConfigData parse(const std::string& path) override;
	void serialize(const ConfigData& data, const std::string& path) override;

private:
	std::string trim(const std::string& str);
	std::string removeComment(const std::string& line);
};
