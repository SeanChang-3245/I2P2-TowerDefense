#ifndef BlackPLAYSCENE_HPP
#define BlackPLAYSCENE_HPP
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

class BlackPlayScene final : public PlayScene {
private:
	// std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> deathBGMInstance;

public:
	Turret* preview;
	// static bool DebugMode;
	// static const std::vector<Engine::Point> directions;
	// static const int MapWidth, MapHeight;
	// static const int BlockSize;
	// static const float DangerTime;
	// static const Engine::Point SpawnGridPoint;
	// static const Engine::Point EndGridPoint;
	// static const std::vector<int> code;
	// int MapId;
	// multiplier for enemy count
	// easy: 1.0, normal: 1.3, hard: 1.5
	// float difficulty;
	// float ticks;
	// float deathCountDown;
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
	// Engine::Image* imgTarget;
	// Engine::Sprite* dangerIndicator;
	// std::vector<std::vector<TileType>> mapState;
	// std::vector<std::vector<TileType>> originalMapState;
	// std::vector<std::vector<int>> mapDistance;
	// std::list<std::pair<int, float>> enemyWaveData;
	/// @brief store previous key strokes, use to activate cheat code
	// std::list<int> keyStrokes;


// ========= Virtual Functions Override ============ //

	virtual void Initialize() override final;
	virtual void ConstructUI() override final;
	virtual void Hit() override final;

// ========= Pure Virtual Functions Implement ============ //

	virtual void UIBtnClicked(int id) override final;
	virtual void Update(float deltaTime) override final;
	virtual void OnMouseDown(int button, int mx, int my) override final;
	virtual void OnMouseMove(int mx, int my) override final;
	virtual void OnMouseUp(int button, int mx, int my) override final;
	virtual void OnKeyDown(int keyCode) override final;
	// place turret at (x,y) if possible
	virtual void PlaceTurret(const int &x, const int &y) override final;
	// delete turret (x,y) and return half of its price if exist
	virtual void DeconstructTurret(const int &x, const int &y) override final;
	virtual void UpdateDangerIndicator() override final;
	virtual void ActivateCheatMode() override final;

// ========= Non-Virtual Functions ============ //

	explicit BlackPlayScene() = default;
	void ReadEnemyWave();
	void UpdateSpawnEnemy(float deltaTime);

	// static Engine::Point GetClientSize();
	// virtual void Draw() const override;
	// void Hit();
	// int GetMoney() const;
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
