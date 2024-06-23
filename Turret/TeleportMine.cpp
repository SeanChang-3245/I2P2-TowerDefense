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
#include "Engine/Point.hpp"
#include "TeleportMine.hpp"
#include "Enemy/Enemy.hpp"
#include "Engine/IScene.hpp"
#include "UI/Animation/ExplosionEffect.hpp"
#include "UI/Animation/DirtyEffect.hpp"

const int TeleportMine::dmg=200;
const int TeleportMine::Price=30;
inline static std::string Minebase="play/Mine.png";
inline static std::string Mineimg="play/Mine.png";

TeleportMine::TeleportMine(float x, float y) : Mine(Minebase, Mineimg, Price, x, y)
{
    damage=dmg;
}

void TeleportMine::Explode()
{
    PlayScene* scene = getPlayScene();
    scene->TowerGroup->RemoveObject(this->GetObjectIterator());
    for (auto& it : scene->EnemyGroup->GetObjects()) {
        Engine::Point diff = it->Position - Position;
        Target = dynamic_cast<Enemy*>(it);
        int rx=0, ry=0;
        if (diff.Magnitude() <= scene->BlockSize)
        {
            // while (scene->mapState[ry][rx]==PlayScene::TILE_OCCUPIED) rx=rand() % scene->MapWidth, ry=rand() % scene->MapHeight;
            srand( time(NULL) );
            Target->Position=Engine::Point(ry*scene->BlockSize+scene->BlockSize/2, rx*scene->BlockSize+scene->BlockSize/2);
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