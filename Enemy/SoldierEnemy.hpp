#ifndef SOLDIERENEMY_HPP
#define SOLDIERENEMY_HPP
#include "Enemy.hpp"

class SoldierEnemy : public Enemy {
public:
	// The cost to spawn in reverse play scene
	static const int Cost;
	static const float Speed;
	static const float HP;
	// The money the user will earn if kill this enemy
	static const int Money;
	static const int KillScore;
	SoldierEnemy(int x, int y);
};
#endif // SOLDIERENEMY_HPP
