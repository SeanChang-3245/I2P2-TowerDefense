#ifndef MISSILETURRET_HPP
#define MISSILETURRET_HPP
#include "Turret.hpp"

class MissileTurret: public Turret {
public:
	static const int Damage;
	static const int Range;
	static const int Price;
	static const float Reload;
    MissileTurret(float x, float y);
    void CreateBullet() override;
};
#endif // MISSILETURRET_HPP
