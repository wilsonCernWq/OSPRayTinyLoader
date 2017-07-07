#include "helper.h"

//! @name mouse2screen: convert mouse coordinate to [-1,1] * [-1,1]
void otv::mouse2screen
(int x, int y, float width, float height, cy::Point2f& p)
{
    p = cy::Point2f(2.0f * (float)x / width - 1.0f, 
		    1.0f - 2.0f * (float)y / height);
}

//! @name load file into string
std::string otv::loadfile
(const char *filename, std::ostream *outStream) {
    std::ifstream stream(filename, std::ios::in);
    if (!stream.is_open()) {
	if (outStream) { 
	    *outStream << "ERROR: Cannot open file '" 
		       << filename << "'"<< std::endl;
	}
	exit(EXIT_FAILURE);
    }
    std::string str = std::string((std::istreambuf_iterator<char>(stream)), 
				  std::istreambuf_iterator<char>());
    stream.close();
    return str;
}

//! @name copychar: copy string data to char pointer
void otv::copychar(char * &str, const std::string& src, int start)
{
    if (str) { delete[] str; }
    if (src == "") { str = nullptr; }
    else {
	size_t len = strlen(src.c_str());
	str = new char[len + 1]; str[len] = '\0';
	strncpy(str, src.c_str(), len);
    }
}

//! @name loadimg
void otv::loadimgActual
(ImageData& image, const char* filename, const std::string path) 
{
    std::string fnamestr(path + std::string(filename));
    std::size_t found = fnamestr.find_last_of('.');
    if (found == std::string::npos) { 
	std::cerr << "Error: unknown file format " << std::endl; return; 
    }
    // check image format
    std::string ext = fnamestr.substr(found + 1);
    if (ext == "png" || ext == "PNG") {
	unsigned error = 
	    lodepng::decode(image.data, image.width, image.height, 
			    fnamestr.c_str());
	if (error) { //if there's an error, display it
	    std::cout << "decoder error '" 
		      << fnamestr.c_str() << "' " 
		      << error << ": " 
		      << lodepng_error_text(error) << std::endl;
	}
    }
    else {
	std::cerr << "Error: unknown file format " << ext << std::endl;
    }
}
void otv::loadimg
(ImageData& image, const std::string filename, const std::string path)
{
    if (!filename.empty()) {
	loadimgActual(image, filename.c_str(), path);
    }
}
void otv::loadimg
(ImageData& image, const char* filename, const std::string path)
{
    if (filename) {
	loadimgActual(image, filename, path);
    }
}

//! @name writePPM Helper function to write the rendered image as PPM file
void otv::writePPM
(const char *fileName, const ospcommon::vec2i &size, const uint32_t *pixel) 
{
    using namespace ospcommon;
    FILE *file = fopen(fileName, "wb");
    fprintf(file, "P6\n%i %i\n255\n", size.x, size.y);
    unsigned char *out = (unsigned char *)alloca(3 * size.x);
    for (int y = 0; y < size.y; y++) {
	const unsigned char *in = 
	    (const unsigned char *)&pixel[(size.y - 1 - y)*size.x];
	for (int x = 0; x < size.x; x++) {
	    out[3 * x + 0] = in[4 * x + 0];
	    out[3 * x + 1] = in[4 * x + 1];
	    out[3 * x + 2] = in[4 * x + 2];
	}
	fwrite(out, 3 * size.x, sizeof(char), file);
    }
    fprintf(file, "\n");
    fclose(file);
}
