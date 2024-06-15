#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Engine/Group.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Point.hpp"
#include "FrostPotion.hpp"
#include "Enemy/Enemy.hpp"

const std::string FrostPotion::Potionbase="play/potion.png";
const std::string FrostPotion::Potionimg="play/potion.png";
const int FrostPotion::Range=100;
const int FrostPotion::Duration=100;
const int FrostPotion::Price=50;

FrostPotion::FrostPotion(float x, float y) : Potion(Potionbase, Potionimg, Range, 0, Duration, Price, x, y)
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