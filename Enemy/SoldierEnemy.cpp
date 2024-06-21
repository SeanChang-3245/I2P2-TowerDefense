#include <string>

#include "SoldierEnemy.hpp"

const int SoldierEnemy::Cost = 2;
const float SoldierEnemy::Speed = 50;
const float SoldierEnemy::HP = 5;
const int SoldierEnemy::Money = 5;
const int SoldierEnemy::KillScore = 50;

SoldierEnemy::SoldierEnemy(int x, int y) : Enemy("play/enemy-1.png", x, y, 10, Speed, HP, Money) {
	// TODO: [CUSTOM-TOOL] You can imitate the 2 files: 'SoldierEnemy.hpp', 'SoldierEnemy.cpp' to create a new enemy.
	Enemy::kill_score = KillScore;
}
