#ifndef ADVANCEDTANKENEMY_HPP
#define ADVANCEDTANKENEMY_HPP
#include "Enemy.hpp"

class AdvancedTankEnemy : public Enemy {
public:
	// The cost to spawn in reverse play scene
	static const int Cost;
	static const float Speed;
	static const float HP;
	// The money the user will earn if kill this enemy
	static const int Money;
	static const int KillScore;

	AdvancedTankEnemy(int x, int y);
	void Update(float deltaTime) override;
};
#endif 