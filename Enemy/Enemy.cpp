#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <cmath>
#include <random>
#include <string>
#include <vector>

#include "Engine/AudioHelper.hpp"
#include "Bullet/Bullet.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "Enemy.hpp"
#include "UI/Animation/ExplosionEffect.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IScene.hpp"
#include "Engine/LOG.hpp"
#include "Scene/PlayScene.hpp"
#include "Scene/ReversePlayScene.hpp"
#include "Turret/Turret.hpp"

using Engine::LOG;
using Engine::INFO;

PlayScene* Enemy::getPlayScene() {
	PlayScene *playscene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
	return playscene;
}

void Enemy::OnExplode() {
	if(getPlayScene() == nullptr)
		LOG(INFO) << "play scene nullptr";
	if(getPlayScene()->EffectGroup == nullptr)
		LOG(INFO) << "effect group nullptr";

	getPlayScene()->EffectGroup->AddNewObject(new ExplosionEffect(Position.x, Position.y));
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> distId(1, 3);
	std::uniform_int_distribution<std::mt19937::result_type> dist(1, 20);
	for (int i = 0; i < 10; i++) {
		// Random add 10 dirty effects.
		getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-" + std::to_string(distId(rng)) + ".png", dist(rng), Position.x, Position.y));
	}
}

Enemy::Enemy(std::string img, float x, float y, float radius, float speed, float hp, int money) :
	Engine::Sprite(img, x, y), speed(speed), hp(hp), money(money)
{
	CollisionRadius = radius;
	reachEndTime = 0;
	froze_count_down = 0;
	Frozen=0;
	ResetShield=0;
	Shield=0;
	Berserk=0;
	FrostCount=0;
}

void Enemy::Hit(float damage) {
	if (Berserk) damage*2/3;
	if (Shield>0)
	{
		Shield-=damage;
	}
	else hp -= damage;
	if (hp <= 0) {
		OnExplode();
		// Remove all turret's reference to target.
		for (auto& it: lockedTurrets)
			it->Target = nullptr;
		for (auto& it: lockedBullets)
			it->Target = nullptr;
		getPlayScene()->EarnScore(kill_score);
		getPlayScene()->EarnMoney(money);
		getPlayScene()->EnemyGroup->RemoveObject(objectIterator);
		AudioHelper::PlayAudio("explosion.wav");
	}
}

void Enemy::UpdatePath(const std::vector<std::vector<int>>& mapDistance) {
	int x = static_cast<int>(floor(Position.x / PlayScene::BlockSize));
	int y = static_cast<int>(floor(Position.y / PlayScene::BlockSize));

	if (x < 0) x = 0;
	if (x >= PlayScene::MapWidth) x = PlayScene::MapWidth - 1;
	if (y < 0) y = 0;
	if (y >= PlayScene::MapHeight) y = PlayScene::MapHeight - 1;
	
	Engine::Point pos(x, y);
	int num = mapDistance[y][x];
	if (num == -1) {
		num = 0;
		Engine::LOG(Engine::ERROR) << "Enemy path finding error";
	}
	path = std::vector<Engine::Point>(num + 1);
	while (num != 0) {
		std::vector<Engine::Point> nextHops;
		for (auto& dir : PlayScene::directions) {
			int x = pos.x + dir.x;
			int y = pos.y + dir.y;
			if (x < 0 || x >= PlayScene::MapWidth || y < 0 || y >= PlayScene::MapHeight || mapDistance[y][x] != num - 1)
				continue;
			nextHops.emplace_back(x, y);
		}
		// There might be multiple shortest path to the end point
		// Choose arbitrary one.
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> dist(0, nextHops.size() - 1);
		pos = nextHops[dist(rng)];
		path[num] = pos;
		num--;
	}
	path[0] = PlayScene::EndGridPoint;
			
	// for(int i = path.size()-1; i >= 0; --i)
	// 	std::cout << path[i].x << ' ' << path[i].y << '\n';
}

void Enemy::UpdateIntermediatePath(const std::vector<std::vector<int>>& mapDistance) {
	if(mapDistance.empty())
	{
		intermediate_path.clear();
		return;
	}
	
	int x = static_cast<int>(floor(Position.x / PlayScene::BlockSize));
	int y = static_cast<int>(floor(Position.y / PlayScene::BlockSize));

	if (x < 0) x = 0;
	if (x >= PlayScene::MapWidth) x = PlayScene::MapWidth - 1;
	if (y < 0) y = 0;
	if (y >= PlayScene::MapHeight) y = PlayScene::MapHeight - 1;
	
	Engine::Point pos(x, y);
	int num = mapDistance[y][x];
	if (num == -1) {
		num = 0;
		Engine::LOG(Engine::ERROR) << "Enemy intermediate path finding error";
	}
	intermediate_path.clear();
	while (num != 0) {
		std::vector<Engine::Point> nextHops;
		for (auto& dir : PlayScene::directions) {
			int x = pos.x + dir.x;
			int y = pos.y + dir.y;
			if (x < 0 || x >= PlayScene::MapWidth || y < 0 || y >= PlayScene::MapHeight || mapDistance[y][x] != num - 1)
				continue;
			nextHops.emplace_back(x, y);
		}
		// There might be multiple shortest path to the end point
		// Choose arbitrary one.
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> dist(0, nextHops.size() - 1);
		pos = nextHops[dist(rng)];
		intermediate_path.push_front(pos);
		num--;
	}

	// path[0] = PlayScene::EndGridPoint;
	
			
	// for(int i = intermediate_path.size()-1; i >= 0; --i)
	// 	std::cout << intermediate_path[i].x << ' ' << intermediate_path[i].y << '\n';
}

