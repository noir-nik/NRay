#ifdef USE_MODULES
module;
#define _FILEMANAGER_EXPORT export
#else
#pragma once
#include <string>
#include <vector>
#define _FILEMANAGER_EXPORT
#endif

#ifdef USE_MODULES
export module FileManager;
import std;
#else
#pragma once
#include <string>
#include <vector>
#endif

class FileManager {
public:
    static std::vector<char> ReadRawBytes(std::string const& filename);
    static void ReadFloats(std::string const& filename, std::vector<float> &buffer);
    static int GetFileVersion(std::string const& filename);
	static void SaveBMP(char const* fname, unsigned const int* pixels, int w, int h);
};