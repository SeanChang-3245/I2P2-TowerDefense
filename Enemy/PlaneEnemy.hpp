#ifndef PLANEENEMY_HPP
#define PLANEENEMY_HPP
#include "Enemy.hpp"

class PlaneEnemy : public Enemy {
public:
	// The cost to spawn in reverse play scene
	static const int Cost;
	static const float Speed;
	static const float HP;
	// The money the user will earn if kill this enemy
	static const int Money;
	static const int KillScore;
	PlaneEnemy(int x, int y);
};
#endif // PLANEENEMY_HPP