void Enemy::Update(float deltaTime) {
	// Pre-calculate the velocity.
	if (FrostCount>0) Frozen=1;
	if (Frozen) return;
	float remainSpeed = speed * deltaTime;
	if (Berserk-->0) remainSpeed *= 2;
	froze_count_down -= deltaTime;
	if(froze_count_down > 0)
		remainSpeed = 0.001;
	else
		froze_count_down = 0;
	
	while (remainSpeed != 0) 
	{
		if(intermediate_path.size() && !pass_intermediate_point)
		{
			Engine::Point target = intermediate_path.back() * PlayScene::BlockSize + Engine::Point(PlayScene::BlockSize / 2, PlayScene::BlockSize / 2);
			Engine::Point vec = target - Position;
			// Add up the distances:
			// 1. to path.back()
			// 2. path.back() to border
			// 3. All intermediate block size
			// 4. to end point
			// reachEndTime = (vec.Magnitude() + (path.size() - 1) * PlayScene::BlockSize - remainSpeed) / speed;
			Engine::Point normalized = vec.Normalize();
			if (remainSpeed - vec.Magnitude() > 0) 
			{
				Position = target;
				intermediate_path.pop_back();
				remainSpeed -= vec.Magnitude();
				// Reach the intermediate point
				if(intermediate_path.size() == 0)
				{
					UpdatePath(getPlayScene()->mapDistance);
					pass_intermediate_point = true;
				}
			}
			else {
				Velocity = normalized * remainSpeed / deltaTime;
				remainSpeed = 0;
			}
		}
		else
		{
			pass_intermediate_point = true;
			if (path.empty()) 
			{
				// Reach end point.
				Shield=0;
				Hit(hp);
				getPlayScene()->Hit();
				reachEndTime = 0;
				return;
			}
			Engine::Point target = path.back() * PlayScene::BlockSize + Engine::Point(PlayScene::BlockSize / 2, PlayScene::BlockSize / 2);
			Engine::Point vec = target - Position;
			// Add up the distances:
			// 1. to path.back()
			// 2. path.back() to border
			// 3. All intermediate block size
			// 4. to end point
			reachEndTime = (vec.Magnitude() + (path.size() - 1) * PlayScene::BlockSize - remainSpeed) / speed;
			Engine::Point normalized = vec.Normalize();
			if (remainSpeed - vec.Magnitude() > 0) {
				Position = target;
				path.pop_back();
				remainSpeed -= vec.Magnitude();
			}
			else {
				Velocity = normalized * remainSpeed / deltaTime;				// if (Berserk) std::cout << "Berserk!!\n";
				remainSpeed = 0;
			}
		}

	}
	// if (Berserk) Velocity.x*=3, Velocity.y*=3, Berserk-=1;
	Rotation = atan2(Velocity.y, Velocity.x);
	Sprite::Update(deltaTime);
}

// void Enemy::Update(float deltaTime) {
// 	// Pre-calculate the velocity.

// 	float remainSpeed = speed * deltaTime;
// 	froze_count_down -= deltaTime;
// 	if(froze_count_down > 0)
// 		remainSpeed = 0.001;
// 	else
// 		froze_count_down = 0;

// 	while (remainSpeed != 0) {
// 		if (path.empty()) {
// 			// Reach end point.
// 			Hit(hp);
// 			getPlayScene()->Hit();
// 			reachEndTime = 0;
// 			return;
// 		}
// 		Engine::Point target = path.back() * PlayScene::BlockSize + Engine::Point(PlayScene::BlockSize / 2, PlayScene::BlockSize / 2);
// 		Engine::Point vec = target - Position;
// 		// Add up the distances:
// 		// 1. to path.back()
// 		// 2. path.back() to border
// 		// 3. All intermediate block size
// 		// 4. to end point
// 		reachEndTime = (vec.Magnitude() + (path.size() - 1) * PlayScene::BlockSize - remainSpeed) / speed;
// 		Engine::Point normalized = vec.Normalize();
// 		if (remainSpeed - vec.Magnitude() > 0) {
// 			Position = target;
// 			path.pop_back();
// 			remainSpeed -= vec.Magnitude();
// 		}
// 		else {
// 			Velocity = normalized * remainSpeed / deltaTime;
// 			remainSpeed = 0;
// 		}
// 	}
// 	Rotation = atan2(Velocity.y, Velocity.x);
// 	Sprite::Update(deltaTime);
// }

void Enemy::Draw() const {
	Sprite::Draw();
	if (PlayScene::DebugMode) {
		// Draw collision radius.
		al_draw_circle(Position.x, Position.y, CollisionRadius, al_map_rgb(255, 0, 0), 2);
	}
	if (Shield>0) al_draw_filled_circle(Position.x, Position.y, CollisionRadius*2, al_map_rgba(0, 0, 255, 50));
}

int Enemy::get_kill_score() const
{
	return this->kill_score;
}

void Enemy::set_froze_timer(float duration)
{
	this->froze_count_down = duration;
}

void Enemy::set_pass_intermediate_point(bool pass)
{
	pass_intermediate_point = pass;
}

bool Enemy::get_pass_intermediate_point() const
{
	return pass_intermediate_point;
}

void Enemy::Freeze()
{
	FrostCount=200;
}
