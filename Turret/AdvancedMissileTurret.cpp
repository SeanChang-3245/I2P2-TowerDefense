#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Bullet/AdvancedMissileBullet.hpp"
#include "AdvancedMissileTurret.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Point.hpp"


const int AdvancedMissileTurret::Price = 500;
const int AdvancedMissileTurret::Range = 500;
const int AdvancedMissileTurret::Damage = AdvancedMissileBullet::Damage;
const float AdvancedMissileTurret::Reload = 2;

AdvancedMissileTurret::AdvancedMissileTurret(float x, float y) :
	Turret("play/tower-base.png", "play/turret-6.png", x, y, Range, Price, Reload, TURRET) 
{
	// make turret point directly at enemy
	bullet_speed = 100000;
}

void AdvancedMissileTurret::CreateBullet() {
	Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
	float rotation = atan2(diff.y, diff.x);
	Engine::Point normalized = diff.Normalize();
	Engine::Point normal = Engine::Point(-normalized.y, normalized.x);
	
    // Change bullet position to the front of the gun barrel.
	getPlayScene()->BulletGroup->AddNewObject(new AdvancedMissileBullet(Position + normalized * 10, diff, rotation, this));
	// getPlayScene()->BulletGroup->AddNewObject(new MissileBullet(Position + normalized * 10 + normal * 6, diff, rotation, this));
	AudioHelper::PlayAudio("missile.wav");
}
