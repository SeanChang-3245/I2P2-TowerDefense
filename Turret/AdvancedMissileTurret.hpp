#ifndef ADVANCEDMISSILETURRET_HPP
#define ADVANCEDMISSILETURRET_HPP
#include "Turret.hpp"

class AdvancedMissileTurret: public Turret {
public:
    static const int Damage;
	static const int Range;
	static const int Price;
    static const float Reload;
    AdvancedMissileTurret(float x, float y);
    void CreateBullet() override;
};
#endif 