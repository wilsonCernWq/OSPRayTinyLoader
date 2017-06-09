#pragma once
#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#include "common.h"
#include "helper.h"

namespace otv {

    class FrameBuffer {
    private:
	// window size
	ospcommon::vec2i size;
	// framebuffers
	uint32_t*          ofb;
	cyGLRenderBuffer2D gfb;	
	// ospray backend
	OSPFrameBuffer fb;
	OSPRenderer ren;
    public:
	FrameBuffer() = default;
	~FrameBuffer() { Clean(); };
	void Clean() {
	    ospUnmapFrameBuffer(ofb, fb);
	    ospFreeFrameBuffer(fb);
	    gfb.Delete();
	}
	void ClearFrameBuffer() {
	    ospFrameBufferClear(fb, OSP_FB_COLOR | OSP_FB_ACCUM);
	}
	OSPFrameBuffer& GetOSPFrameBuffer() { return fb; }
	void InitRenderer(OSPRenderer& renderer, ospcommon::vec2i winsize) {
	    ren = renderer;
	    size = winsize;	    
	    fb = ospNewFrameBuffer((osp::vec2i&)size, OSP_FB_SRGBA, 
					    OSP_FB_COLOR | OSP_FB_ACCUM);
	    ospFrameBufferClear(fb, OSP_FB_COLOR | OSP_FB_ACCUM);
	    ospRenderFrame(fb, ren, OSP_FB_COLOR | OSP_FB_ACCUM);
	    ofb = (uint32_t*)ospMapFrameBuffer(fb, OSP_FB_COLOR);
	    gfb.Initialize(true, 4, size.x, size.y);
	}
	void RenderOSPRay() {
	    ospRenderFrame(fb, ren, OSP_FB_COLOR | OSP_FB_ACCUM);
	}
	void RenderOpenGL() {
	    // render to ospray framebuffer
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

    };

};
#endif//_FRAMEBUFFER_H_
