#include <allegro5/base.h>
#include <allegro5/color.h>
#include <allegro5/allegro_primitives.h>
#include <cmath>
#include <string>

#include "Engine/Group.hpp"
#include "Scene/PlayScene.hpp"
#include "Scene/NormalPlayScene.hpp"
#include "Engine/Point.hpp"
#include "Mine.hpp"
#include "Enemy/Enemy.hpp"

// inline static std::string Minebase="play/Mine.png";
// inline static std::string Mineimg="play/Mine.png";

Mine::Mine(std::string Minebase, std::string Mineimg, int price, float x, float y) : Turret(Minebase, Mineimg, x, y, 0, price, 0, MINE)
{
    CollisionRadius=10;
    initializetime=100;
}

void Mine::Update(float deltaTime) 
{
    Sprite::Update(deltaTime);
    PlayScene* scene = getPlayScene();
    imgBase.Position = Position;
    imgBase.Tint = Tint;
    if (!Enabled)
        return;
    if (initializetime>0)
    {
        initializetime--;
        this->Tint=al_map_rgba(0, 0, 0, 160);
        return;
    }
    this->Tint=al_map_rgba(255, 255, 255, 255);
    for (auto& it : scene->EnemyGroup->GetObjects()) {
        Engine::Point diff = it->Position - Position;
        Target = dynamic_cast<Enemy*>(it);
        if (diff.Magnitude() <= CollisionRadius)
        {
            Explode();
            return ;
        }
    }
}

void Mine::CreateBullet()
{
    
}

int Mine::GetInitialTime()
{
    return initializetime;
}