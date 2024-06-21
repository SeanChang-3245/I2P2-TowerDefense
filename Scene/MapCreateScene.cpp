#include <allegro5/allegro.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <vector>
#include <queue>
#include <string>
#include <memory>
#include <filesystem>
#include <fstream>

#include "Engine/AudioHelper.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/ImageButton.hpp"
#include "MapCreateScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "Engine/LOG.hpp"
#include "DebugMacro.hpp"

using namespace std;

bool MapCreateScene::DebugMode = false;
const std::vector<Engine::Point> MapCreateScene::directions = {Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1)};
const int MapCreateScene::MapWidth = 20, MapCreateScene::MapHeight = 13;
const int MapCreateScene::BlockSize = 64;
const Engine::Point MapCreateScene::SpawnGridPoint = Engine::Point(-1, 0);
const Engine::Point MapCreateScene::EndGridPoint = Engine::Point(MapWidth, MapHeight - 1);


void MapCreateScene::Initialize()
{
	mapState.clear();

    MapId = get_new_map_id();

	// Add groups from bottom to top.
	AddNewObject(TileMapGroup = new Group());
	AddNewControlObject(UIGroup = new Group());

    init_map_data();
	mapDistance = CalculateBFSDistance();

	ConstructUI();
	
    ErrorLabel = new Engine::Label("invalid map",  "pirulen.ttf", 48, 640, 416, 255, 0, 0, 255, 0.5, 0.5);
    ErrorLabel->Visible = false;
	AddNewObject(ErrorLabel);
    // UIGroup->AddNewObject(ErrorLabel);
}

void MapCreateScene::Terminate()
{
	IScene::Terminate();
}

void MapCreateScene::Update(float deltaTime)
{
    if(error_message_count_down > 0)
    {
        error_message_count_down -= deltaTime;
        if(error_message_count_down < 0)
            error_message_count_down = 0;
        ErrorLabel->Color.a = (unsigned char)(255 - error_message_count_down * 255);
    }
    else
    {
        error_message_count_down = 0;
        ErrorLabel->Visible = false;
    }
    IScene::Update(deltaTime);
}


void MapCreateScene::Draw() const
{
	IScene::Draw();
	if (DebugMode)
	{
		// Draw reverse BFS distance on all reachable blocks.
		for (int i = 0; i < MapHeight; i++)
		{
			for (int j = 0; j < MapWidth; j++)
			{
				if (mapDistance[i][j] != -1)
				{
					// Not elegant nor efficient, but it's quite enough for debugging.
					Engine::Label label(std::to_string(mapDistance[i][j]), "pirulen.ttf", 32, (j + 0.5) * BlockSize, (i + 0.5) * BlockSize);
					label.Anchor = Engine::Point(0.5, 0.5);
					label.Draw();
				}
			}
		}
	}
	// make label show upon the distance
	if(ErrorLabel->Visible)
		ErrorLabel->Draw();
}

