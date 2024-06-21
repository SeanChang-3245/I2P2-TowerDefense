#ifndef SHIELDPOTION_HPP
#define SHIELDPOTION_HPP
#include "Potion.hpp"
#include <string>

class ShieldPotion: public Potion {
public:
    static const std::string Potionbase;
    static const std::string Potionimg;
    static const int Range;
    static const int Duration;
    static const int Price;
    ShieldPotion(float x, float y);
	// void Update(float deltaTime) override;
    void effectenemy(Enemy *enemy) override;
    void resumeenemy(Enemy *enemy) override;
    void effectturret(Turret *turret) override;
    void resumeturret(Turret *turret) override;
    void CreateBullet() override;
};

#endif // SHIELDPOTION_HPP