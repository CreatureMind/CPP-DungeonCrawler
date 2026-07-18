#pragma once

#include <map>
#include <string>
#include <variant>
#include <vector>

enum class ValueType { String, Number, Boolean };

struct ConfigValue {
	std::string raw;
	ValueType type = ValueType::String;

	ConfigValue() = default;
	ConfigValue(const std::string& val, ValueType t = ValueType::String) : raw(val), type(t) {}

	bool asBool() const { return raw == "true" || raw == "1"; }
	double asNumber() const { return std::stod(raw); }
	const std::string& asString() const { return raw; }
};

using Section = std::map<std::string, ConfigValue>;
using ConfigData = std::map<std::string, Section>;
