#pragma once
#ifndef _QUAD_H_
#define _QUAD_H_

#include "otv_common.h"
#include "otv_trackball.h"

namespace otv {

	class Quad {
	private:
		float vtptr[12] = {
			 1.0f, 1.0f,  0.0f,
			-1.0f, 1.0f,  0.0f,
			 1.0f, -1.0f,  0.0f,
			-1.0f, -1.0f, 0.0f
		};
		cy::GLSLProgram	shaders;
		std::string	vshader = "vs.quad.glsl" ;
		std::string	fshader = "fs.quad.glsl" ;
		cy::GLTexture2D texture;
		GLuint vao, vbo;
	public:

		virtual void Init()
		{
			// Compile shaders
			if (!shaders.BuildFiles(vshader.c_str(), fshader.c_str())) {
				std::cerr << "Failed to compile shaders" << std::endl;
				exit(EXIT_FAILURE);
			}
			shaders.Bind();
			// Vertex Array Object
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);
			// Vertex Buffer Objects
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, vtptr, GL_STATIC_DRAW);
			// Initialize framebuffer
			texture.Initialize();
			texture.SetFilteringMode(GL_LINEAR, GL_LINEAR);
		}

		virtual void Bind(GLuint width, GLuint height, uint32_t* data)
		{
			texture.SetImage(GL_RGBA4, GL_RGBA, (GLubyte*)data, width, height);
			shaders.Bind();
			glBindVertexArray(vao);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}

		virtual void Draw()
		{
			shaders.SetUniform("tex", 0); texture.Bind(0);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}

	};

};
#endif//_QUAD_H_
