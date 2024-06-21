#include <string>

#include "AdvancedTankEnemy.hpp"

const int AdvancedTankEnemy::Cost = 20;
const float AdvancedTankEnemy::Speed = 15;
const float AdvancedTankEnemy::HP = 200;
const int AdvancedTankEnemy::Money = 75;
const int AdvancedTankEnemy::KillScore = 300;

AdvancedTankEnemy::AdvancedTankEnemy(int x, int y) :
    Enemy("play/enemy-4.png", x, y, 20, Speed, HP, Money) 
{
	// TODO: [CUSTOM-TOOL] You can imitate the 2 files: 'SoldierEnemy.hpp', 'SoldierEnemy.cpp' to create a new enemy.
	Enemy::kill_score = KillScore;
}

void AdvancedTankEnemy::Update(float deltaTime)
{
	Enemy::Update(deltaTime);
	speed = 15 + 30 * (200-hp) / 200;
}