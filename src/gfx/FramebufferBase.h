/**
*  @file FramebufferBase.h
*  @date May 8, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __FRAMEBUFFERBASE_14942908062084042827_H__
#define __FRAMEBUFFERBASE_14942908062084042827_H__
#include "../gfx/GfxHeader.h"

namespace BR2 {
/**
*  @class FramebufferBase
*  @brief Base for all framebuffers
*/
class FramebufferBase : public VirtualMemory {
public:
  FramebufferBase(std::shared_ptr<GLContext> pc, bool bMsaa, int nMsaa, vec4& vClear);
  virtual ~FramebufferBase()  override;

  virtual void init(int32_t iWidth, int32_t iHeight, std::shared_ptr<RenderTarget> sharedDepth, std::shared_ptr<RenderTarget> sharedPick) = 0;
  //virtual void resizeScreenBuffers(int iWidth, int iHeight, std::shared_ptr<RenderTarget> pShared);
  virtual void beginRender() = 0;
  virtual void endRender() = 0;

  static std::shared_ptr<RenderTarget> createTarget(t_string strName, GLenum texFormat, GLenum internalFormat,
    GLenum dataType, int32_t w, int32_t h, RenderTargetType::e eTargetType,
    int32_t iIndex, bool bMsaaEnabled, int32_t nMsaaSamples);
  GLuint getGlId() { return _uiGlFramebufferId; }
  static std::shared_ptr<RenderTarget> createDepthTarget(t_string strName, int32_t w, int32_t h, int iIndex, bool bMsaaEnabled, int32_t nMsaaSamples);
  std::vector<std::shared_ptr<RenderTarget>>& getTargets() { return _vecTargets; }
  GLuint getFramebufferId() { return _uiGlFramebufferId; }

  std::shared_ptr<RenderTarget> getTargetByName(std::string name);
  void setClear(vec4& v) { _vClear = v; }
  const vec4& getClear() { return _vClear; }
protected:
    std::shared_ptr<GLContext> _pContext = nullptr;
    bool _bMsaaEnabled = false;
    uint32_t _nMsaaSamples = 0;
    GLuint _uiGlFramebufferId = 0;
    std::vector<std::shared_ptr<RenderTarget>> _vecTargets;
    FramebufferState::e _eState = FramebufferState::e::Not_Initialized;
    vec4 _vClear;

    void attachAllTargets();
    void setDrawAllTargets();
    void checkFramebufferComplete();
    void addTarget(t_string strName, GLenum internalFormat, GLenum texFormat, GLenum dataType, int32_t w, int32_t h, RenderTargetType::e eTargetType);
    void addTarget(std::shared_ptr<RenderTarget> other);//Copy target from other framebuffer
   // void attachDepthTarget(std::shared_ptr<RenderTarget> pSharedDepth);
    void deleteTargets();
protected:
    static void makeRenderTexture(GLuint* iTexId, GLenum eAttachment, GLenum eInternalFormat, GLenum eTextureFormat, 
        GLenum eDataType, int32_t iWidth, int32_t iHeight, GLenum* eOutTarget, bool bMultisample, int32_t nSamples);

};





}//ns Game



#endif
