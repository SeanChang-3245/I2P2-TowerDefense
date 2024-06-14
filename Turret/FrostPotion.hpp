#ifndef FROSTPOTION_HPP
#define FROSTPOTION_HPP
#include "Potion.hpp"

class FrostPotion: public Potion {
public:
    FrostPotion(float x, float y);
	// void Update(float deltaTime) override;
    void effect(Enemy *enemy) override;
    void resume(Enemy *enemy) override;
    void CreateBullet() override;
};

#endif // FROSTPOTION_HPP