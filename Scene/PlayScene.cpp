#include <allegro5/allegro.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <vector>
#include <queue>
#include <string>
#include <memory>
#include <iomanip>

#include "Engine/AudioHelper.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "Enemy/Enemy.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "UI/Component/Label.hpp"
#include "Turret/LaserTurret.hpp"
#include "Turret/MachineGunTurret.hpp"
#include "Turret/MissileTurret.hpp"
#include "Turret/AdvancedMissileTurret.hpp"
#include "UI/Animation/Plane.hpp"
#include "Enemy/PlaneEnemy.hpp"
#include "PlayScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "Enemy/SoldierEnemy.hpp"
#include "Enemy/TankEnemy.hpp"
#include "Enemy/AdvancedTankEnemy.hpp"
#include "Turret/TurretButton.hpp"
#include "Engine/LOG.hpp"
#include "Turret/HoverTurretButton.hpp"
#include "Turret/Shovel.hpp"
#include "DebugMacro.hpp"
#include "Bullet/FireBullet.hpp"
#include "UI/Component/ImageButton.hpp"

bool PlayScene::DebugMode = false;
const std::vector<Engine::Point> PlayScene::directions = {Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1)};
const int PlayScene::MapWidth = 20, PlayScene::MapHeight = 13;
const int PlayScene::BlockSize = 64;
const Engine::Point PlayScene::SpawnGridPoint = Engine::Point(-1, 0);
const Engine::Point PlayScene::EndGridPoint = Engine::Point(MapWidth, MapHeight - 1);
const float PlayScene::DangerTime = 7.61;
const int PlayScene::EnemyTypes = 4;

// code to activate cheat mode
const std::vector<int> PlayScene::code = {ALLEGRO_KEY_UP, ALLEGRO_KEY_UP, ALLEGRO_KEY_DOWN, ALLEGRO_KEY_DOWN,
										  ALLEGRO_KEY_LEFT, ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT, ALLEGRO_KEY_RIGHT,
										  ALLEGRO_KEY_B, ALLEGRO_KEY_A, ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_ENTER};

Engine::Point PlayScene::GetClientSize()
{
	return Engine::Point(MapWidth * BlockSize, MapHeight * BlockSize);
}

void PlayScene::Initialize()
{
	bool enter_from_revive = handle_revive();
	if(enter_from_revive)
		return;

	

	Engine::LOG(Engine::INFO) << "enter PlayScene::initialize\n";

	mapState.clear();
	keyStrokes.clear();
	ticks = 0;
	lives = 10;
	money = 150;
	total_score = 0;
	SpeedMult = 1;
	have_entered_revive_scene = false;
	entering_revive_scene = false;

	// a scene is a Group, so there is a vector of object in the scene
	// Add groups from bottom to top.
	AddNewObject(TileMapGroup = new Group());
	AddNewObject(GroundEffectGroup = new Group());
	AddNewObject(DebugIndicatorGroup = new Group());
	AddNewObject(TowerGroup = new Group());
	AddNewObject(EnemyGroup = new Group());
	AddNewObject(BulletGroup = new Group());
	AddNewObject(EffectGroup = new Group());
	AddNewControlObject(UIGroup = new Group());

	ConstructUI();
	
	// Read map data and initialize distance
	ReadMap();
	mapDistance = CalculateBFSDistance();

	// Preload Lose Scene
	deathBGMInstance = Engine::Resources::GetInstance().GetSampleInstance("astronomia.ogg");
	Engine::Resources::GetInstance().GetBitmap("lose/benjamin-happy.png");

	// Start BGM.
	bgmId = AudioHelper::PlayBGM("play.ogg");
}

void PlayScene::Terminate()
{
	if(entering_revive_scene)
	{
		AudioHelper::StopBGM(bgmId);
		SpeedMult = 0;
		entering_revive_scene = false;
		return;
	}

	entering_revive_scene = have_entered_revive_scene = false;
	AudioHelper::StopBGM(bgmId);
	AudioHelper::StopSample(deathBGMInstance);
	deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
	IScene::Terminate();
}

