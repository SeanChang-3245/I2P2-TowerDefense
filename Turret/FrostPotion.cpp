#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Point.hpp"
#include "FrostPotion.hpp"
#include "Enemy/Enemy.hpp"

const std::string FrostPotion::Potionbase="play/FrostPotion.png";
const std::string FrostPotion::Potionimg="play/FrostPotion.png";
const int FrostPotion::Range=100;
const int FrostPotion::Duration=100;
const int FrostPotion::Price=50;

FrostPotion::FrostPotion(float x, float y) : Potion(Potionbase, Potionimg, Range, 0, Duration, Price, x, y)
{

}

void FrostPotion::effectenemy(Enemy *enemy)
{
	std::string SceneName = Engine::GameEngine::GetInstance().GetSceneName(getPlayScene());
	if (SceneName == "play-reverse") return;
	enemy->Frozen=1;
}

void FrostPotion::resumeenemy(Enemy* enemy)
{
	std::string SceneName = Engine::GameEngine::GetInstance().GetSceneName(getPlayScene());
	if(SceneName == "play-reverse") return;
	enemy->Frozen=0;
}

void FrostPotion::effectturret(Turret *turret)
{
	std::string SceneName = Engine::GameEngine::GetInstance().GetSceneName(getPlayScene());
	if (SceneName == "play-normal" || SceneName == "play-black" || SceneName == "play-survival") return;
	turret->Freeze=1;
}

void FrostPotion::resumeturret(Turret *turret)
{
	std::string SceneName = Engine::GameEngine::GetInstance().GetSceneName(getPlayScene());
	if (SceneName == "play-normal" || SceneName == "play-black" || SceneName == "play-survival") return;
	turret->Freeze=0;
}

void FrostPotion::CreateBullet() {}