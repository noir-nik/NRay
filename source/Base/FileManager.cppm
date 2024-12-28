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
import stl;
#else
#pragma once
#include <string>
#include <vector>
#endif

class FileManager {
public:
    static std::vector<char> ReadRawBytes(const std::string& filename);
    static void ReadFloats(const std::string& filename, std::vector<float> &buffer);
    static int GetFileVersion(const std::string& filename);
	static void SaveBMP(const char* fname, const unsigned int* pixels, int w, int h);
};