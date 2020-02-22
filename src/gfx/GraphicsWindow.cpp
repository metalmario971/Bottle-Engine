#include "../base/Gu.h"
#include "../base/SDLUtils.h"
#include "../base/Delta.h"
#include "../base/FrameSync.h"
#include "../base/Perf.h"
#include "../base/InputManager.h"
#include "../base/FrameSync.h"
#include "../base/Logger.h"
#include "../base/EngineConfig.h"
#include "../base/ApplicationPackage.h"
#include "../gfx/RenderViewport.h"
#include "../gfx/ShaderManager.h"
#include "../gfx/Picker.h"
#include "../gfx/ParticleMaker.h"
#include "../gfx/LightManager.h"
#include "../gfx/TexCache.h"
#include "../gfx/GraphicsApi.h"
#include "../gfx/RenderPipeline.h"
#include "../gfx/UiControls.h"
#include "../gfx/GraphicsApi.h"
#include "../gfx/VulkanApi.h"
#include "../gfx/UiControls.h"
#include "../gfx/CameraNode.h"
#include "../gfx/RenderViewport.h"
#include "../gfx/GraphicsWindow.h"
#include "../gfx/GLContext.h"
#include "../model/ModelCache.h"
#include "../world/Scene.h"

namespace BR2 {

//Called exclusively by the graphics API
GraphicsWindow::GraphicsWindow(bool ismain, string_t title, RenderSystem::e sys) {
  _bIsMainWindow = ismain;
  _title = title;
}
GraphicsWindow::~GraphicsWindow() {
  if (_pSDLWindow != nullptr) {
    SDL_DestroyWindow(_pSDLWindow);
    _pSDLWindow = nullptr;
  }
}
void GraphicsWindow::init() {
  createManagers();
}
void GraphicsWindow::idle(int64_t us) {
  if (_pScene) {
    _pScene->idle(us);
  }
}
int32_t GraphicsWindow::getWidth() {
  return _iLastWidth;
}
int32_t GraphicsWindow::getHeight() {
  return _iLastHeight;
}
void GraphicsWindow::createManagers() {
  //BroLogInfo("Creating Graphics API.");
  //if (Gu::getEngineConfig()->getRenderSystem() == RenderSystem::OpenGL) {
  //  _pGraphicsApi = std::make_shared<OpenGLApi>();
  //}
  //else if (Gu::getEngineConfig()->getRenderSystem() == RenderSystem::Vulkan) {
  //  _pGraphicsApi = std::make_shared<VulkanApi>();
  //}
  //else {
  //  BroThrowException("Invalid render engine.");
  //}

  Br2LogInfo("Creating SDL Window");
  createSDL_OpenGLWindow(_title);

  Br2LogInfo("Creating Renderer.");
  initRenderSystem();

}
void GraphicsWindow::createSDL_OpenGLWindow(string_t windowTitle) {

  int minGLVersion;
  int minGLSubversion;
  const int c_iMax_Profs = 2;
  GLProfile profs[c_iMax_Profs];
  int iProfile = SDL_GL_CONTEXT_PROFILE_CORE;
  bool bVsync = false;

#ifdef BRO_OS_IPHONE
  minGLVersion = 3;
  minGLSubversion = 0;
  iProfile = SDL_GL_CONTEXT_PROFILE_ES;
#else
#ifdef BR2_OS_WINDOWS
  minGLVersion = 3;
  minGLSubversion = 3;
  iProfile = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY;
#endif
#endif

  // if (getMainWindow() == nullptr) {
  profs[0].make(32, minGLVersion, minGLSubversion, iProfile, bVsync);
  profs[1].make(24, minGLVersion, minGLSubversion, iProfile, bVsync);

  for (int iProf = 0; iProf < c_iMax_Profs; ++iProf) {
    //This must be called before creating the window because this sets SDL's PixelFormatDescritpro
    GLContext::setWindowAndOpenGLFlags(profs[iProf]);

    makeSDLWindow(windowTitle, SDL_WINDOW_OPENGL);

    _pContext = std::make_shared<GLContext>();
    if (_pContext->create(getThis<GraphicsWindow>(), profs[iProf]) == false) {
      _pContext = nullptr;
    }

    //  //Unfortunately because SDL needs a window, we need to wait to create the context.
    //  if (getContext() == nullptr) {
    //    if (std::dynamic_pointer_cast<OpenGLApi>(getGraphicsApi())->makeContext(getThis<GraphicsWindow>(), profs[iProf]) != nullptr) {
    //      //Couldn't make context, try again.
    //      break;
    //    }
    //  }
  }

  _pFrameSync = std::make_shared<FrameSync>(_pContext);
}
void GraphicsWindow::makeCurrent() {
  SDL_GL_MakeCurrent(getSDLWindow(), getContext()->getSDLGLContext());
}
void GraphicsWindow::getDrawableSize(int* w, int* h) {
  SDL_GL_GetDrawableSize(getSDLWindow(), w, h);
}
void GraphicsWindow::swapBuffers() {
  SDL_GL_SwapWindow(getSDLWindow());
}
void GraphicsWindow::makeSDLWindow(string_t windowTitle, int render_system) {
  string_t title;
  bool bFullscreen = false;

  int x, y, w, h, flags;
#ifdef BRO_OS_IPHONE
  x = 0, y = 0, w = 320, h = 480, flags = SDL_WINDOW_BORDERLESS | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL;
  title = "";
#else
#ifdef BR2_OS_WINDOWS

  //SDL_WINDOW_OPENGL | SDL_WINDOW_VULKAN;

  if (bFullscreen) {
    x = 0; y = 0;
    w = 1920; h = 1080;
    flags = render_system;
  }
  else {
    x = 100, y = 100, w = 800, h = 600, flags = SDL_WINDOW_SHOWN | render_system | SDL_WINDOW_RESIZABLE;
  }
  title = windowTitle;
#endif
#endif

  //No0te: This calls SDL_GL_LOADLIBRARY if SDL_WINDOW_OPENGL is set.
  _pSDLWindow = SDL_CreateWindow(title.c_str(), x, y, w, h, flags);
  SDLUtils::checkSDLErr();

  //Fullscreen nonsense
  if (bFullscreen) {
    SDL_SetWindowFullscreen(_pSDLWindow, SDL_WINDOW_FULLSCREEN);
  }
  SDLUtils::checkSDLErr();

  //Note we may need to adjust for actual start width/height if start width is too large , etc.
  //*Set room width / height
  _iLastWidth = Gu::getConfig()->getDefaultScreenWidth();
  _iLastHeight = Gu::getConfig()->getDefaultScreenHeight();

#ifdef BR2_OS_WINDOWS
  Br2LogError("We are not making the window icon because there's an error somewhere in SDL here.");
  //**There is an error here
//  Gu::SDLTrySetWindowIcon(_pWindow, "./data-dc/tex/tx64-icon.png");//_pApp->getIconFullPath());
#endif
}
void GraphicsWindow::initRenderSystem() {
  if (_pSDLWindow == nullptr) {
    Br2ThrowException("You need to make the SDL window before initializing render system.");
  }

  SDLUtils::trySetWindowIcon(_pSDLWindow, Gu::getAppPackage()->getIconPath());

  if (Gu::getConfig()->getForceAspectRatio()) {
    SDL_DisplayMode dm;
    if (SDL_GetDesktopDisplayMode(0, &dm) != 0) {
      Br2LogError("SDL_GetDesktopDisplayMode failed: " + SDL_GetError());
    }
    else {
      float ar = (float)dm.h / (float)dm.w;
      int newHeight = (int)((float)_iLastWidth * ar);
      if (newHeight != _iLastHeight) {
        Br2LogInfo("Adjusting window dims from " + _iLastWidth + " x " + _iLastHeight + " to " + _iLastWidth + " x " + newHeight);
        _iLastHeight = newHeight;
      }
    }
  }

  Br2LogInfo("Setting window size to, " + _iLastWidth + " x " + _iLastHeight);
  SDL_SetWindowSize(_pSDLWindow, _iLastWidth, _iLastHeight);
  updateWidthHeight(_iLastWidth, _iLastHeight, true);

  if (Gu::getConfig()->getStartFullscreen() == true) {
    Br2LogInfo("Setting window fullscreen.");
    toggleFullscreen();
  }

  createRenderPipe();
}
void GraphicsWindow::updateWidthHeight(uint32_t w, uint32_t h, bool bForce) {
  //update view/cam
  if ((_iLastWidth != w) || (_iLastHeight != h) || bForce) {

    for (auto cam : getScene()->getAllCameras()) {
      cam->getViewport()->updateBox(getThis<GraphicsWindow>());
    }

    if (_pRenderPipe != nullptr) {
      _pRenderPipe->resizeScreenBuffers((int32_t)w, (int32_t)h);
    }

    //   _pApp->screenChanged(w, h, _bFullscreen);

    if (_pScene) {
      _pScene->updateWidthHeight(w, h, bForce);
    }

    _iLastWidth = w;
    _iLastHeight = h;
  }

}
void GraphicsWindow::toggleFullscreen() {
  if (_bFullscreen == false) {
    //get the fullscreen resolution
    int32_t iFsW = Gu::getEngineConfig()->getFullscreenWidth();
    int32_t iFsH = Gu::getEngineConfig()->getFullscreenHeight();
    if (iFsW <= 0 || iFsH <= 0) {
      SDL_DisplayMode DM;
      SDL_GetCurrentDisplayMode(0, &DM);
      iFsW = DM.w;
      iFsH = DM.h;
    }
    //Save pre-fullscreen width/height
    _iFullscreenToggleWidth = _iLastWidth;
    _iFullscreenToggleHeight = _iLastHeight;
    SDL_SetWindowSize(_pSDLWindow, iFsW, iFsH);

    if (SDL_SetWindowFullscreen(_pSDLWindow, SDL_WINDOW_FULLSCREEN) != 0) {
      Br2LogError("Failed to go fullscreen.");
    }
    else {
      _bFullscreen = true;
      //_pApp->screenChanged(iFsW, iFsH, _bFullscreen);
    }
  }
  else {
    if (_iFullscreenToggleWidth > 0 && _iFullscreenToggleHeight > 0) {
      //Restore pre-fullscreen width/height
      SDL_SetWindowSize(_pSDLWindow, _iFullscreenToggleWidth, _iFullscreenToggleHeight);
    }
    if (SDL_SetWindowFullscreen(_pSDLWindow, 0) != 0) {
      Br2LogError("Failed to exit fullscreen.");
    }
    else {
      _bFullscreen = false;
      //_pApp->screenChanged(_iLastWidth, _iLastHeight, _bFullscreen);
    }
  }

}
void GraphicsWindow::printHelpfulDebug() {
  int dw, dh;
  SDL_DisplayMode mode;
  SDL_Window* win = _pSDLWindow;

  SDL_GetCurrentDisplayMode(0, &mode);
  Br2LogInfo("Screen BPP    : " + SDL_BITSPERPIXEL(mode.format));
  Br2LogInfo("Swap Interval : " + SDL_GL_GetSwapInterval());
  SDL_GetWindowSize(win, &dw, &dh);
  Br2LogInfo("Initial Window Size   : " + dw + "x" + dh);
  SDL_GL_GetDrawableSize(win, &dw, &dh);
  Br2LogInfo("Draw Size     : " + dw + "x" + dh);

  SDLUtils::checkSDLErr();
}
void GraphicsWindow::createRenderPipe() {
  //Deferred Renderer
  _pRenderPipe = std::make_shared<RenderPipeline>(getThis<GraphicsWindow>());
  _pRenderPipe->init(getWidth(), getHeight(), Gu::getAppPackage()->getEnvTextureFolder());
}
void GraphicsWindow::beginRender() {

  //Make this window current *critical*
  //OPTIMIZE:TODO:NOTE: if there is only 1 window we don't have to call this.
  makeCurrent();

  //Update the widnow size
  int w, h;
  getDrawableSize(&w, &h);
  updateWidthHeight(w, h, false);

  Perf::pushPerf();
}
void GraphicsWindow::endRender() {
  swapBuffers();

  Perf::popPerf();
}
void GraphicsWindow::step() {
  //Managers
  _pContext->updateThisContext();

  beginRender();
  {
    if (Gu::getInputManager()->keyPress(SDL_SCANCODE_F11)) {
      toggleFullscreen();
    }

    getContext()->setLoopState(EngineLoopState::SyncBegin);
    getFrameSync()->syncBegin();
    {
      getContext()->setLoopState(EngineLoopState::Update);
      _pScene->update((float)_pContext->getDelta()->get());

      getContext()->setLoopState(EngineLoopState::Render);

      //Main Render
      if (_pScene) {
        PipeBits pipebits;
        pipebits.set();
        _pRenderPipe->renderScene(_pScene, pipebits);
      }
    }
    getContext()->setLoopState(EngineLoopState::SyncEnd);
    getFrameSync()->syncEnd();
  }

  endRender();

  //Do not remove
  getContext()->chkErrRt();

}


}//ns BR2
