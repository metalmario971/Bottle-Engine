#include "../base/Perf.h"
#include "../base/Gu.h"
#include "../base/GLContext.h"
#include "../base/Logger.h"
#include "../base/GraphicsWindow.h"
#include "../base/WindowManager.h"
#include "../base/ApplicationPackage.h"
#include "../base/FileSystem.h"
#include "../base/ScriptManager.h"
#include "../base/FpsMeter.h"
#include "../base/FrameSync.h"
#include "../base/InputManager.h"

#include "../gfx/ShaderManager.h"
#include "../gfx/ShadowBox.h"
#include "../gfx/LightNode.h"
#include "../gfx/RenderViewport.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/TexCache.h"
#include "../gfx/Texture2DSpec.h"
#include "../gfx/CameraNode.h"
#include "../gfx/RenderUtils.h"
#include "../gfx/UiControls.h"
#include "../gfx/GraphicsApi.h"
#include "../gfx/FlyingCameraControls.h"
#include "../gfx/MegaTex.h"
#include "../model/MeshUtils.h"
#include "../model/Model.h"
#include "../model/MeshNode.h"
#include "../world/RenderBucket.h"
#include "../world/PhysicsWorld.h"
#include "../world/Scene.h"


namespace BR2 {
Scene::Scene() : SceneNode(nullptr) {
  //_pFlyCam = std::make_shared<FlyingCameraControls>(Gu::getViewport());

  //_pFlyCam->getCam()->setLookAt(vec3(0, 0, 0));
  //_pFlyCam->getCam()->setPos(vec3(0, 0, -10));


  BRThrowNotImplementedException();
  //BRLogInfo("Creating Window UI");
  //_pScreen = std::make_shared<UiScreen>(getThis<GraphicsWindow>());

  BRLogInfo("GLContext -  Lights");
  _pLightManager = std::make_shared<LightManager>();

  BRLogInfo("GLContext -  ScriptManager");
  _pScriptManager = std::make_shared<ScriptManager>();

  createFlyingCamera();
  _pRenderBucket = std::make_shared<RenderBucket>();
}
Scene::~Scene() {
  BRThrowNotImplementedException();
  //_pScreen = nullptr;
  _pLightManager = nullptr;
  _pScriptManager = nullptr;
}
void Scene::update(float delta) {

}
void Scene::idle(int64_t us) {
}
std::shared_ptr<GLContext> Scene::getContext() { 
  return getWindow()->getContext(); 
}
std::vector<std::shared_ptr<CameraNode>> Scene::getAllCameras() {
  std::vector<std::shared_ptr<CameraNode>> ret;
  iterateBreadthFirst<CameraNode>([&ret](std::shared_ptr<CameraNode> cam) {
    ret.push_back(cam);
    return true;
  });
  return ret;
}
void Scene::createFlyingCamera() {
  //In the future we will replace this witht he active object.
  BRLogInfo("Creating Flying Camera");

#define NOSCRIPT

#ifdef NOSCRIPT
  BRLogInfo("Creating Fly Camera.");
  std::shared_ptr<CameraNode> cn = std::make_shared<CameraNode>();
  std::shared_ptr<FlyingCameraControls> css = std::make_shared<FlyingCameraControls>();
  cn->addComponent(css);
  attachChild(cn);
  // cn->init();

  //cn->getFrustum()->setZFar(1000.0f); //We need a SUPER long zFar in order to zoom up to the tiles.  
  //updateCameraPosition();
  //_vMoveVel.construct(0, 0, 0);
  //_pCamera->setPos(vec3(30, 30, 30));
  //_pCamera->update(0.0f, std::map<Hash32, std::shared_ptr<Animator>>());//Make sure to create the frustum.
  //_vCamNormal = _pCamera->getViewNormal();
  //_vCamPos = _pCamera->getPos();
#else
  //Sort of also a scripting test.
  std::shared_ptr<CameraNode> cam = CameraNode::create(getWindow()->getViewport(), getThis<Scene>());
  std::shared_ptr<CSharpScript> getScriptManager()->loadScript(FileSystem::combinePath(Gu::getAppPackage()->getScriptsFolder(), "FlyCamera.cs"));
  cam->addComponent(script);
#endif

}
void Scene::debugChangeRenderState() {
  if (Gu::getInputManager()->keyPress(SDL_SCANCODE_F1)) {
    if (Gu::getInputManager()->shiftHeld()) {
      _bDebugDisableShadows = !_bDebugDisableShadows;
    }
    else {
      _bShowDebugText = !_bShowDebugText;
    }
  }
  if (Gu::getInputManager()->keyPress(SDL_SCANCODE_F2)) {
    _bDrawDebug = !_bDrawDebug;
  }
  if (Gu::getInputManager()->keyPress(SDL_SCANCODE_F3)) {
    _bDebugDisableCull = !_bDebugDisableCull;
  }
  if (Gu::getInputManager()->keyPress(SDL_SCANCODE_F4)) {
    _bDebugShowWireframe = !_bDebugShowWireframe;
  }
  if (Gu::getInputManager()->keyPress(SDL_SCANCODE_F5)) {
    _bDebugClearWhite = !_bDebugClearWhite;
  }
  if (Gu::getInputManager()->keyPress(SDL_SCANCODE_F6)) {
    _bDebugDisableDepthTest = !_bDebugDisableDepthTest;
  }
  if (Gu::getInputManager()->keyPress(SDL_SCANCODE_F7)) {
    //if (Gu::getFrameSync()->isEnabled()) {
    //  Gu::getFrameSync()->disable();
    //}
    //else {
    //  Gu::getFrameSync()->enable();
    //}
  }
  //if (Gu::getFingers()->keyPressOrDown(SDL_SCANCODE_F10)) {
  //    if (_iF10Pressed == 0) {
  //        _iF10Pressed = Gu::getMicroSeconds();
  //    }
  //    if (Gu::getMicroSeconds() - _iF10Pressed > 1000000) {
  //        _pWorld25->getConfig()->setRenderBlocks(!_pWorld25->getConfig()->getRenderBlocks());
  //        BroLogInfo("Block Mode turned " + (_pWorld25->getConfig()->getRenderBlocks() ? "On" : "Off"));
  //        BroLogInfo("Remaking all grid meshes... be patient.");
  //        _pWorld25->remakeGridMeshes();
  //        _iF10Pressed = 0;
  //    }
  //}
  //else {
  //    _iF10Pressed = 0;
  //}



#ifdef BR2_OS_WINDOWS
    //#ifdef _DEBUG
    //These must be #ifdef out because glPolygonMOde is not present in gl330 core 
  if (_bDebugShowWireframe == true) {
    glPolygonMode(GL_FRONT, GL_LINE);
    glPolygonMode(GL_BACK, GL_LINE);
  }
  else {
    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_FILL);
  }
  if (_bDebugClearWhite == true) {
    //  Graphics->getClearR() = 1.f;
    //  Graphics->getClearG() = 1.f;
    //  Graphics->getClearB() = 1.f;
    //  Graphics->getClearA() = 1.f;
    //}
    //else {
    //  Graphics->getClearR() = 0.01f;
    //  Graphics->getClearG() = 0.01f;
    //  Graphics->getClearB() = 0.01f;
    //  Graphics->getClearA() = 1.0f;
  }
  //#endif
#endif
  if (_bDebugDisableCull == true) {
    glDisable(GL_CULL_FACE);
  }
  else {
    glEnable(GL_CULL_FACE);
  }
  if (_bDebugDisableDepthTest == true) {
    glDisable(GL_DEPTH_TEST);
  }
  else {
    glEnable(GL_DEPTH_TEST);
  }
}
void Scene::drawDeferred(RenderParams& rp) {
  //TODO:
  //Deferred rendering goes here.
  debugChangeRenderState();

  Perf::pushPerf();

  getContext()->pushDepthTest();
  getContext()->pushCullFace();
  getContext()->pushBlend();
  {
    getContext()->enableBlend(false);
    getContext()->enableCullFace(true);
    getContext()->enableDepthTest(true);

    //**
    //2/24 in order to set up the rendering system to be instanced we gotta change a lot around, like merge all the uniform buffers, skin joint buffers, and stuff.  Then reference by gl_InstanceID
    //so -- this is a huge task and it's nto really needed yet.
    //**

    //If we don't have skin, draw instanced, 
    //if we have skin draw normally.
    //_pRenderBucket->sortAndDrawMeshes(
    //    [](std::shared_ptr<VertexFormat> vf) {
    //        return getContext()->getShaderManager()->getDiffuseShader(vf);
    //    },
    //    [&](std::shared_ptr<ShaderBase> sb) {
    //        sb->bind();
    //        sb->setUf("_ufProj", (void*)&_projMatrix, 1, false);
    //        sb->setUf("_ufView", (void*)&_viewMatrix, 1, false);
    //        sb->setUf("_ufShadowLightPos", (void*)&vFinal, 1, false);
    //    },
    //    [](std::shared_ptr<ShaderBase> sb, std::shared_ptr<MeshNode> bn) {
    //        RenderParams rp;
    //        rp.setShader(sb);
    //        bn->drawDeferred(std::move(rp));
    //    }
    //);
    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    //glDisable(GL_BLEND);

    ////3D Models
    ////  glEnable(GL_BLEND);
    ////glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //for (std::pair<float, std::shared_ptr<BaseNode>> p : getVisibleNodes()) {
    //    std::shared_ptr<BaseNode> pm = p.second;
    //    std::shared_ptr<WorldObject> wo = std::dynamic_pointer_cast<WorldObject>(pm);
    //    if (wo != nullptr) {
    //        wo->drawDeferred(std::move(rp)); //Don't pass a matrix in, the model is the same now
    //    }
    //}
    //3D Models
    //  glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    RenderParams rp;
    for (std::pair<float, std::shared_ptr<MeshNode>> p : _pRenderBucket->getMeshes()) {
      std::shared_ptr<MeshNode> pm = p.second;
      pm->drawDeferred(rp);
    }
  }
  getContext()->popBlend();
  getContext()->popCullFace();
  getContext()->popDepthTest();

  Perf::popPerf();
}
void Scene::drawForward(RenderParams& rp) {
  BRThrowNotImplementedException();
  //debugChangeRenderState();

  //if (_pQuadMeshBackground == nullptr) {
  //  _pQuadMeshBackground = std::make_shared<MeshNode>(
  //    MeshUtils::createScreenQuadMesh(getActiveCamera()->getViewport()->getWidth(), getActiveCamera()->getViewport()->getHeight());

  //  _pTex = Gu::getTexCache()->getOrLoad(Gu::getAppPackage()->makeAssetPath("tex", "test_tex3.png"));
  //}

  ////Meshes
  //RenderParams rp;
  //for (std::pair<float, std::shared_ptr<SceneNode>> p : _pPhysicsWorld->getVisibleNodes()) {
  //  std::shared_ptr<SceneNode> pm = p.second;
  //  pm->drawForward(rp);
  //}

  //RenderUtils::drawAxisShader();
}
void Scene::drawShadow(RenderParams& rp) {
}
void Scene::drawForwardDebug(RenderParams& rp) {
}
void Scene::drawNonDepth(RenderParams& rp) {
  draw2d();
}
void Scene::drawTransparent(RenderParams& rp) {
  Perf::pushPerf();

  getContext()->pushDepthTest();
  getContext()->pushCullFace();
  getContext()->pushBlend();
  {
    getContext()->enableDepthTest(true);
    getContext()->enableCullFace(true);
    getContext()->enableBlend(false);

    RenderParams rp;
    for (std::pair<float, std::shared_ptr<MeshNode>> p : _pRenderBucket->getMeshesTransparent()) {
      std::shared_ptr<MeshNode> pm = p.second;
      pm->drawTransparent(rp);
    }
  }
  getContext()->popBlend();
  getContext()->popCullFace();
  getContext()->popDepthTest();

  Perf::popPerf();
}
void Scene::drawUI(RenderParams& rp) {
  BRThrowNotImplementedException();
  //_pUiScreen->update(Gu::getInputManager());
  //_pUiScreen->drawForward();
}
void Scene::draw2d() {
  drawDebugText();
}

