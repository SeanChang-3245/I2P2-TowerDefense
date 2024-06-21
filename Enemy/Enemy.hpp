#ifndef ENEMY_HPP
#define ENEMY_HPP
#include <list>
#include <vector>
#include <string>
#include <deque>

#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
#include "Engine/IScene.hpp"

class Bullet;
class PlayScene;
class Turret;

class Enemy : public Engine::Sprite {
protected:

	Engine::IScene *sceneType;
	// Path to the end point
 	std::vector<Engine::Point> path;
	// Path to the intermediate point
	std::deque<Engine::Point> intermediate_path;
	// Check whether the enemy have pass the intermediate point
	bool pass_intermediate_point;

	// // The cost to spawn the enemy in reverse mode
	// const int Cost;
	// // The maximum HP the enemy can have
	// const float MaxHP;
	// const float Speed;
	// // The money the user will earn if kill the enemy in normal mode
	// const int Money;
	// const int KillScore;

	float speed;
	float hp;
	float froze_count_down;
	// in normal mode, the monye you get if you kill the enemy
	int money;
	// in normal mode, the score you get if you kill the enemy
	int kill_score;

	PlayScene* getPlayScene();
	virtual void OnExplode();
public:
	// static const int cost = 0;
	float reachEndTime;
	bool Frozen;
	std::list<Turret*> lockedTurrets;
	std::list<Bullet*> lockedBullets;

	Enemy(std::string img, float x, float y, float radius, float speed, float hp, int money);
 	void Hit(float damage);
	void UpdatePath(const std::vector<std::vector<int>>& mapDistance);
	void UpdateIntermediatePath(const std::vector<std::vector<int>>& mapDistance);
	void Update(float deltaTime) override;
	void Draw() const override;
	int get_kill_score() const;
	void set_froze_timer(float duration);
	void set_pass_intermediate_point(bool pass);
	bool get_pass_intermediate_point() const;

	// float get_max_HP() const;
	// int get_cost() const;
	// float get_speed() const;
};
#endif // ENEMY_HPP
