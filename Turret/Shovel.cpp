#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Engine/Group.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Point.hpp"
#include "Shovel.hpp"

Shovel::Shovel(float x, float y) :
    Turret("play/shovel.png", "play/shovel.png", x, y, 0, 0, 0, TOOL)
{}

void Shovel::Update(float deltaTime) 
{
    Sprite::Update(deltaTime);
	imgBase.Position = Position;
	imgBase.Tint = Tint;
}
void Shovel::CreateBullet() {}