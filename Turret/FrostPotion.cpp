#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Engine/Group.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Point.hpp"
#include "FrostPotion.hpp"
#include "Enemy/Enemy.hpp"

inline static std::string Potionbase="play/potion.png";
inline static std::string Potionimg="play/potion.png";

FrostPotion::FrostPotion(float x, float y) : Potion(Potionbase, Potionimg, 100, 0, 100, 50, x, y)
{

}

#include <iostream>
void FrostPotion::effect(Enemy *enemy)
{
	enemy->Frozen=1;
	// std::cout << "Freeze!\n";
}

void FrostPotion::resume(Enemy* enemy)
{
	enemy->Frozen=0;
}

void FrostPotion::CreateBullet() {}