#ifndef MACHINEGUNTURRET_HPP
#define MACHINEGUNTURRET_HPP
#include "Turret.hpp"

class MachineGunTurret: public Turret {
public:
	static const int Damage;
	static const int Range;
	static const int Price;
	static const float Reload;
    MachineGunTurret(float x, float y);
	void CreateBullet() override;
};
#endif // MACHINEGUNTURRET_HPP
