#ifndef LASERTURRET_HPP
#define LASERTURRET_HPP
#include "Turret.hpp"

class LaserTurret: public Turret {
public:
    static const int Damage;
	static const int Range;
	static const int Price;
    static const float Reload;
    LaserTurret(float x, float y);
    void CreateBullet() override;
};
#endif // LASERTURRET_HPP
