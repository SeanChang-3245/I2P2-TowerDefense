#include <string>

#include "PlaneEnemy.hpp"

const int PlaneEnemy::Cost = 10;
const float PlaneEnemy::Speed = 100;
const float PlaneEnemy::HP = 10;
const int PlaneEnemy::Money = 10;
const int PlaneEnemy::KillScore = 100;

PlaneEnemy::PlaneEnemy(int x, int y) : Enemy("play/enemy-2.png", x, y, 16, Speed, HP, Money) {
	// Use bounding circle to detect collision is for simplicity, pixel-perfect collision can be implemented quite easily,
	// and efficiently if we use AABB collision detection first, and then pixel-perfect collision.
	Enemy::kill_score = KillScore;
}
