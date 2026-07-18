#include "JsonParser.h"
#include <fstream>
#include <stdexcept>
#include <algorithm>

void JsonParser::skipWhitespace(const std::string& json, size_t& pos) {
	while (pos < json.size() && std::isspace(json[pos])) ++pos;
}

std::string JsonParser::parseString(const std::string& json, size_t& pos) {
	if (json[pos] != '"') throw std::runtime_error("Expected '\"'");
	++pos;
	std::string result;
	while (pos < json.size() && json[pos] != '"') {
		if (json[pos] == '\\' && pos + 1 < json.size()) {
			++pos;
			switch (json[pos]) {
				case 'n': result += '\n'; break;
				case 't': result += '\t'; break;
				case 'r': result += '\r'; break;
				case '\\': result += '\\'; break;
				case '"': result += '"'; break;
				default: result += json[pos];
			}
		} else {
			result += json[pos];
		}
		++pos;
	}
	if (pos >= json.size()) throw std::runtime_error("Unterminated string");
	++pos;
	return result;
}

std::string JsonParser::parseValue(const std::string& json, size_t& pos, ValueType& type) {
	skipWhitespace(json, pos);

	if (json[pos] == '"') {
		type = ValueType::String;
		return parseString(json, pos);
	}

	if (json.substr(pos, 4) == "true") {
		pos += 4;
		type = ValueType::Boolean;
		return "true";
	}
	if (json.substr(pos, 5) == "false") {
		pos += 5;
		type = ValueType::Boolean;
		return "false";
	}
	if (json.substr(pos, 4) == "null") {
		pos += 4;
		type = ValueType::String;
		return "";
	}

	std::string num;
	while (pos < json.size() && (std::isdigit(json[pos]) || json[pos] == '.' || json[pos] == '-' || json[pos] == '+' || json[pos] == 'e' || json[pos] == 'E')) {
		num += json[pos++];
	}
	type = ValueType::Number;
	return num;
}

void JsonParser::parseArray(const std::string& json, size_t& pos, ConfigData& data, const std::string& prefix) {
	++pos;
	skipWhitespace(json, pos);
	int index = 0;

	while (pos < json.size() && json[pos] != ']') {
		skipWhitespace(json, pos);
		std::string itemKey = prefix + "." + std::to_string(index);

		if (json[pos] == '{') {
			parseObject(json, pos, data, itemKey);
		} else if (json[pos] == '[') {
			parseArray(json, pos, data, itemKey);
		} else {
			ValueType type;
			std::string value = parseValue(json, pos, type);
			size_t dotPos = itemKey.find('.');
			if (dotPos != std::string::npos) {
				std::string section = itemKey.substr(0, dotPos);
				std::string key = itemKey.substr(dotPos + 1);
				data[section][key] = ConfigValue(value, type);
			}
		}

		skipWhitespace(json, pos);
		if (json[pos] == ',') ++pos;
		++index;
	}
	++pos;
}

void JsonParser::parseObject(const std::string& json, size_t& pos, ConfigData& data, const std::string& prefix) {
	++pos;
	skipWhitespace(json, pos);

	while (pos < json.size() && json[pos] != '}') {
		skipWhitespace(json, pos);
		std::string key = parseString(json, pos);
		skipWhitespace(json, pos);

		if (json[pos] != ':') throw std::runtime_error("Expected ':'");
		++pos;
		skipWhitespace(json, pos);

		std::string fullKey = prefix.empty() ? key : prefix + "." + key;

		if (json[pos] == '{') {
			parseObject(json, pos, data, fullKey);
		} else if (json[pos] == '[') {
			parseArray(json, pos, data, fullKey);
		} else {
			ValueType type;
			std::string value = parseValue(json, pos, type);

			size_t dotPos = fullKey.find('.');
			if (dotPos != std::string::npos) {
				std::string section = fullKey.substr(0, dotPos);
				std::string keyPart = fullKey.substr(dotPos + 1);
				data[section][keyPart] = ConfigValue(value, type);
			} else {
				data["global"][fullKey] = ConfigValue(value, type);
			}
		}

		skipWhitespace(json, pos);
		if (json[pos] == ',') ++pos;
	}
	++pos;
}

ConfigData JsonParser::parse(const std::string& path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		throw std::runtime_error("Cannot open file: " + path);
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string json = buffer.str();

	ConfigData data;
	size_t pos = 0;
	skipWhitespace(json, pos);

	if (json[pos] == '{') {
		parseObject(json, pos, data, "");
	} else {
		throw std::runtime_error("JSON must start with '{'");
	}

	return data;
}

void JsonParser::serializeSection(std::ostream& out, const Section& section, int indent, bool isRoot) {
	std::string ind(indent * 2, ' ');
	std::string ind2((indent + 1) * 2, ' ');

	std::map<std::string, Section> nested;
	std::vector<std::pair<std::string, ConfigValue>> direct;

	for (const auto& [key, value] : section) {
		size_t dotPos = key.find('.');
		if (dotPos != std::string::npos) {
			std::string prefix = key.substr(0, dotPos);
			std::string rest = key.substr(dotPos + 1);
			nested[prefix][rest] = value;
		} else {
			direct.emplace_back(key, value);
		}
	}

	out << "{\n";
	bool first = true;

	for (const auto& [key, value] : direct) {
		if (!first) out << ",\n";
		first = false;
		out << ind2 << "\"" << key << "\": ";
		if (value.type == ValueType::String) {
			out << "\"" << value.raw << "\"";
		} else {
			out << value.raw;
		}
	}

	for (const auto& [key, subsection] : nested) {
		if (!first) out << ",\n";
		first = false;

		bool isArray = !subsection.empty();
		for (const auto& [k, v] : subsection) {
			if (k.empty() || !std::isdigit(k[0])) {
				isArray = false;
				break;
			}
		}

		out << ind2 << "\"" << key << "\": ";
		if (isArray) {
			out << "[\n";
			bool firstItem = true;
			for (const auto& [idx, val] : subsection) {
				if (!firstItem) out << ",\n";
				firstItem = false;
				out << std::string((indent + 2) * 2, ' ');
				if (val.type == ValueType::String) {
					out << "\"" << val.raw << "\"";
				} else {
					out << val.raw;
				}
			}
			out << "\n" << ind2 << "]";
		} else {
			serializeSection(out, subsection, indent + 1, false);
		}
	}

	out << "\n" << ind << "}";
}

void JsonParser::serialize(const ConfigData& data, const std::string& path) {
	std::ofstream file(path);
	if (!file.is_open()) {
		throw std::runtime_error("Cannot write to file: " + path);
	}

	Section merged;
	for (const auto& [section, keys] : data) {
		for (const auto& [key, value] : keys) {
			std::string fullKey = (section == "global") ? key : section + "." + key;
			merged[fullKey] = value;
		}
	}

	serializeSection(file, merged, 0, true);
	file << "\n";
}
