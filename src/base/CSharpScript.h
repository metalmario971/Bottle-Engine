/**
*  @file CSharpScript.h
*  @date August 23, 2019
*  @author MetalMario971
*/
#pragma once
#ifndef __CSHARPSCRIPT_15665746341154937548_H__
#define __CSHARPSCRIPT_15665746341154937548_H__

#include "../world/WorldHeader.h"
#include "../world/Component.h"

namespace BR2 {
/**
*  @class CSharpScript
*  @brief A minimally functional C Sharp script.
*/
class CSharpScript : public Component {
public:
  CSharpScript();
  virtual ~CSharpScript() override;

  virtual void start() = 0;

};

}//ns BR2



#endif
