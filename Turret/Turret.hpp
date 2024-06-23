#ifndef TURRET_HPP
#define TURRET_HPP
#include <allegro5/base.h>
#include <list>
#include <string>
#include <vector>

#include "Engine/Sprite.hpp"

class Enemy;
class PlayScene;

enum Turret_Type
{
    TURRET, TOOL, POTION
};

class Turret: public Engine::Sprite {
protected:
    Turret_Type type;
    int price;
    float coolDown;
    float reload = 0;
    float rotateRadian = 2 * ALLEGRO_PI;
    float bullet_speed;
    Sprite imgBase;
    std::list<Turret*>::iterator lockedTurretIterator;
    PlayScene* getPlayScene();
    // Reference: Design Patterns - Factory Method.
    virtual void CreateBullet() = 0;

public:
    bool Enabled = true;
    bool Freeze;
    int Berserker;
    // decide whether the turret is preview or a real one
    bool Preview = false;
    Enemy* Target = nullptr;
    Turret *Target_tower;
    Turret(std::string imgBase, std::string imgTurret, float x, float y, float radius, int price, float coolDown, Turret_Type type);
    void Update(float deltaTime) override;
    void Draw() const override;
	int GetPrice() const;
    Turret_Type GetType() const;
};
#endif // TURRET_HPP
