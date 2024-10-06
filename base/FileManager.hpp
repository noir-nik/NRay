#pragma once

#include <string>
#include <vector>

class FileManager {
public:
    static std::vector<char> ReadRawBytes(const std::string& filename);
    static std::vector<float> ReadFloats(const std::string& filename);
    static int GetFileVersion(const std::string& filename);
	static void SaveBMP(const char* fname, const unsigned int* pixels, int w, int h);
};