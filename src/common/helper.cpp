#include "helper.h"
#include <set>

#ifdef USE_IMAGEMAGICK
# define MAGICKCORE_QUANTUM_DEPTH 16
# define MAGICKCORE_HDRI_ENABLE 0
# include <Magick++.h>
#endif

namespace otv {
  void loadimgActual
  (ImageData& image, const char* filename, const std::string path);
};

void otv::WarnAlways(std::string str)
{
  std::cerr << "\033[1;33m" << "[Warning] " << str << "\033[0m"
	    << std::endl << std::endl;   
}
void otv::WarnOnce(std::string str)
{
  static std::set<std::string> warned;
  if (warned.find(str) == warned.end()) {
    otv::WarnAlways(str);
    warned.insert(str);
  }
}
void otv::ErrorNoExit(std::string str)
{
  std::cerr << "\033[1;31m" << "[Error] " << str << "\033[0m"
	    << std::endl << std::endl;
}
void otv::ErrorFatal(std::string str)
{
  otv::ErrorNoExit(str);
  exit(EXIT_FAILURE); 
}

//! @name CreateOSPTex
OSPTexture2D
otv::ImageData::CreateOSPTex()
{
  OSPTexture2D osptex =
    ospNewTexture2D(osp::vec2i{(int)width, (int)height}, 
		    channel == 4 ? OSP_TEXTURE_RGBA8 : OSP_TEXTURE_RGB8,
		    data.data(),
		    OSP_DATA_SHARED_BUFFER);      
  return osptex;
}

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
    image.channel = 4;
    if (error) { //if there's an error, display it
      std::cout << "decoder error '" 
		<< fnamestr.c_str() << "' " 
		<< error << ": " 
		<< lodepng_error_text(error) << std::endl;
    }
  }
  else {
#ifdef USE_IMAGEMAGICK
    std::cout << "[loader] Using ImageMagick++" << std::endl;
    // load texture
    std::cout << "[loader] Opening texture " << fnamestr << std::endl;
    Magick::Image magtex(fnamestr.c_str());
    // get basic image information
    std::cout << "[loader] Getting texture information" << std::endl;
    const bool hdr = magtex.depth() > 8;
    //const float rcpMaxRGB = 1.0f/float(MaxRGB);
    image.width   = magtex.columns();
    image.height  = magtex.rows();
    image.depth   = hdr ? 4 : 1;
    image.channel = magtex.matte() ? 4 : 3;
    std::cout << "[loader] Width  : " << image.width   << std::endl;
    std::cout << "[loader] Height : " << image.height  << std::endl;
    std::cout << "[loader] Depth  : " << image.depth   << std::endl;
    std::cout << "[loader] Channel: " << image.channel << std::endl;
    // read pixels
    std::cout << "[loader] Reading pixels" << std::endl;
    const Magick::PixelPacket* pixels =
      magtex.getConstPixels(0,0,image.width,image.height);    
    if (!pixels) {
      std::cerr << "Error: failed to load texture '" + fnamestr + "'"
		<< std::endl;
    } else {
      image.data = std::vector<unsigned char>
	(image.width*image.height*image.depth*image.channel, 0);
      // convert pixels and flip image
      // OSPRay's textures have the origin at the lower left corner
      if (hdr) {
	std::cout << "[loader] HDIR Image" << std::endl;
      }	   
      for (size_t y = 0; y < image.height; y++) {
	for (size_t x = 0; x < image.width; x++) {
	  const Magick::PixelPacket &pixel = pixels[y*image.width+x];
	  if (hdr) {
	    // float *dst =
	    //   &((float*)image.data.data())
	    //   [(x+(image.height-1-y)*image.width)*image.channel];
	    // *dst++ = pixel.red   * rcpMaxRGB;
	    // *dst++ = pixel.green * rcpMaxRGB;
	    // *dst++ = pixel.blue  * rcpMaxRGB;
	    // if (image.channel == 4) {
	    //   *dst++ = pixel.opacity * rcpMaxRGB;
	    // }
	  } else {
	    unsigned char *dst =
	      &((unsigned char*)image.data.data())
	      [(x+(image.height-1-y)*image.width)*image.channel];
	    *dst++ = pixel.red;
	    *dst++ = pixel.green;
	    *dst++ = pixel.blue;
	    if (image.channel == 4) {
	      *dst++ = pixel.opacity;
	    }
	  }
	}
      }
    }
    std::cout << "[loader] Done ImageMagick++" << std::endl;
#else
    std::cerr << "Error: unknown file format " << ext << std::endl;
#endif
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
