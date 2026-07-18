#include "ParserFactory.h"
#include "IniParser.h"
#include "JsonParser.h"
#include <algorithm>
#include <stdexcept>

std::string getExtension(const std::string& path) {
	size_t dotPos = path.rfind('.');
	if (dotPos == std::string::npos) return "";
	std::string ext = path.substr(dotPos + 1);
	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
	return ext;
}

std::unique_ptr<IConfigParser> makeParser(const std::string& path) {
	std::string ext = getExtension(path);

	if (ext == "ini") {
		return std::make_unique<IniParser>();
	}
	if (ext == "json") {
		return std::make_unique<JsonParser>();
	}

	throw std::runtime_error("Unsupported format: " + ext);
}
