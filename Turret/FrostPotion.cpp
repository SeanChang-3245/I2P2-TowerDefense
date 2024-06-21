#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Engine/GameEngine.hpp"
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

void FrostPotion::effectenemy(Enemy *enemy)
{
	if (Engine::GameEngine::GetInstance().GetSceneName(getPlayScene())=="play-reverse") return;
	enemy->Frozen=1;
}

void FrostPotion::resumeenemy(Enemy* enemy)
{
	if (Engine::GameEngine::GetInstance().GetSceneName(getPlayScene())=="play-reverse") return;
	enemy->Frozen=0;
}

#include <iostream>
void FrostPotion::effectturret(Turret *turret)
{
	if (Engine::GameEngine::GetInstance().GetSceneName(getPlayScene())=="play-normal") return;
	turret->Freeze=1;
}

void FrostPotion::resumeturret(Turret *turret)
{
	if (Engine::GameEngine::GetInstance().GetSceneName(getPlayScene())=="play-normal") return;
	turret->Freeze=0;
}

void FrostPotion::CreateBullet() {}