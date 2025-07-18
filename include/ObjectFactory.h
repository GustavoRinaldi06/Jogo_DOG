#pragma once

#include "GameObject.h"

#include "tmxlite/Object.hpp"

namespace ObjectFactory
{
  GameObject *CreateHandObject(const tmx::Object &object);

  GameObject *CreateThornObject(const tmx::Object &object);

  GameObject *CreateChainSawObject(const tmx::Object &object);

  GameObject *CreateFallingBranchObject(const tmx::Object &object);

  GameObject *CreateColliderObject(const tmx::Object &object);

  GameObject *CreatePlayerObject(const tmx::Object &object);

  GameObject *CreateWaterLilyObject(const tmx::Object &object);

  GameObject *CreateTreeObject(const tmx::Object &object);

  GameObject *CreateTreeBossObject(const tmx::Object &object);

  GameObject *CreateGateObject(const tmx::Object &object);

  GameObject *CreateDeerEatingObject(const tmx::Object &object);

  GameObject *CreateDangerAreaObject(const tmx::Object &object);

  GameObject *CreateDeerObject(float x, float y);

  GameObject *CreateDeerObject(const tmx::Object &object);
}