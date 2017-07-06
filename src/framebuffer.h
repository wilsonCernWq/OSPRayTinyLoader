#pragma once
#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#include "common.h"
#include "helper.h"

namespace otv {

  class FrameBuffer {
  private:
    // flags and parameters
    bool nowin;
    vec2i size;
    // framebuffers
    OSPFrameBuffer     fb = nullptr;
    uint32_t*          ofb;
    cyGLRenderBuffer2D gfb;
    // reference to global renderer
    OSPRenderer ren;
  public:
    FrameBuffer() {}
    ~FrameBuffer() { Clean(); };
    void Clean() {
      if (fb != nullptr) {
	ospUnmapFrameBuffer(ofb, fb);
	ospFreeFrameBuffer(fb);
	if (!nowin) {
	  gfb.Delete();
	}
      }
      fb = nullptr;
    }

    OSPFrameBuffer& GetOSPFrameBuffer() { return fb; }
    void ClearFrameBuffer() {
      ospFrameBufferClear(fb, OSP_FB_COLOR | OSP_FB_ACCUM);
    }
    
    void Resize(const vec2i& winsize) { Init(winsize, nowin, ren); }
    void Init(const vec2i& winsize, bool nowin, OSPRenderer renderer) {
      Clean();
      size = winsize;
      ren = renderer;
      fb = ospNewFrameBuffer((osp::vec2i&)size, OSP_FB_SRGBA, 
			     OSP_FB_COLOR | OSP_FB_ACCUM);
      ospFrameBufferClear(fb, OSP_FB_COLOR | OSP_FB_ACCUM);
      ofb = (uint32_t*)ospMapFrameBuffer(fb, OSP_FB_COLOR);
      if (!nowin) {
	gfb.Initialize(true, 4, size.x, size.y);
      }
    }
    
    void RenderOSPRay() {
      ospRenderFrame(fb, ren, OSP_FB_COLOR | OSP_FB_ACCUM);
    }
    void RenderOpenGL() {
      if (!nowin) {
	RenderOSPRay();
	// put framebuffer to screen
	gfb.BindTexture();
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size.x, size.y, 
			GL_RGBA, GL_UNSIGNED_BYTE, ofb);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gfb.GetID());
	glBlitFramebuffer(0, 0, size.x, size.y, 
			  0, 0, size.x, size.y, 
			  GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glutSwapBuffers();
      }
    }    
  };
};

#endif//_FRAMEBUFFER_H_
