#pragma once
#ifndef _SG_RAPIDJSON_H_
#define _SG_RAPIDJSON_H_

#include "scenegraph.h"

namespace otv {
  class SgRapidJSON : public SceneGraphAbstract {
  public:    
    void Dump(const std::string& fname = "");
    void Load(const std::string& fname = "");
  };
  using SceneGraph = SgRapidJSON;
};

#endif//_SG_RAPIDJSON_H_
