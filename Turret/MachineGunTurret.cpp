#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Bullet/FireBullet.hpp"
#include "Bullet/FrostBullet.hpp"
#include "Engine/Group.hpp"
#include "MachineGunTurret.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Point.hpp"

const int MachineGunTurret::Price = 50;
const int MachineGunTurret::Range = 200;
const int MachineGunTurret::Damage = FireBullet::Damage;
const float MachineGunTurret::Reload = 0.5;

MachineGunTurret::MachineGunTurret(float x, float y) :
	// TODO: [CUSTOM-TOOL] You can imitate the 2 files: 'MachineGunTurret.hpp', 'MachineGunTurret.cpp' to create a new turret.
	Turret("play/tower-base.png", "play/turret-1.png", x, y, Range, Price, Reload, TURRET) {
	// Move center downward, since we the turret head is slightly biased upward.
	Anchor.y += 8.0f / GetBitmapHeight();
	bullet_speed = FireBullet::Speed;
	AbletocastSnowball=0;
}
void MachineGunTurret::CreateBullet() {
	Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
	float rotation = atan2(diff.y, diff.x);
	Engine::Point normalized = diff.Normalize();
	// Change bullet position to the front of the gun barrel.
	if (FrostUpdate)
	{
		getPlayScene()->BulletGroup->AddNewObject(new FrostBullet(Position + normalized * 36, diff, rotation, this));
	}
	else
	{
		getPlayScene()->BulletGroup->AddNewObject(new FireBullet(Position + normalized * 36, diff, rotation, this));
	}
	AudioHelper::PlayAudio("gun.wav");
}
