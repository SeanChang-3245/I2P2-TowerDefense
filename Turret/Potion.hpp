#ifndef POTION_HPP
#define POTION_HPP
#include "Turret.hpp"

class Potion: public Turret {
protected:
    int damage;
    int duration;
    int range;
public:
    Potion(std::string Potionbase, std::string Potionimg, int r, int dmg, int p,  int price, float x, float y);
	void Update(float deltaTime) override;\
    
    virtual void effectenemy(Enemy *enemy) = 0;
    virtual void resumeenemy(Enemy *enemy) = 0;
    virtual void effectturret(Turret *turret) = 0;
    virtual void resumeturret(Turret *turret) = 0;
    void CreateBullet() override;
};

#endif // POTION_HPP