// void PlayScene::Update(float deltaTime)
// {
// 	// If we use deltaTime directly, then we might have Bullet-through-paper problem.
// 	// Reference: Bullet-Through-Paper
// 	if (SpeedMult == 0)
// 		deathCountDown = -1;
// 	else if (deathCountDown != -1)
// 		SpeedMult = 1;
// 	// Calculate danger zone.
// 	std::vector<float> reachEndTimes;
// 	for (auto &it : EnemyGroup->GetObjects())
// 	{
// 		reachEndTimes.push_back(dynamic_cast<Enemy *>(it)->reachEndTime);
// 	}
// 	// Can use Heap / Priority-Queue instead. But since we won't have too many enemies, sorting is fast enough.
// 	std::sort(reachEndTimes.begin(), reachEndTimes.end());
// 	float newDeathCountDown = -1;
// 	int danger = lives;
// 	for (auto &it : reachEndTimes)
// 	{
// 		if (it <= DangerTime)
// 		{
// 			danger--;
// 			if (danger <= 0)
// 			{
// 				// Death Countdown
// 				float pos = DangerTime - it;
// 				if (it > deathCountDown)
// 				{
// 					// Restart Death Count Down BGM.
// 					AudioHelper::StopSample(deathBGMInstance);
// 					if (SpeedMult != 0)
// 						deathBGMInstance = AudioHelper::PlaySample("astronomia.ogg", false, AudioHelper::BGMVolume, pos);
// 				}
// 				float alpha = pos / DangerTime;
// 				alpha = std::max(0, std::min(255, static_cast<int>(alpha * alpha * 255)));
// 				dangerIndicator->Tint = al_map_rgba(255, 255, 255, alpha);
// 				newDeathCountDown = it;
// 				break;
// 			}
// 		}
// 	}
// 	deathCountDown = newDeathCountDown;
// 	if (SpeedMult == 0)
// 		AudioHelper::StopSample(deathBGMInstance);
// 	if (deathCountDown == -1 && lives > 0)
// 	{
// 		AudioHelper::StopSample(deathBGMInstance);
// 		dangerIndicator->Tint.a = 0;
// 	}
// 	if (SpeedMult == 0)
// 		deathCountDown = -1;
// 	for (int i = 0; i < SpeedMult; i++)
// 	{
// 		IScene::Update(deltaTime);
// 		// Check if we should create new enemy.
// 		ticks += deltaTime;
// 		if (enemyWaveData.empty() || DIRECT_WIN)
// 		{
// 			if (EnemyGroup->GetObjects().empty() || DIRECT_WIN)
// 			{
// 				// Free resources.
// 				/*delete TileMapGroup;
// 				delete GroundEffectGroup;
// 				delete DebugIndicatorGroup;
// 				delete TowerGroup;
// 				delete EnemyGroup;
// 				delete BulletGroup;
// 				delete EffectGroup;
// 				delete UIGroup;
// 				delete imgTarget;*/
// 				Engine::GameEngine::GetInstance().ChangeScene("win");
// 			}
// 			continue;
// 		}
// 		auto current = enemyWaveData.front();
// 		if (ticks < current.second)
// 			continue;
// 		ticks -= current.second;
// 		enemyWaveData.pop_front();
// 		const Engine::Point SpawnCoordinate = Engine::Point(SpawnGridPoint.x * BlockSize + BlockSize / 2, SpawnGridPoint.y * BlockSize + BlockSize / 2);
// 		Enemy *enemy;
// 		switch (current.first)
// 		{
// 		case 1:
// 			EnemyGroup->AddNewObject(enemy = new SoldierEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
// 			break;
// 		case 2:
// 			EnemyGroup->AddNewObject(enemy = new PlaneEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
// 			break;
// 		case 3:
// 			EnemyGroup->AddNewObject(enemy = new TankEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
// 			break;
// 		case 4:
// 			EnemyGroup->AddNewObject(enemy = new AdvancedTankEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
// 			break;
// 		// TODO: [CUSTOM-ENEMY]: You need to modify 'Resource/enemy1.txt', or 'Resource/enemy2.txt' to spawn the 4th enemy.
// 		//         The format is "[EnemyId] [TimeDelay] [Repeat]".
// 		// TODO: [CUSTOM-ENEMY]: Enable the creation of the enemy.
// 		default:
// 			continue;
// 		}
// 		enemy->UpdatePath(mapDistance);
// 		// Compensate the time lost.
// 		enemy->Update(ticks);
// 	}
// 	if (preview)
// 	{
// 		preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
// 		// To keep responding when paused.
// 		preview->Update(deltaTime);
// 	}
// }

