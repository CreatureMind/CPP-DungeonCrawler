#pragma once

#include "ConfigData.h"
#include <string>

class IConfigParser {
public:
	virtual ~IConfigParser() = default;
	virtual ConfigData parse(const std::string& path) = 0;
	virtual void serialize(const ConfigData& data, const std::string& path) = 0;
};
