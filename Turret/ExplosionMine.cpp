#include <allegro5/base.h>
#include <allegro5/allegro_primitives.h>
#include <cmath>
#include <random>
#include <string>
#include <vector>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Point.hpp"
#include "ExplosionMine.hpp"
#include "Enemy/Enemy.hpp"
#include "Engine/IScene.hpp"
#include "UI/Animation/ExplosionEffect.hpp"
#include "UI/Animation/DirtyEffect.hpp"

const int ExplosionMine::dmg=200;
const int ExplosionMine::Price=30;
inline std::string ExplosionMine::Minebase="play/Mine.png";
inline std::string ExplosionMine::Mineimg="play/Mine.png";

ExplosionMine::ExplosionMine(float x, float y) : Mine(Minebase, Mineimg, Price, x, y)
{
    damage=dmg;
}

void ExplosionMine::Explode()
{
    PlayScene* scene = getPlayScene();
    scene->TowerGroup->RemoveObject(this->GetObjectIterator());
    for (auto& it : scene->EnemyGroup->GetObjects()) {
        Engine::Point diff = it->Position - Position;
        Target = dynamic_cast<Enemy*>(it);
        if (diff.Magnitude() <= scene->BlockSize)
        {
            Target->Hit(damage);
        }
    }
    PlayScene *playscene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
    std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> distId(1, 3);
	std::uniform_int_distribution<std::mt19937::result_type> dist(1, 20);
	for (int i = 0; i < 10; i++) {
		// Random add 10 dirty effects.
		getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-" + std::to_string(distId(rng)) + ".png", dist(rng), Position.x, Position.y));
	}
}