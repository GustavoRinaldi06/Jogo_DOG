#include "ObjectFactory.h"

#include "DamageObj.h"
#include "Chainsaw.h"
#include "FallingBranch.h"
#include "Collider.h"
#include "Character.h"
#include "PlayerController.h"
#include "Camera.h"

namespace ObjectFactory
{
  GameObject* CreateHandObject(const tmx::Object& object)
  {
      GameObject* handGO = new GameObject();
      handGO->box.x = object.getAABB().left;
      handGO->box.y = object.getAABB().top;
      handGO->box.w = object.getAABB().width;
      handGO->box.h = object.getAABB().height;
      
      DamageObj* damageObj = new DamageObj(*handGO, 10
          , "recursos/img/sprites/Hand.png", "recursos/audio/Hunter/boing.mp3", "", 8, 1);
      handGO->AddComponent(damageObj);
      handGO->AddComponent(new Collider(*handGO));
      return handGO;
  }

  GameObject* CreateThornObject(const tmx::Object& object)
  {
      GameObject* thornGO = new GameObject();
      thornGO->box.x = object.getAABB().left;
      thornGO->box.y = object.getAABB().top;
      thornGO->box.w = object.getAABB().width;
      thornGO->box.h = object.getAABB().height;

      DamageObj* damageObj = new DamageObj(*thornGO, 10
          , "recursos/img/sprites/Thorn.png", "recursos/audio/Hunter/boing.mp3", "", 8, 1, "Thorn");
      thornGO->AddComponent(damageObj);
      thornGO->AddComponent(new Collider(*thornGO));
      return thornGO;
  }

  GameObject* CreateChainSawObject(const tmx::Object& object)
  {
      GameObject* chainSawGO = new GameObject();
      chainSawGO->box.x = object.getAABB().left;
      chainSawGO->box.y = object.getAABB().top;
      chainSawGO->box.w = object.getAABB().width;
      chainSawGO->box.h = object.getAABB().height;

      Chainsaw* damageObj = new Chainsaw(*chainSawGO, 10, "recursos/img/sprites/Chainsaw.png", "recursos/audio/Hunter/boing.mp3", "", 8, 1);
      chainSawGO->AddComponent(damageObj);
      chainSawGO->AddComponent(new Collider(*chainSawGO));
      return chainSawGO;
  }

  GameObject* CreateFallingBranchObject(const tmx::Object& object)
  {
      GameObject* branchGO = new GameObject();
      branchGO->box.x = object.getAABB().left;
      branchGO->box.y = object.getAABB().top;
      branchGO->box.w = object.getAABB().width;
      branchGO->box.h = object.getAABB().height;

      FallingBranch* fallingBranch = new FallingBranch(*branchGO, 100.0f, 10.0f, "recursos/img/sprites/FallingBranch.png", 8, 1);
      branchGO->AddComponent(fallingBranch);
      branchGO->AddComponent(new Collider(*branchGO));
      return branchGO;
  }

  GameObject* CreateColliderObject(const tmx::Object& object)
  {
    GameObject *obj = new GameObject();
    obj->box.x = object.getAABB().left;
    obj->box.y = object.getAABB().top;
    obj->box.w = object.getAABB().width;
    obj->box.h = object.getAABB().height;

    auto collider = new Collider(*obj);
    collider->tag = "ground"; 
    obj->AddComponent(collider);
    return obj;
  }

  GameObject* CreatePlayerObject(const tmx::Object& object)
  {
    GameObject *playerGO = new GameObject();
    playerGO->box.x = object.getAABB().left;
    playerGO->box.y = object.getAABB().top;

    playerGO->AddComponent(new Character(*playerGO, "recursos/img/sprites/Player.png"));
    playerGO->AddComponent(new PlayerController(*playerGO));

    Camera::GetInstance().Follow(playerGO);

    return playerGO;
  }
}