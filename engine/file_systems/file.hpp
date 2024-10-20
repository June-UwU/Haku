#pragma once
#include "../types.hpp"
#include <vector>
#include <string>

std::vector<byte> read_binary(const std::string path);
std::string		  get_application_path();
std::string		  get_application_directory();