void PlayScene::Draw() const
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
}

// void PlayScene::OnMouseDown(int button, int mx, int my)
// {
// 	if ((button == 1) && !imgTarget->Visible && preview)
// 	{
// 		// Cancel turret construct.
// 		UIGroup->RemoveObject(preview->GetObjectIterator());
// 		preview = nullptr;
// 	}
// 	IScene::OnMouseDown(button, mx, my);
// }

// void PlayScene::OnMouseMove(int mx, int my)
// {
// 	IScene::OnMouseMove(mx, my);
// 	const int x = mx / BlockSize;
// 	const int y = my / BlockSize;
// 	if (!preview || x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
// 	{
// 		imgTarget->Visible = false;
// 		return;
// 	}
// 	imgTarget->Visible = true;
// 	imgTarget->Position.x = x * BlockSize;
// 	imgTarget->Position.y = y * BlockSize;
// }
// void PlayScene::OnMouseUp(int button, int mx, int my)
// {
// 	IScene::OnMouseUp(button, mx, my);
// 	if (!imgTarget->Visible)
// 		return;
// 	const int x = mx / BlockSize;
// 	const int y = my / BlockSize;
// 	// left click
// 	if (button == 1)
// 	{ 
// 		if (mapState[y][x] != TILE_OCCUPIED)
// 		{
// 			PlaceTurret(x, y);
// 		}
// 		if (mapState[y][x] == TILE_OCCUPIED)
// 		{
// 			DeconstructTurret(x, y);
// 		}
// 	}
// 	OnMouseMove(mx, my);
// }

void PlayScene::OnKeyDown(int keyCode)
{
	IScene::OnKeyDown(keyCode);
	if (keyCode == ALLEGRO_KEY_TAB)
	{
		DebugMode = !DebugMode;
	}
	else
	{
		keyStrokes.push_back(keyCode);
		if (keyStrokes.size() > code.size())
			keyStrokes.pop_front();
		if (keyCode == ALLEGRO_KEY_ENTER && keyStrokes.size() == code.size())
		{
			auto it = keyStrokes.begin();
			for (int c : code)
			{
				if (!((*it == c) ||
					  (c == ALLEGRO_KEYMOD_SHIFT &&
					   (*it == ALLEGRO_KEY_LSHIFT || *it == ALLEGRO_KEY_RSHIFT))))
					return;
				++it;
			}
			// successfully enter cheat code
			ActivateCheatMode();
			// EarnMoney(10000);
			// EffectGroup->AddNewObject(new Plane());
		}
	}
	
	if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9)
	{
		// Hotkey for Speed up.
		SpeedMult = keyCode - ALLEGRO_KEY_0;
	}
}

void PlayScene::Hit()
{
	lives--;
	UILives->Text = std::string("Life ") + std::to_string(lives);
}
int PlayScene::GetMoney() const
{
	return money;
}
int PlayScene::GetScore() const
{
	return total_score;
}

void PlayScene::EarnMoney(int money)
{
	this->money += money;
	UIMoney->Text = std::string("$") + std::to_string(this->money);
}

void PlayScene::EarnScore(int score)
{
	this->total_score += score;
	UIScore->Text = std::string("Score: ") + std::to_string(this->total_score);
}

