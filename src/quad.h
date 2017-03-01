#pragma once
#ifndef _QUAD_H_
#define _QUAD_H_

#include "otv_common.h"
#include "otv_trackball.h"

namespace otv {

	class Quad {
	private:
		cy::GLRenderBuffer2D texture;
		GLuint width, height;
	public:

		virtual void Init(GLuint width, GLuint height)
		{
			this->width = width;
			this->height = height;
			texture.Initialize(true, 4, width, height);
		}

		virtual void Draw(uint32_t* data)
		{
			texture.BindTexture();
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, texture.GetID());
			glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}

	};

};
#endif//_QUAD_H_
