#ifndef SURVIVALPLAYSCENE_HPP
#define SURVIVALPLAYSCENE_HPP
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

class SurvivalPlayScene final : public PlayScene {
private:
    float computer_money;
    float elapsed_time;
    float enemy_spawn_cd;
    int next_enemy_type;
    float player_auto_earn_money_cd;

public:
    static const float EnemySpawnDuration; 
    static const float PlayerAutoEarnMoneyDuration;
	Turret* preview;
	
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
	virtual bool handle_revive() override final;
	virtual void PlacePotion(const int &x, const int &y) override final;
	virtual void PlaceObject(const int &x, const int &y) override final;
	virtual void PlaceMine(const int &x, const int &y) override final;

// ========= Non-Virtual Functions ============ // 	

	explicit SurvivalPlayScene() = default;
	void UpdateSpawnEnemy(float deltaTime);
	void ClearCloseEnemy();

};
#endif // PLAYSCENE_HPP
