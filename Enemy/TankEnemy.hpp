#ifndef TANKENEMY_HPP
#define TANKENEMY_HPP
#include "Enemy.hpp"
#include "Engine/Sprite.hpp"

class TankEnemy : public Enemy {
private:
	Sprite head;
	float targetRotation;
public:
	// The cost to spawn in reverse play scene
	static const int Cost;
	static const float Speed;
	static const float HP;
	// The money the user will earn if kill this enemy
	static const int Money;
	static const int KillScore;

	TankEnemy(int x, int y);
	void Draw() const override;
	void Update(float deltaTime) override;
};
#endif // TANKENEMY_HPP
