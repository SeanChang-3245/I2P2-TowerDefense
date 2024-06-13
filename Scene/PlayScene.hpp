#ifndef PLAYSCENE_HPP
#define PLAYSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <list>
#include <memory>
#include <utility>
#include <vector>

#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"

class Turret;
namespace Engine {
	class Group;
	class Image;
	class Label;
	class Sprite;
}  // namespace Engine

class PlayScene : public Engine::IScene {
protected:
	enum TileType {
		TILE_DIRT, // enemy can walk     
		TILE_FLOOR, // enemy can not walk 
		TILE_OCCUPIED, // tile occupied by turret
	};
	ALLEGRO_SAMPLE_ID bgmId;
	std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> deathBGMInstance;
	int lives;
	int money;
	int total_score;
	int SpeedMult;
	bool have_entered_revive_scene;
	bool entering_revive_scene;

public:
	static bool DebugMode;
	static const std::vector<Engine::Point> directions;
	static const int MapWidth, MapHeight;
	static const int BlockSize;
	static const float DangerTime;
	static const Engine::Point SpawnGridPoint;
	static const Engine::Point EndGridPoint;
	static const std::vector<int> code;
	// multiplier for enemy count
	// easy: 1.0, normal: 1.3, hard: 1.5
	float difficulty;
	float ticks;
	float deathCountDown;
	int MapId;

	// Map tiles.
	Group* TileMapGroup;
	Group* GroundEffectGroup;
	Group* DebugIndicatorGroup;
	Group* BulletGroup;
	Group* TowerGroup;
	Group* EnemyGroup;
	Group* EffectGroup;
	Group* UIGroup;
	
	Engine::Label* UIMoney;
	Engine::Label* UIScore;
	Engine::Label* UILives;
	Engine::Image* imgTarget;
	Engine::Sprite* dangerIndicator;
	std::vector<std::vector<TileType>> mapState;
	std::vector<std::vector<TileType>> originalMapState;
	// Store the distance to the end point
	std::vector<std::vector<int>> mapDistance;
	std::list<std::pair<int, float>> enemyWaveData;
	// Store previous key strokes, use to activate cheat code
	std::list<int> keyStrokes;


	// mouse indicator
	// Turret* preview;
	
// ========= Static Functions ============ // 

	static Engine::Point GetClientSize();
	
// ========= Virtual Functions ============ // 

	virtual void Initialize() override;
	virtual void Terminate() override;
	virtual void Draw() const override;
	virtual void ConstructUI();
	virtual void OnKeyDown(int keyCode) override;
	virtual void Hit();

// ========= Pure Virtual Functions ============ // 

	virtual void ActivateCheatMode() =0;
	virtual void Update(float deltaTime) override =0;
	virtual void OnMouseMove(int mx, int my) override =0;
	virtual void OnMouseUp(int button, int mx, int my) override =0;
	virtual void UIBtnClicked(int id) =0;
	virtual void OnMouseDown(int button, int mx, int my) override =0;
	// place turret at (x,y) if possible
	virtual void PlaceTurret(const int &x, const int &y) =0;
	// delete turret (x,y) and return half of its price if exist
	virtual void DeconstructTurret(const int &x, const int &y) =0;
	virtual void UpdateDangerIndicator() =0;

	// Return value show whether it is enter from revive scene
	virtual bool handle_revive() =0;


// ========= Non-Virtual Functions ============ // 

	explicit PlayScene() = default;
	void ExitOnClick();
	int GetMoney() const;
	int GetScore() const;
	void EarnMoney(int money);
	void EarnScore(int score);
	void ReadMap();
	// check whether a turret can be placed at (x,y)
	bool CheckSpaceValid(int x, int y);
	std::vector<std::vector<int>> CheckSpaceValid2(int x, int y);
	// Calculate the distance to the end point
	std::vector<std::vector<int>> CalculateBFSDistance();
	// Calculate the distance to the point (x,y)
	std::vector<std::vector<int>> CalculateBFSDistance(int x, int y);
	
};
#endif // PLAYSCENE_HPP