void MapCreateScene::OnMouseDown(int button, int mx, int my)
{
	if (button == 1)
        left_button_down = true;
    if (button == 2)
        right_button_down = true;
	IScene::OnMouseDown(button, mx, my);
    OnMouseMove(mx, my);
}
void MapCreateScene::OnMouseMove(int mx, int my)
{
	IScene::OnMouseMove(mx, my);
	const int x = mx / BlockSize;
	const int y = my / BlockSize;

	if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
	{
		imgTarget->Visible = false;
		return;
	}

    if(left_button_down && mapState[y][x] != TILE_FLOOR)
    {
        place_floor_block(x, y);
		// update new map distance
        mapDistance = CalculateBFSDistance();
    }
    else if(right_button_down && mapState[y][x] != TILE_DIRT)
    {
        place_dirt_block(x, y);
		// update new map distance
        mapDistance = CalculateBFSDistance();
    }

	imgTarget->Visible = true;
	imgTarget->Position.x = x * BlockSize;
	imgTarget->Position.y = y * BlockSize;
}
void MapCreateScene::OnMouseUp(int button, int mx, int my)
{
	IScene::OnMouseUp(button, mx, my);
	
    if (button == 1)
        left_button_down = false;
    if (button == 2)
        right_button_down = false;

	if (!imgTarget->Visible)
		return;
	OnMouseMove(mx, my);
}
void MapCreateScene::OnKeyDown(int keyCode)
{
	IScene::OnKeyDown(keyCode);
	if (keyCode == ALLEGRO_KEY_TAB)
	{
		DebugMode = !DebugMode;
	}
	
}
void MapCreateScene::ConstructUI()
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;


	// UI section background
	UIGroup->AddNewObject(new Engine::Image("play/sand.png", 1280, 0, 320, 832));

	// Text
	UIGroup->AddNewObject(new Engine::Label(std::string("Stage ") + std::to_string(MapId), "pirulen.ttf", 32, 1294, 0));

    // Save button
    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", w - 235 , h - 75, 150, 50);
    btn->SetOnClickCallback(std::bind(&MapCreateScene::SaveOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Save", "pirulen.ttf", 30, w - 160 , h - 50, 0, 0, 0, 255, 0.5, 0.5));

	// Back button
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", w - 235 , h - 150, 150, 50);
    btn->SetOnClickCallback(std::bind(&MapCreateScene::BackOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("back", "pirulen.ttf", 30, w - 160 , h - 125, 0, 0, 0, 255, 0.5, 0.5));

    // mouse indicator
    imgTarget = new Engine::Image("play/target.png", 0, 0);
	imgTarget->Visible = false;
	UIGroup->AddNewObject(imgTarget);
}

std::vector<std::vector<int>> MapCreateScene::CalculateBFSDistance()
{
	// Reverse BFS to find path.
	std::vector<std::vector<int>> map(MapHeight, std::vector<int>(std::vector<int>(MapWidth, -1)));
	std::queue<Engine::Point> que;
	// Push end point.
	// BFS from end point.
	if (mapState[MapHeight - 1][MapWidth - 1] != TILE_DIRT)
		return map;
	que.push(Engine::Point(MapWidth - 1, MapHeight - 1));
	map[MapHeight - 1][MapWidth - 1] = 0;

	int cur_dist = 0;
	while (!que.empty())
	{
		int s = que.size();
		while (s--)
		{
			Engine::Point p = que.front();
			que.pop();

			for (const auto &dir : directions)
			{
				int nextX = (dir + p).x;
				int nextY = (dir + p).y;

				if (nextX < 0 || nextX >= MapWidth ||
					nextY < 0 || nextY >= MapHeight ||
					mapState[nextY][nextX] == TILE_FLOOR ||
					mapState[nextY][nextX] == TILE_OCCUPIED ||
					map[nextY][nextX] != -1) // the tile is floor but its shortest distance has been updated
					continue;

				map[nextY][nextX] = cur_dist + 1;
				que.emplace(Engine::Point(nextX, nextY));
			}
		}
		++cur_dist;
	}

	if (PRINT_MAP_DISTANCE)
	{
		Engine::LOG(Engine::INFO) << "map distance information";
		for (const auto &v : map)
		{
			for (const auto &dist : v)
				std::cout << dist << ' ';
			std::cout << '\n';
		}
	}

	return map;
}

int MapCreateScene::get_new_map_id() const
{
    auto dirIter = std::filesystem::directory_iterator("Resource/maps");

    int fileCount = std::count_if(
	    begin(dirIter),
	    end(dirIter),
	    [](auto& entry) { return entry.is_regular_file(); }
    );

    return fileCount+1;
}

void MapCreateScene::init_map_data()
{
    mapState.assign(MapHeight, std::vector<TileType>(MapWidth, TILE_DIRT));
    tile_pointers.assign(MapHeight, std::vector<Engine::Image*>(MapWidth, nullptr));

    for (int i = 0; i < MapHeight; i++)
	{
		for (int j = 0; j < MapWidth; j++)
		{
            tile_pointers[i][j] = new Engine::Image("play/dirt.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize);
            TileMapGroup->AddNewObject(tile_pointers[i][j]);
		}
	}
}

void MapCreateScene::place_dirt_block(int x, int y)
{
    mapState[y][x] = TILE_DIRT;

    TileMapGroup->RemoveObject(tile_pointers[y][x]->GetObjectIterator());
    tile_pointers[y][x] = new Engine::Image("play/dirt.png", x * BlockSize, y * BlockSize, BlockSize, BlockSize);
    TileMapGroup->AddNewObject(tile_pointers[y][x]);
}

void MapCreateScene::place_floor_block(int x, int y)
{
    mapState[y][x] = TILE_FLOOR;

    TileMapGroup->RemoveObject(tile_pointers[y][x]->GetObjectIterator());
    tile_pointers[y][x] = new Engine::Image("play/floor.png", x * BlockSize, y * BlockSize, BlockSize, BlockSize);
    TileMapGroup->AddNewObject(tile_pointers[y][x]);
}

void MapCreateScene::SaveOnClick()
{
    if(check_map_valid())
    {
        save_map_to_file();
        Engine::GameEngine::GetInstance().ChangeScene("start");
    }
    else
    {
        error_message_count_down = 1;
        ErrorLabel->Visible = true;
    }
}

void MapCreateScene::BackOnClick()
{
    Engine::GameEngine::GetInstance().ChangeScene("start");
}

void MapCreateScene::save_map_to_file()
{
    ofstream fout;

    fout.open(string("../Resource/maps/map") + to_string(MapId) + string(".txt"), ios_base::app);
    for(int i = 0; i < MapHeight; ++i)
    {
        for(int j = 0; j < MapWidth; ++j)
            fout << mapState[i][j];
        fout << '\n';
    }
    fout.close();

    fout.open(string("Resource/maps/map") + to_string(MapId) + string(".txt"), ios_base::app);
    for(int i = 0; i < MapHeight; ++i)
    {
        for(int j = 0; j < MapWidth; ++j)
            fout << mapState[i][j];
        fout << '\n';
    }
    fout.close();
}

bool MapCreateScene::check_map_valid()
{
    return mapDistance[SpawnGridPoint.y][SpawnGridPoint.y] != -1;
}