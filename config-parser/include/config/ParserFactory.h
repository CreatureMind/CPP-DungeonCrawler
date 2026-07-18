#pragma once

#include "IConfigParser.h"
#include <memory>
#include <string>

std::string getExtension(const std::string& path);
std::unique_ptr<IConfigParser> makeParser(const std::string& path);
