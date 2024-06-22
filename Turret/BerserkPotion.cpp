#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Point.hpp"
#include "BerserkPotion.hpp"
#include "Enemy/Enemy.hpp"

const std::string BerserkPotion::Potionbase="play/potion.png";
const std::string BerserkPotion::Potionimg="play/potion.png";
const int BerserkPotion::Range=200;
const int BerserkPotion::Duration=200;
const int BerserkPotion::Price=50;

BerserkPotion::BerserkPotion(float x, float y) : Potion(Potionbase, Potionimg, Range, 0, Duration, Price, x, y)
{

}

void BerserkPotion::effectenemy(Enemy *enemy)
{
	if (Engine::GameEngine::GetInstance().GetSceneName(getPlayScene())!="play-reverse") return;
	enemy->Berserk=100;
	// std::cout << "Freeze!\n";
}

void BerserkPotion::resumeenemy(Enemy* enemy)
{
	if (Engine::GameEngine::GetInstance().GetSceneName(getPlayScene())!="play-reverse") return;
	// enemy->Berserk=0;
}

void BerserkPotion::effectturret(Turret *turret)
{
	if (Engine::GameEngine::GetInstance().GetSceneName(getPlayScene())!="play-normal") return;
	turret->Berserker=1;
}

void BerserkPotion::resumeturret(Turret *turret)
{
	if (Engine::GameEngine::GetInstance().GetSceneName(getPlayScene())!="play-normal") return;
	turret->Berserker=0;
}

void BerserkPotion::CreateBullet() {}