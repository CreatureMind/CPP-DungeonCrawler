#pragma once

#include "IConfigParser.h"
#include <sstream>

class JsonParser : public IConfigParser {
public:
	ConfigData parse(const std::string& path) override;
	void serialize(const ConfigData& data, const std::string& path) override;

private:
	void skipWhitespace(const std::string& json, size_t& pos);
	std::string parseString(const std::string& json, size_t& pos);
	std::string parseValue(const std::string& json, size_t& pos, ValueType& type);
	void parseObject(const std::string& json, size_t& pos, ConfigData& data, const std::string& prefix);
	void parseArray(const std::string& json, size_t& pos, ConfigData& data, const std::string& prefix);
	void serializeSection(std::ostream& out, const Section& section, int indent, bool isRoot);
};
