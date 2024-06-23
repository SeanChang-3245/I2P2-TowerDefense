#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Engine/Group.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Point.hpp"
#include "ShieldPotion.hpp"
#include "Enemy/Enemy.hpp"

const std::string ShieldPotion::Potionbase="play/ShieldPotion.png";
const std::string ShieldPotion::Potionimg="play/ShieldPotion.png";
const int ShieldPotion::Range=100;
const int ShieldPotion::Duration=100;
const int ShieldPotion::Price=50;

ShieldPotion::ShieldPotion(float x, float y) : Potion(Potionbase, Potionimg, Range, 0, Duration, Price, x, y)
{

}

void ShieldPotion::effectenemy(Enemy *enemy)
{
	if (enemy->ResetShield) return ;
	enemy->ResetShield=1;
	enemy->Shield=200;
}

void ShieldPotion::resumeenemy(Enemy* enemy)
{
	enemy->ResetShield=0;
}

void ShieldPotion::effectturret(Turret *turret)
{

}

void ShieldPotion::resumeturret(Turret *turret)
{

}

void ShieldPotion::CreateBullet() {}