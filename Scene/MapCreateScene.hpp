#ifndef MAPCREATESCENE_HPP
#define MAPCREATESCENE_HPP
#include <allegro5/allegro_audio.h>
#include <list>
#include <memory>
#include <utility>
#include <vector>

#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"

namespace Engine {
	class Group;
	class Image;
	class Label;
	class Sprite;
}  // namespace Engine

class MapCreateScene final : public Engine::IScene {
private:
	enum TileType {
		TILE_DIRT, // enemy can walk     
		TILE_FLOOR, // enemy can not walk 
		TILE_OCCUPIED, // tile occupied by turret
	};

public:
	static bool DebugMode;
	static const std::vector<Engine::Point> directions;
	static const int MapWidth, MapHeight;
	static const int BlockSize;
	static const Engine::Point SpawnGridPoint;
	static const Engine::Point EndGridPoint;

    // true when user hold mouse left button
    bool left_button_down;    
    // true when user hold mouse right button
    bool right_button_down;    
	int MapId;
    // record the remain time the error message need to show
    float error_message_count_down;
	// Map tiles.
    Engine::Label* ErrorLabel; 
	Group* TileMapGroup;
	Group* DebugIndicatorGroup;
	Group* UIGroup;
	Engine::Image* imgTarget;

    std::vector<std::vector<Engine::Image*>> tile_pointers;
	std::vector<std::vector<TileType>> mapState;
	std::vector<std::vector<int>> mapDistance;
	
	explicit MapCreateScene() = default;
	void Initialize() override;
	void Terminate() override;
	void Update(float deltaTime) override;
	void Draw() const override;
	void OnMouseDown(int button, int mx, int my) override;
	void OnMouseMove(int mx, int my) override;
	void OnMouseUp(int button, int mx, int my) override;
	void OnKeyDown(int keyCode) override;
    void SaveOnClick();
    void BackOnClick();
	void ConstructUI();
    
    void place_dirt_block(int x, int y);
    void place_floor_block(int x, int y);
    void init_map_data();
    int get_new_map_id() const;
    bool check_map_valid();
    void save_map_to_file();



	std::vector<std::vector<int>> CalculateBFSDistance();
};
#endif // PLAYSCENE_HPP
