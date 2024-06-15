#ifndef BERSERKPOTION_HPP
#define BERSERKPOTION_HPP
#include "Potion.hpp"
#include <string>

class BerserkPotion: public Potion {
public:
    static const std::string Potionbase;
    static const std::string Potionimg;
    static const int Range;
    static const int Duration;
    static const int Price;
    BerserkPotion(float x, float y);
	// void Update(float deltaTime) override;
    void effect(Enemy *enemy) override;
    void resume(Enemy *enemy) override;
    void CreateBullet() override;
};

#endif // BERSERKPOTION_HPP