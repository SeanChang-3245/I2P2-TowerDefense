#include <allegro5/base.h>
#include <allegro5/color.h>
#include <allegro5/allegro_primitives.h>
#include <cmath>
#include <string>

#include "Engine/Group.hpp"
#include "Scene/PlayScene.hpp"
#include "Scene/NormalPlayScene.hpp"
#include "Engine/Point.hpp"
#include "Potion.hpp"
#include "Enemy/Enemy.hpp"

inline static std::string Potionbase="play/shovel.png";
inline static std::string Potionimg="play/shovel.png";

Potion::Potion(std::string Potionbase, std::string Potionimg, int r, int dmg, int p, int price, float x, float y) : Turret(Potionbase, Potionimg, x, y, r, price, 0, POTION)
{
    damage=dmg;
    duration=p;
}

void Potion::Update(float deltaTime) 
{
  Sprite::Update(deltaTime);
  PlayScene* scene = getPlayScene();
	imgBase.Position = Position;
	imgBase.Tint = Tint;
  if (!Enabled)
		return;
  duration-=1;
  if (duration<=0)
  {
    for (auto& it : scene->EnemyGroup->GetObjects())
    {
      Target = dynamic_cast<Enemy*>(it);
      resume(Target);
    }
    // scene->mapState[Position.y][Position.x]=scene->originalMapState[Position.y][Position.x];
    scene->TowerGroup->RemoveObject(this->GetObjectIterator());
    return ;
  }
  
  for (auto& it : scene->EnemyGroup->GetObjects()) {
			Engine::Point diff = it->Position - Position;
      Target = dynamic_cast<Enemy*>(it);
			if (diff.Magnitude() <= CollisionRadius) effect(Target);
      else resume(Target);
		}
}

void Potion::CreateBullet() {}