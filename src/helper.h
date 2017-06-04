/**
 * This file defines all helper functions.
 * Those functions shouldn't depend on any thing in this project
 */
#pragma once
#ifndef _HELPER_H_
#define _HELPER_H_

#include "common.h"

//! @name error check helper from EPFL ICG class
static inline const char* ErrorString(GLenum error) {
    const char* msg;
    switch (error) {
#define Case(Token)  case Token: msg = #Token; break;
	Case(GL_INVALID_ENUM);
	Case(GL_INVALID_VALUE);
	Case(GL_INVALID_OPERATION);
	Case(GL_INVALID_FRAMEBUFFER_OPERATION);
	Case(GL_NO_ERROR);
	Case(GL_OUT_OF_MEMORY);
#undef Case
    }
    return msg;
}

//! @name check error
static inline void _glCheckError(const char* file, int line, const char* comment) {
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
	fprintf(stderr, "ERROR: %s (file %s, line %i: %s).\n", comment, file, line, ErrorString(error));
    }
}

#ifndef NDEBUG
# define check_error_gl(x) _glCheckError(FILE, LINE, x)
#else
# define check_error_gl() ((void)0)
#endif

#ifndef NDEBUG
#define DEBUG_VECTOR(n, t)\
static void debug(const cy::Point##n##t& m) {   	     \
    std::cout << std::endl;                                  \
    for (int i = 0; i < n; ++i) {			     \
        std::cout << "\t" << m[i];                           \
    }                                                        \
    std::cout << std::endl;				     \
}
#define DEBUG_MATRIX(n, t)\
static void debug(const cy::Matrix##n##t& m) {   	     \
    std::cout << std::endl;                                  \
    for (int i = 0; i < n; ++i) {			     \
    for (int j = 0; j < n; ++j) {			     \
        std::cout << "\t" << m(i, j);			     \
    }                                                        \
    std::cout << std::endl;				     \
    }                                                        \
    std::cout << std::endl;				     \
}
#else
#define DEBUG_VECTOR (n, t) static void debug(const cy::Point##n##t& m) {}
#define DEBUG_MATRIX (n, t) static void debug(const cy::Matrix##n##t& m) {}
#endif                                                     

namespace cy {
    typedef Point2<int> Point2i;
    typedef Point3<int> Point3i;
    typedef Point4<int> Point4i;
    typedef Matrix2<int> Matrix2i;
    typedef Matrix3<int> Matrix3i;
    typedef Matrix4<int> Matrix4i;

};

typedef cy::Point2<int> cyPoint2i;
typedef cy::Point3<int> cyPoint3i;
typedef cy::Point4<int> cyPoint4i;
typedef cy::Matrix2<int> cyMatrix2i;
typedef cy::Matrix3<int> cyMatrix3i;
typedef cy::Matrix4<int> cyMatrix4i;

namespace otv 
{
    DEBUG_VECTOR(2,f);
    DEBUG_VECTOR(3,f);
    DEBUG_VECTOR(4,f);
    DEBUG_VECTOR(2,i);
    DEBUG_VECTOR(3,i);
    DEBUG_VECTOR(4,i);

    DEBUG_MATRIX(2,f);
    DEBUG_MATRIX(3,f);
    DEBUG_MATRIX(4,f);
    DEBUG_MATRIX(2,i);
    DEBUG_MATRIX(3,i);
    DEBUG_MATRIX(4,i);
};

#define CAST_VECTOR(n, t)\
static ospcommon::vec##n##t make_vec(const cy::Point##n##t& m) { \
    ospcommon::vec##n##t v;					 \
    for (int i = 0; i < n; ++i) {			         \
        v[i] = m[i];                                             \
    }                                                            \
    return v;							 \
}

namespace otv 
{
    CAST_VECTOR(2,i);
    CAST_VECTOR(3,i);
    CAST_VECTOR(4,i);
    CAST_VECTOR(2,f);
    CAST_VECTOR(3,f);
    CAST_VECTOR(4,f);
};

namespace otv 
{
    struct ImageData {
	std::vector<unsigned char> data;
	unsigned int width = 0, height = 0;
	bool IsEmpty() { return (width * height <= 0); }
	cy::Point2<int> Size() { return cy::Point2<int>(width, height); }
    };

    //! @name mouse2screen: convert mouse coordinate to [-1,1] * [-1,1]
    void mouse2screen(int x, int y, float width, float height, cy::Point2f& p);

    //! @name load file into string
    std::string loadfile
	(const char *filename, std::ostream *outStream = &std::cout);

    //! @name copychar: copy string data to char pointer
    void copychar(char * &str, const std::string& src, int start = 0);

    //! @name loadimg
    void loadimgActual
	(ImageData& image, const char* filename, const std::string path);

    void loadimg
	(ImageData& image, const std::string filename, const std::string path);

    void loadimg
	(ImageData& image, const char* filename, const std::string path);

    //! @name writePPM Helper function to write the rendered image as PPM file
    void writePPM
	(const char *fileName, 
	 const ospcommon::vec2i &size, 
	 const uint32_t *pixel);

};

#endif//_HELPER_H_
