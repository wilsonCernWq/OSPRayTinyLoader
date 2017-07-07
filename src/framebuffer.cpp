#include "framebuffer.h"

void ::otv::FrameBuffer::Clean() {
  if (fb != nullptr) {
    ospUnmapFrameBuffer(ofb, fb);
    ospFreeFrameBuffer(fb);
    if (!nowin) {
      gfb.Delete();
    }
  }
  fb = nullptr;
}

void ::otv::FrameBuffer::Resize(const vec2i& winsize) {
  Clean();
  fb = ospNewFrameBuffer((osp::vec2i&)size, OSP_FB_SRGBA, 
			 OSP_FB_COLOR | OSP_FB_ACCUM);
  ospFrameBufferClear(fb, OSP_FB_COLOR | OSP_FB_ACCUM);
  ofb = (uint32_t*)ospMapFrameBuffer(fb, OSP_FB_COLOR);
  if (!nowin) {
    gfb.Initialize(true, 4, size.x, size.y);
  }
}

void ::otv::FrameBuffer::Init(const vec2i& winsize, bool nwmode, OSPRenderer renderer) {
  nowin = nwmode;
  size = winsize;
  ren = renderer;
  Resize(winsize);
}
    
void ::otv::FrameBuffer::RenderOSPRay() {
  ospRenderFrame(fb, ren, OSP_FB_COLOR | OSP_FB_ACCUM);
}

void ::otv::FrameBuffer::RenderOpenGL() {
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
