#ifndef FROSTPOTION_HPP
#define FROSTPOTION_HPP
#include "Potion.hpp"
#include <string>

class FrostPotion: public Potion {
public:
    static const std::string Potionbase;
    static const std::string Potionimg;
    static const int Range;
    static const int Duration;
    static const int Price;
    FrostPotion(float x, float y);
	// void Update(float deltaTime) override;
    void effectenemy(Enemy *enemy) override;
    void resumeenemy(Enemy *enemy) override;
    void effectturret(Turret *turret) override;
    void resumeturret(Turret *turret) override;
    void CreateBullet() override;
};

#endif // FROSTPOTION_HPP