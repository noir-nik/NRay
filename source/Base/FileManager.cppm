export module FileManager;
import std;

class FileManager {
public:
    static std::vector<char> ReadRawBytes(std::string const& filename);
    static void ReadFloats(std::string const& filename, std::vector<float> &buffer);
    static int GetFileVersion(std::string const& filename);
	static void SaveBMP(char const* fname, unsigned const int* pixels, int w, int h);
};