void PlayScene::ReadMap()
{
	std::string filename = std::string("Resource/maps/map") + std::to_string(MapId) + ".txt";
	// Read map file.
	char c;
	std::vector<bool> mapData;
	std::ifstream fin(filename);
	while (fin >> c)
	{
		switch (c)
		{
		case '0':
			mapData.push_back(false);
			break;
		case '1':
			mapData.push_back(true);
			break;
		case '\n':
		case '\r':
			if (static_cast<int>(mapData.size()) / MapWidth != 0)
				throw std::ios_base::failure("Map data is corrupted1.");
			break;
		default:
			throw std::ios_base::failure("Map data is corrupted2.");
		}
	}
	fin.close();
	// Validate map data.
	if (static_cast<int>(mapData.size()) != MapWidth * MapHeight)
		throw std::ios_base::failure("Map data is corrupted3.");
	// Store map in 2d array.
	mapState = std::vector<std::vector<TileType>>(MapHeight, std::vector<TileType>(MapWidth));
	originalMapState = std::vector<std::vector<TileType>>(MapHeight, std::vector<TileType>(MapWidth));
	for (int i = 0; i < MapHeight; i++)
	{
		for (int j = 0; j < MapWidth; j++)
		{
			const bool num = mapData[i * MapWidth + j];
			mapState[i][j] = originalMapState[i][j] = num ? TILE_FLOOR : TILE_DIRT;
			if (num)
				TileMapGroup->AddNewObject(new Engine::Image("play/floor.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
			else
				TileMapGroup->AddNewObject(new Engine::Image("play/dirt.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
		}
	}
}

void PlayScene::ConstructUI()
{
	// Background
	UIGroup->AddNewObject(new Engine::Image("play/sand.png", 1280, 0, 320, 832));

	// Text
	UIGroup->AddNewObject(new Engine::Label(std::string("Stage ") + std::to_string(MapId), "pirulen.ttf", 32, 1294, 0));
	UIGroup->AddNewObject(UIMoney = new Engine::Label(std::string("$") + std::to_string(money), "pirulen.ttf", 24, 1294, 48));
	UIGroup->AddNewObject(UIScore = new Engine::Label(std::string("Score: ") + std::to_string(total_score), "pirulen.ttf", 24, 1294, 88));
	UIGroup->AddNewObject(UILives = new Engine::Label(std::string("Life ") + std::to_string(lives), "pirulen.ttf", 24, 1294, 128));

	// Exit button
	Engine::ImageButton *btn;
	btn = new Engine::ImageButton("play/dirt.png", "play/floor.png", 1310, 750, 260, 75);
	btn->SetOnClickCallback(std::bind(&PlayScene::ExitOnClick, this));
	UIGroup->AddNewControlObject(btn);
	UIGroup->AddNewObject(new Engine::Label("exit", "pirulen.ttf", 32, 1440, 750 + 75.0/2, 0, 0, 0, 255, 0.5, 0.5));

	// Danger indicator
	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	int shift = 135 + 25;
	dangerIndicator = new Engine::Sprite("play/benjamin.png", w - shift, h - shift - 100);
	dangerIndicator->Tint.a = 0;
	UIGroup->AddNewObject(dangerIndicator);

	// Initialize mouse indicator
	imgTarget = new Engine::Image("play/target.png", 0, 0);
	imgTarget->Visible = false;
	UIGroup->AddNewObject(imgTarget);
}

bool PlayScene::CheckSpaceValid(int x, int y)
{
	// a turret can be placed on TILE_DIRT or TILE_FLOOR

	if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
		return false;
	if(mapState[y][x] == TILE_OCCUPIED)
		return false;
	TileType originalState = mapState[y][x];

	// assume a turret has been placed, will it blocks enemy's path
	mapState[y][x] = TILE_OCCUPIED;
	std::vector<std::vector<int>> map = CalculateBFSDistance();
	mapState[y][x] = originalState;
	if (map[0][0] == -1) // the path from start point to end point is blocked
		return false;
	for (auto &it : EnemyGroup->GetObjects())
	{
		Engine::Point pnt;
		pnt.x = floor(it->Position.x / BlockSize);
		pnt.y = floor(it->Position.y / BlockSize);
		if (pnt.x < 0)
			pnt.x = 0;
		if (pnt.x >= MapWidth)
			pnt.x = MapWidth - 1;
		if (pnt.y < 0)
			pnt.y = 0;
		if (pnt.y >= MapHeight)
			pnt.y = MapHeight - 1;
		if (map[pnt.y][pnt.x] == -1) // the turrect is placed on a enemy or enemy is enclosed in a room
			return false;
	}
	// All enemy have path to exit, hence the turrect can be placed at (x,y)
	mapState[y][x] = TILE_OCCUPIED;
	mapDistance = map;
	for (auto &it : EnemyGroup->GetObjects())
		dynamic_cast<Enemy *>(it)->UpdatePath(mapDistance);
	return true;
}

std::vector<std::vector<int>> PlayScene::CheckSpaceValid2(int x, int y)
{
	// a turret can be placed on TILE_DIRT or TILE_FLOOR
	std::vector<std::vector<int>> ans;
	if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
		return ans;
	if(mapState[y][x] == TILE_OCCUPIED)
		return ans;
	TileType originalState = mapState[y][x];

	// assume a turret has been placed, will it blocks enemy's path
	mapState[y][x] = TILE_OCCUPIED;
	std::vector<std::vector<int>> map = CalculateBFSDistance();
	mapState[y][x] = originalState;
	if (map[0][0] == -1) // the path from start point to end point is blocked
		return ans;
	for (auto &it : EnemyGroup->GetObjects())
	{
		Engine::Point pnt;
		pnt.x = floor(it->Position.x / BlockSize);
		pnt.y = floor(it->Position.y / BlockSize);
		if (pnt.x < 0)
			pnt.x = 0;
		if (pnt.x >= MapWidth)
			pnt.x = MapWidth - 1;
		if (pnt.y < 0)
			pnt.y = 0;
		if (pnt.y >= MapHeight)
			pnt.y = MapHeight - 1;
		if (map[pnt.y][pnt.x] == -1) // the turrect is placed on a enemy or enemy is enclosed in a room
			return ans;
	}
	// All enemy have path to exit, hence the turrect can be placed at (x,y)
	// mapState[y][x] = TILE_OCCUPIED;
	ans = map;
	for (auto &it : EnemyGroup->GetObjects())
		dynamic_cast<Enemy *>(it)->UpdatePath(ans);
	return ans;
}

std::vector<std::vector<int>> PlayScene::CalculateBFSDistance()
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
		// TODO: [BFS PathFinding] (1/1): Implement a BFS starting from the most right-bottom block in the map.
		//               For each step you should assign the corresponding distance to the most right-bottom block.
		//               mapState[y][x] is TILE_DIRT if it is empty.
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

std::vector<std::vector<int>> PlayScene::CalculateBFSDistance(int x, int y)
{
	if(x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
		throw std::invalid_argument("position is not valid, call at " + std::string(__FILE__) + "::" + std::to_string(__LINE__));
	// Reverse BFS to find path.
	std::vector<std::vector<int>> map(MapHeight, std::vector<int>(std::vector<int>(MapWidth, -1)));
	std::queue<Engine::Point> que;
	// Push the point (x,y).
	// BFS from (x,y).
	if (mapState[y][x] != TILE_DIRT)
		return map;
	que.push(Engine::Point(x, y));
	map[y][x] = 0;

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

	if (PRINT_INTERMEDIATE_MAP_DISTANCE)
	{
		Engine::LOG(Engine::INFO) << "map distance information";
		for (const auto &v : map)
		{
			for (const auto &dist : v)
				std::cout << std::setw(2) << dist << ' ';
			std::cout << '\n';
		}
	}
	return map;
}

void PlayScene::ExitOnClick()
{
	Engine::LOG(Engine::INFO) << "enter ExitOnClick";
	if(deathBGMInstance == nullptr)
		Engine::LOG(Engine::INFO) << "exit on click nullptr";
	Engine::GameEngine::GetInstance().ChangeScene("lose");
}

// void PlayScene::ActivateCheatMode()
// {
// 	EarnMoney(10000);
// 	EffectGroup->AddNewObject(new Plane());
// }

// void PlayScene::UIBtnClicked(int id)
// {
// 	if (preview)
// 	{
// 		UIGroup->RemoveObject(preview->GetObjectIterator());
// 		preview = nullptr;
// 	}
// 	// TODO: [CUSTOM-TURRET]: On callback, create the turret.
// 	if (id == 0 && money >= MachineGunTurret::Price)
// 		preview = new MachineGunTurret(0, 0);
// 	else if (id == 1 && money >= LaserTurret::Price)
// 		preview = new LaserTurret(0, 0);
// 	else if (id == 2 && money >= MissileTurret::Price)
// 		preview = new MissileTurret(0, 0);
// 	else if (id == 3 && money >= AdvancedMissileTurret::Price)
// 		preview = new AdvancedMissileTurret(0, 0);
// 	else if (id == 4)
// 		preview = new Shovel(0, 0);


// 	if (!preview)
// 		return;
// 	preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
// 	preview->Tint = al_map_rgba(255, 255, 255, 200);
// 	preview->Enabled = false;
// 	preview->Preview = true;
// 	UIGroup->AddNewObject(preview);
// 	OnMouseMove(Engine::GameEngine::GetInstance().GetMousePosition().x, Engine::GameEngine::GetInstance().GetMousePosition().y);
// }


// void PlayScene::PlaceTurret(const int &x, const int &y)
// {
// 	if (!preview || preview->GetType() != TURRET)
// 		return;
// 	// Check if turret can be place at (x,y)
// 	if (!CheckSpaceValid(x, y))
// 	{
// 		Engine::Sprite *sprite;
// 		GroundEffectGroup->AddNewObject(sprite = new DirtyEffect("play/target-invalid.png", 1, x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2));
// 		sprite->Rotation = 0;
// 		return;
// 	}

// 	for(auto &it : this->TowerGroup->GetObjects())
// 	{
// 		Turret* turret = dynamic_cast<Turret*>(it);
// 		if(!it)
// 			return;
// 	}

// 	// Purchase.
// 	EarnMoney(-preview->GetPrice());
	
// 	// Remove Preview.
// 	preview->GetObjectIterator()->first = false;
// 	UIGroup->RemoveObject(preview->GetObjectIterator());
	
// 	// Construct real turret.
// 	preview->Position.x = x * BlockSize + BlockSize / 2;
// 	preview->Position.y = y * BlockSize + BlockSize / 2;
// 	preview->Enabled = true;
// 	preview->Preview = false;
// 	preview->Tint = al_map_rgba(255, 255, 255, 255);
// 	TowerGroup->AddNewObject(preview);
	
// 	// To keep responding when paused.
// 	preview->Update(0);
	
// 	// Remove Preview.
// 	preview = nullptr;

// 	mapState[y][x] = TILE_OCCUPIED;
// 	// OnMouseMove(mx, my);
// }

// void PlayScene::DeconstructTurret(const int &x, const int &y)
// {
// 	if (!preview || preview->GetType() != TOOL)
// 		return;
// 	// Check if turret can be place at (x,y)
// 	Engine::Point cur_mouse_position(x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2);

// 	Turret *target_turret = nullptr;
// 	for(auto &it : this->TowerGroup->GetObjects())
// 	{
// 		Turret* turret = dynamic_cast<Turret*>(it);
// 		if(!turret->Visible)
// 			continue;
// 		if(cur_mouse_position == turret->Position)
// 		{
// 			target_turret = turret;
// 			break;
// 		}
// 	}
	
// 	if(target_turret == nullptr)
// 		return;

// 	// Get money back.
// 	EarnMoney(target_turret->GetPrice() / 2);

// 	// Remove Preview.
// 	preview->GetObjectIterator()->first = false;
// 	UIGroup->RemoveObject(preview->GetObjectIterator());

// 	// Delete target turret.
// 	TowerGroup->RemoveObject(target_turret->GetObjectIterator());

// 	// To keep responding when paused.
// 	preview->Update(0);
	
// 	// Remove Preview.
// 	preview = nullptr;

// 	// Give Back Space
// 	mapState[y][x] = originalMapState[y][x];
// }
