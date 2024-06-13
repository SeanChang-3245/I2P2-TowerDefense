#ifndef REVERSEPLAYSCENE_HPP
#define REVERSEPLAYSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <list>
#include <memory>
#include <utility>
#include <vector>
#include <functional>

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

class ReversePlayScene final : public PlayScene 
{
private:

	bool playing_danger_bgm;
	Engine::Point intermediate_point;

	// Store the distacne to the intermediate point, empty if there is no intermediate point
	std::vector<std::vector<int>> intermediateMapDistance;

	// int lives;
	// int money;
	// int total_score;
	// int SpeedMult;

	// Choose a random valid path from spawn to end
	std::vector<Engine::Point> choose_random_path();
	// Given a point p, search for the nearest point that can place turret
	Engine::Point closet_valid_space(Engine::Point p);

public:
	static const float MaxTimeSpan;
	// remaining time to play this round, lose if run out
	float remain_time;
	// the time interval between placing two turrets
	static const float PlaceTurretDuration;
	// remaining time to place next turret
	float placeTurretCountDown;
	Engine::Label* UITime;
	// Engine::Image* imgTarget;
	// store the type of the turret, used to create a real turret
	
	
	Turret *cur_turret;
	Engine::Point turret_pos;
	// The function that decide where the program should put turret
	std::function<void()> ChooseTurretPosition;

	
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
	virtual void ActivateCheatMode() override final;
	virtual bool handle_revive() override final;  

	virtual void PlaceTurret(const int &x, const int &y) override final;
	virtual void DeconstructTurret(const int &x, const int &y) override final;

// ========= Non-Virtual Functions ============ // 	

	void UpdateTimer(float deltaTime);
	explicit ReversePlayScene() = default;
	void ChooseTurretType();
	// Check if need to add a new turret, if yes, then add
	void UpdatePlaceTurret(float deltaTime);
	// After placing new turret, update all enemies' path and intermediate_path
	void UpdateAllEnemyPath();
	void SetChooseTurretPositionFunc(std::function<void(void)> selectFunc);
	void set_intermediate_point(int x, int y);

// ========= Script Functions ============ // 	

	void TurretPosision_Random();
	void TurretPosision_RandomPosOnRandomPath();	

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
