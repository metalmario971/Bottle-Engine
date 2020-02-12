#include "../base/Gu.h"
#include "../base/GLContext.h"
#include "../gfx/BufferRenderTarget.h"


namespace BR2 {
///////////////////////////////////////////////////////////////////
BufferRenderTarget::~BufferRenderTarget() {
  glDeleteTextures(1, &_iGlTexId);
}
///////////////////////////////////////////////////////////////////
bool BufferRenderTarget::getMsaaEnabled() {
  if (_eTextureTarget == GL_TEXTURE_2D_MULTISAMPLE) {
    return true;
  }
  else if (_eTextureTarget == GL_TEXTURE_2D) {
    return false;
  }
  else {
    BroThrowNotImplementedException();
  }
}

void BufferRenderTarget::bind(GLenum eAttachment) {
  if (_eTargetType == RenderTargetType::e::Depth) {
    if (getMsaaEnabled()) {
      Gu::getContext()->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, _iGlTexId, 0);
    }
    else {
      Gu::getContext()->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _iGlTexId, 0);
    }
  }
  else {
    if (eAttachment == 0) {
      eAttachment = _eAttachment;
    }
    if (getMsaaEnabled()) {
      Gu::getContext()->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, eAttachment, GL_TEXTURE_2D_MULTISAMPLE, _iGlTexId, 0);
    }
    else {
      Gu::getContext()->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, eAttachment, GL_TEXTURE_2D, _iGlTexId, 0);
    }
  }
  Gu::checkErrorsDbg();
}






}//ns Game
