#include "framebuffer.h"

void ::otv::FrameBuffer::Clean() {
  if (fb != nullptr) {
    ospUnmapFrameBuffer(ofb, fb);
    ospFreeFrameBuffer(fb);
    fb = nullptr;
  }
}

void ::otv::FrameBuffer::Resize(const vec2i& winsize) {
  Clean();
  fb = ospNewFrameBuffer((osp::vec2i&)size, OSP_FB_SRGBA, OSP_FB_COLOR | OSP_FB_ACCUM);
  ospFrameBufferClear(fb, OSP_FB_COLOR | OSP_FB_ACCUM);
  ofb = (uint32_t*)ospMapFrameBuffer(fb, OSP_FB_COLOR);
}

void ::otv::FrameBuffer::Init(const vec2i& winsize, OSPRenderer renderer) {
  size = winsize;
  ren = renderer;
  Resize(winsize);
}
    
void ::otv::FrameBuffer::RenderOSPRay() {
  ospRenderFrame(fb, ren, OSP_FB_COLOR | OSP_FB_ACCUM);
}

