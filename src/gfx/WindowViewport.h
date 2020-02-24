/**
*  @file Viewport.h
*  @date September 24, 2011
*  @author MetalMario971
*/
#pragma once
#ifndef __VIEWPORT_1483030407219963222425227_H__
#define __VIEWPORT_1483030407219963222425227_H__

#include "../base/BaseHeader.h"
#include "../math/MathAll.h"
#include "../gfx/ScreenProps.h"
namespace BR2 {
/**
*  @class Viewport
*  @brief Viewport class.
*/
class WindowViewport : public VirtualMemory /*This is contained by RenderWindow, do not accept window param events */ {
public:
  WindowViewport(
    int32_t w
    , int32_t h
    , VIEWPORT_LOCATION location = VIEWPORT_LOCATION::VIEWPORT_TOPLEFTCORNER
    , ViewportConstraint constraint = ViewportConstraint::VP_FILL_WINDOW
  );
  FORCE_INLINE float getWidth_1() { return _width_1; }
  FORCE_INLINE float getHeight_1() { return _height_1; }
  Box2f getClientQuad();
  //These return a percentage of the width/height of the viewport.  f is [0,100]
  float pctW(float f);
  float pctH(float f);
  // - Set the width / height of the viewport given the window's width/height parameters. (not the viewport's)
  void setHeight(int32_t h);
  void setWidth(int32_t w);
  void setX(int32_t x);
  void setY(int32_t y);
  int32_t getWidth();
  int32_t getHeight();
  int32_t getX();
  int32_t getY();
  float getAspectRatio();
  float getAspectRatio_1();
  void updateAspectRatio();
  void updateChanged(bool blnForce = false);
  bool containsPointRelativeToWindow(vec2& mp);

private:
  float _width_1, _height_1;    // Reciprocal width/ height for faster computations.
  int32_t _windowWidth, _windowHeight;        // - We need the h/w of the host window to change the location of the viewport inside of it.
  Box3i _lastViewportRect;
  Box3i _currentViewportRect;
  VIEWPORT_LOCATION _viewportLocation;
  ViewportConstraint _constraint;
  t_screen_resolution    _resolution;
  float _aspectRatio;
  float _aspectRatio_1;
  void centerViewport();
  void updateReciprocalValues();
};

}//ns game



#endif
