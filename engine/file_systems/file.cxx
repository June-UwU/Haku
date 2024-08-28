#include "file.hpp"
#include "defines.hpp"
#include <fstream>
#include <filesystem>

std::vector<byte> read_binary(const std::string path) {
	TODO("file and folder verfication")
	TRACE << "opening file : " << path << "\n";
	std::ifstream file(path, std::ios::ate | std::ios::binary);

	if (false == file.is_open()) {
		WARN << path << " failed to open file. \n";
		return {};
	}

	u32 file_size = static_cast<u32>(file.tellg());

	std::vector<byte> binary_data(file_size);

	file.seekg(0);
	file.read(binary_data.data(), file_size);
	file.close();

	TRACE << "file opened with binary data : " << binary_data.size() << "\n";

	return binary_data;
}

std::string get_application_path() {
	return std::filesystem::current_path().string();
}

std::string get_application_directory() {
	return std::filesystem::current_path().remove_filename().string();
}