#pragma once
#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#include "common/helper.h"

namespace otv {

  class FrameBuffer {
  private:
    // flags and parameters
    vec2i size;
    OSPRenderer ren; // reference to global renderer
    // framebuffers
    OSPFrameBuffer fb = nullptr;
    uint32_t*      ofb = nullptr;
  public:
    FrameBuffer() = default;
    ~FrameBuffer() { Clean(); };

    OSPFrameBuffer& GetOSPFrameBuffer() { return fb; }
    uint32_t*       GetMAPFrameBuffer() { return ofb; }
    
    void Resize(const vec2i& winsize);
    void Init(const vec2i& winsize, OSPRenderer renderer);
    void RenderOSPRay();
    void Clean();
    void Clear() { ospFrameBufferClear(fb, OSP_FB_COLOR | OSP_FB_ACCUM); }
  };
};

#endif//_FRAMEBUFFER_H_
