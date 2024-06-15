#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Engine/Group.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Point.hpp"
#include "BerserkPotion.hpp"
#include "Enemy/Enemy.hpp"

const std::string BerserkPotion::Potionbase="play/potion.png";
const std::string BerserkPotion::Potionimg="play/potion.png";
const int BerserkPotion::Range=100;
const int BerserkPotion::Duration=100;
const int BerserkPotion::Price=50;

BerserkPotion::BerserkPotion(float x, float y) : Potion(Potionbase, Potionimg, Range, 0, Duration, Price, x, y)
{

}

void BerserkPotion::effect(Enemy *enemy)
{
	enemy->Berserk=1;
	// std::cout << "Freeze!\n";
}

void BerserkPotion::resume(Enemy* enemy)
{
	enemy->Berserk=0;
}

void BerserkPotion::CreateBullet() {}