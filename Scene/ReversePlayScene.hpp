#ifndef REVERSEPLAYSCENE_HPP
#define REVERSEPLAYSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <list>
#include <memory>
#include <utility>
#include <vector>

#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "Scene/PlayScene.hpp"

class Turret;
namespace Engine {
	class Group;
	class Image;
	class Label;
	class Sprite;
}  // namespace Engine

class ReversePlayScene final : public PlayScene {

private:
	bool playing_danger_bgm;
	// int lives;
	// int money;
	// int total_score;
	// int SpeedMult;
public:
	// remaining time to play this round, lose if run out
	float remain_time;
	// the time interval between placing two turrets
	static const float PlaceTurretDuration;
	// remaining time to place next turret
	float placeTurretCountDown;
	Engine::Label* UITime;
	Engine::Image* imgTarget;
	// store the type of the turret, used to create a real turret
	
	
	Turret *cur_turret;
	Engine::Point turret_pos;

	
	// static bool DebugMode;
	// static const std::vector<Engine::Point> directions;
	// static const int MapWidth, MapHeight;
	// static const int BlockSize;
	// static const float DangerTime;
	// static const Engine::Point SpawnGridPoint;
	// static const Engine::Point EndGridPoint;
	// static const std::vector<int> code;
	// int MapId;
	// float difficulty;
	// float ticks;
	// float deathCountDown;
	// Engine::Sprite* dangerIndicator;
	// Map tiles.
	// Group* TileMapGroup;
	// Group* GroundEffectGroup;
	// Group* DebugIndicatorGroup;
	// Group* BulletGroup;
	// Group* TowerGroup;
	// Group* EnemyGroup;
	// Group* EffectGroup;
	// Group* UIGroup;
	// Engine::Label* UIMoney;
	// Engine::Label* UIScore;
	// Engine::Label* UILives;
	// Turret* preview;
	// std::vector<std::vector<TileType>> mapState;
	// std::vector<std::vector<TileType>> originalMapState;
	// std::vector<std::vector<int>> mapDistance;
	/// @brief store previous key strokes, use to activate cheat code
	// std::list<int> keyStrokes;

	
// ========= Virtual Functions Override ============ // 

	virtual void Initialize() override final;
	virtual void ConstructUI() override final;
	virtual void OnKeyDown(int keyCode) override final;
	virtual void Hit() override final;

// ========= Pure Virtual Functions Implement ============ // 

	virtual void UIBtnClicked(int id) override final;
	virtual void Update(float deltaTime) override final;
	virtual void OnMouseDown(int button, int mx, int my) override final;
	virtual void OnMouseMove(int mx, int my) override final;
	virtual void OnMouseUp(int button, int mx, int my) override final;
	virtual void UpdateDangerIndicator() override final;

	virtual void PlaceTurret(const int &x, const int &y) override final;
	virtual void DeconstructTurret(const int &x, const int &y) override final;

// ========= Non-Virtual Functions ============ // 	

	void UpdateTimer(float deltaTime);
	explicit ReversePlayScene() = default;
	void ChooseTurretType();
	void ChooseTurretPosition();
	void UpdatePlaceTurret(float deltaTime);



	// static Engine::Point GetClientSize();
	// void Terminate() override;
	// void Draw() const override;
	// void Hit();
	// int GetScore() const;
	// void EarnMoney(int money);
	// void EarnScore(int score);
	// void ReadMap();
	// void ExitOnClick();
	// check whether a turret can be placed at (x,y)
	// bool CheckSpaceValid(int x, int y);
	// std::vector<std::vector<int>> CalculateBFSDistance();
};
#endif // PLAYSCENE_HPP