void Scene::setDebugMode() {
  //set some debug vars to make ikte easier
  _bShowDebugText = true;
  _bDrawDebug = true;
  if (Gu::isDebug()) {
    _bShowDebugText = true;
    _bDrawDebug = false;
    //Keep this legit so we can test x , y
 //   _pFlyCam->setPosAndLookAt(vec3(0, 5, -20), vec3(0, 0, 0));
  }
}
void Scene::drawDebugText() {
  int bx = 5;
  int by = 32 + 34;// 0 is the height of the font
  int dy = 16;//Also the font size
  int cy = 0;

  //  _pAppUi->clearDebugText();
  //  if (_bShowDebugText) {
  //    size_t iVisibleLights = 0;//_pWorld25->getFrameVisibleLights().size();
  //
  //    //////////////////////////////////////////////////////////////////////////
  //#define DBGL(...) _pAppUi->dbgLine(StringUtil::format(__VA_ARGS__))
  ////////////////////////////////////////////////////////////////////////////
  //    //DBGL("%.2f", Gu::getFpsMeter()->getFps());
  //
  //    //    _pContext->getTextBoss()->setColor(vec4(0, 0, b, 1));
  //    DBGL("Global");
  //    DBGL("  Debug: %s", _bDrawDebug ? "Enabled" : "Disabled");
  //    DBGL("  Culling: %s", _bDebugDisableCull ? "Disabled" : "Enabled");
  //    DBGL("  Depth Test: %s", _bDebugDisableDepthTest ? "Disabled" : "Enabled");
  //    DBGL("  Render: %s", _bDebugShowWireframe ? "Wire" : "Solid");
  //    DBGL("  Clear: %s", _bDebugClearWhite ? "White" : "Black-ish");
  //    // DBGL("  Vsync: %s", (Gu::getFrameSync()->isEnabled()) ? "Enabled" : "Disabled");
  //    DBGL("  Shadows: %s", (_bDebugDisableShadows) ? "Enabled" : "Disabled");
  //
  //    //DBGL("  Camera: %s", Gu::getCamera()->getPos().toString(5).c_str());
  //
  //  }
  //  _pAppUi->endDebugText();
}
void Scene::updateWidthHeight(int32_t w, int32_t h, bool bForce) {
  BRThrowNotImplementedException();
  //_pUiScreen->screenChanged(w, h);
}
//std::shared_ptr<ModelNode> Scene::createObj(std::shared_ptr<ModelData> ms) {
//  std::shared_ptr<ModelNode> mn = std::make_shared<ModelNode>(ms);
//  mn->update(0.0, std::map<Hash32, std::shared_ptr<Animator>>());
//  
//  attachChild(mn);
//
//  return mn;
//}
//std::shared_ptr<ModelNode> Scene::createObj(std::shared_ptr<ModelData> ms, vec3& pos, vec4& rot, vec3& scale, std::string action) {
//  std::shared_ptr<ModelNode> mn = createObj(ms);
//
//  mn->stopAllActions();
//  if (StringUtil::isNotEmpty(action)) {
//    mn->playAction(action);
//  }
//
//  mn->setScale(std::move(scale));
//  mn->setRot(std::move(rot));
//  mn->setPos(std::move(pos));
//
//  return mn;
//}
std::shared_ptr<LightNodePoint> Scene::createPointLight(vec3&& pos, float radius, vec4&& color, string_t action, bool bShadowsEnabled) {
  std::shared_ptr<LightNodePoint> lp = LightNodePoint::create(bShadowsEnabled);
  lp->update(0.0f, std::map<Hash32, std::shared_ptr<Animator>>());
  lp->setPos(std::move(pos));
  lp->setLightRadius(radius);
  lp->setLightColor(std::move(color));

  lp->getShadowBox()->getSmallBoxSize() = 0.6f;
  attachChild(lp);

  return lp;
}
std::shared_ptr<LightNodeDir> Scene::createDirLight(const vec3&& pos, const vec3&& lookAt, float fDist, const vec4&& color, const string_t action, bool bShadowsEnabled) {
  std::shared_ptr<LightNodeDir> dir = LightNodeDir::create(bShadowsEnabled);
  dir->setMaxDistance(fDist);
  dir->setPos(std::move(pos));
  dir->setLookAt(std::move(lookAt));
  dir->setLightColor(std::move(color));
  //dir->getSpecExp() = 50.109f;
  dir->update(0.0f, std::map<Hash32, std::shared_ptr<Animator>>());
  
  attachChild(dir);

  return dir;
}
std::shared_ptr<TreeNode> Scene::attachChild(std::shared_ptr<TreeNode> pChild){
  std::shared_ptr<PhysicsNode> casted = std::dynamic_pointer_cast<PhysicsNode>(pChild);
  if (casted!=nullptr) {
    _pPhysicsWorld->addObj(casted, false, false);
  }
  return TreeNode::attachChild(pChild);
}
bool Scene::detachChild(std::shared_ptr<TreeNode> pChild){
  std::shared_ptr<PhysicsNode> casted = std::dynamic_pointer_cast<PhysicsNode>(pChild);
  if (casted != nullptr) {
    _pPhysicsWorld->tryRemoveObj(casted);
  }
  return TreeNode::detachChild(pChild);
}

}//ns BR2
