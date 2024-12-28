#ifdef USE_MODULES
module FileManager;
import Log;
import std;
#else
#include "Log.cppm"
#include "FileManager.cppm"
#include <filesystem>
#include <fstream>
#endif

std::vector<char> FileManager::ReadRawBytes(std::string const& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	if (!file.is_open()) {
		LOG_CRITICAL("Failed to open file: '{}'", filename);
	}
	std::size_t fileSize = (std::size_t)file.tellg();
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}

void FileManager::ReadFloats(std::string const& filename, std::vector<float> &buffer) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	if (!file.is_open()) {
		LOG_CRITICAL("Failed to open file: '{}'", filename);
	}
	std::size_t fileSize = (std::size_t)file.tellg();
	buffer.resize(fileSize / sizeof(float));
	file.seekg(0);
	file.read((char*)buffer.data(), fileSize);
	file.close();
}

/// @return Last write time of a file in seconds since epoch
int FileManager::GetFileVersion(std::string const& filename) {
	std::filesystem::path filePath(filename);
	if (!std::filesystem::exists(filePath)) {
		LOG_ERROR("File does not exist: '{}'", filename);
		return -1;
	}
	auto lastWriteTime = std::filesystem::last_write_time(filePath);
	auto duration = lastWriteTime.time_since_epoch();
	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();

	return static_cast<int>(seconds);
}

namespace { // BMP helper
struct Pixel3 { unsigned char r, g, b; };

void WriteBMP(char const* fname, Pixel3* a_pixelData, int width, int height)
{
  int paddedsize = (width*height) * sizeof(Pixel3);

  unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
  unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};

  bmpfileheader[ 2] = (unsigned char)(paddedsize    );
  bmpfileheader[ 3] = (unsigned char)(paddedsize>> 8);
  bmpfileheader[ 4] = (unsigned char)(paddedsize>>16);
  bmpfileheader[ 5] = (unsigned char)(paddedsize>>24);

  bmpinfoheader[ 4] = (unsigned char)(width    );
  bmpinfoheader[ 5] = (unsigned char)(width>> 8);
  bmpinfoheader[ 6] = (unsigned char)(width>>16);
  bmpinfoheader[ 7] = (unsigned char)(width>>24);
  bmpinfoheader[ 8] = (unsigned char)(height    );
  bmpinfoheader[ 9] = (unsigned char)(height>> 8);
  bmpinfoheader[10] = (unsigned char)(height>>16);
  bmpinfoheader[11] = (unsigned char)(height>>24);

  std::ofstream out(fname, std::ios::out | std::ios::binary);
  out.write((char const*)bmpfileheader, 14);
  out.write((char const*)bmpinfoheader, 40);
  out.write((char const*)a_pixelData, paddedsize);
  out.flush();
  out.close();
}
}
void FileManager::SaveBMP(char const* fname, unsigned const int* pixels, int w, int h)
{
  std::vector<Pixel3> pixels2(w*h);

  for (std::size_t i = 0; i < pixels2.size(); i++)
  {

    Pixel3 px;
    px.r       = (pixels[i] & 0x00FF0000) >> 16;
    px.g       = (pixels[i] & 0x0000FF00) >> 8;
    px.b       = (pixels[i] & 0x000000FF);
    pixels2[i] = px;
  }

  WriteBMP(fname, &pixels2[0], w, h);
}