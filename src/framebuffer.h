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

    OSPFrameBuffer& GetOSPFrameBuffer() { return fb; }
    void ClearFrameBuffer() {
      ospFrameBufferClear(fb, OSP_FB_COLOR | OSP_FB_ACCUM);
    }
    
    void Resize(const vec2i& winsize);
    void Init(const vec2i& winsize, bool nowin, OSPRenderer renderer);
    
    void RenderOSPRay();
    void RenderOpenGL();
    void Clean();
  };
};

#endif//_FRAMEBUFFER_H_
