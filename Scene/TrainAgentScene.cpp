#include <allegro5/allegro.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <vector>
#include <queue>
#include <string>
#include <memory>
#include <random>
#include <iostream>
using namespace std;

#include "Engine/AudioHelper.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "Enemy/Enemy.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/HoverImageButton.hpp"
#include "UI/Component/EnemyButton.hpp"
#include "Turret/LaserTurret.hpp"
#include "Turret/MachineGunTurret.hpp"
#include "Turret/MissileTurret.hpp"
#include "Turret/AdvancedMissileTurret.hpp"
#include "UI/Animation/Plane.hpp"
#include "Enemy/PlaneEnemy.hpp"
#include "TrainAgentScene.hpp"
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
#include "Turret/BerserkPotion.hpp"
#include "Turret/HoverTurretButton.hpp"
#include "Turret/ShieldPotion.hpp"
#include "Turret/FrostPotion.hpp"

const float TrainAgentScene::PlaceTurretDuration = 1.0;
const float TrainAgentScene::MaxTimeSpan = 100;

using Engine::LOG;
using Engine::INFO;

void TrainAgentScene::Initialize()
{
	Engine::LOG(Engine::INFO) << "enter TrainAgentScene::initialize\n";
	PlayScene::Initialize();

	// enter from revive scene
	if(have_entered_revive_scene)
		return;

	remain_time = MaxTimeSpan / difficulty;
	placeTurretCountDown = PlaceTurretDuration;
	playing_danger_bgm = false;
	cur_turret = nullptr;
	turret_pos.x = turret_pos.y = -1;
	intermediate_point.x = intermediate_point.y = -1;

	SetChooseTurretPositionFunc(std::bind(&TrainAgentScene::TurretPosision_RandomPosOnRandomPath, this));
	preview = nullptr;
}

void TrainAgentScene::Update(float deltaTime)
{
	UpdateDangerIndicator();
	for (int i = 0; i < SpeedMult; i++)
	{
		IScene::Update(deltaTime);		
		UpdateTimer(deltaTime);
		UpdatePlaceTurret(deltaTime);
	
		// Check if run out of time, if so then lose
		if(remain_time < 0)
		{
			if(have_entered_revive_scene)
			{
				remain_time = 0;
				Engine::GameEngine::GetInstance().ChangeScene("lose");
			}
			else
			{
				have_entered_revive_scene = true;
				entering_revive_scene = true;
				Engine::GameEngine::GetInstance().ChangeScene("revive");
			}
		}
	}	
	if (preview)
	{
		preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
		// To keep responding when paused.
		preview->Update(deltaTime);
	}
}

void TrainAgentScene::OnMouseDown(int button, int mx, int my)
{
	const int x = mx / BlockSize;
	const int y = my / BlockSize;
	if(button == 2)
		set_intermediate_point(x, y);
	if ((button == 1) && !imgTarget->Visible && preview)
	{
		// Cancel turret construct.
		UIGroup->RemoveObject(preview->GetObjectIterator());
		preview = nullptr;
	}
	IScene::OnMouseDown(button, mx, my);
}

void TrainAgentScene::OnMouseMove(int mx, int my)
{
	IScene::OnMouseMove(mx, my);
	const int x = mx / BlockSize;
	const int y = my / BlockSize;
	if (!preview || x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
	{
		imgTarget->Visible = false;
		return;
	}
	imgTarget->Visible = true;
	imgTarget->Position.x = x * BlockSize;
	imgTarget->Position.y = y * BlockSize;
}
void TrainAgentScene::OnMouseUp(int button, int mx, int my)
{
	IScene::OnMouseUp(button, mx, my);
	if (!imgTarget->Visible)
		return;
	const int x = mx / BlockSize;
	const int y = my / BlockSize;
	if (button == 1 && preview) PlaceObject(x, y);
	OnMouseMove(mx, my);
}
void TrainAgentScene::OnKeyDown(int keyCode)
{
	PlayScene::OnKeyDown(keyCode);
	if (keyCode == ALLEGRO_KEY_Q)
	{
		// Hotkey for Soldier.
		UIBtnClicked(0);
	}
	else if (keyCode == ALLEGRO_KEY_W)
	{
		// Hotkey for Plane.
		UIBtnClicked(1);
	}
	else if (keyCode == ALLEGRO_KEY_E)
	{
		// Hotkey for Tank.
		UIBtnClicked(2);
	}
	else if (keyCode == ALLEGRO_KEY_R)
	{
		// Hotkey for AdvancedTank.
		UIBtnClicked(3);
	}
}


void TrainAgentScene::ConstructUI()
{
	PlayScene::ConstructUI();

	// Text
	UIGroup->AddNewObject(UITime = new Engine::Label(std::string("time") + std::to_string(remain_time), "pirulen.ttf", 24, 1294, 168));
	std::vector<std::string> details;
	Engine::EnemyButton *btn;
	const int information_x = 1294;
	const int information_y = 400;

	// Soldier Enemy 
	btn = new Engine::EnemyButton("play/floor.png", "play/dirt.png", "play/enemy-1.png",
		1294, 216,
		information_x, information_y,
		64, 64, 
		0, 0, 0, 255);
	btn->SetOnClickCallback(std::bind(&TrainAgentScene::UIBtnClicked, this, 0));
	btn->AddNewInformation(std::string("Cost: ") + std::to_string(SoldierEnemy::Cost));
	btn->AddNewInformation(std::string("HP: ") + std::to_string(static_cast<int>(SoldierEnemy::HP)));
	btn->AddNewInformation(std::string("Speed: ") + std::to_string(static_cast<int>(SoldierEnemy::Speed)));
	btn->SetCostValue(SoldierEnemy::Cost);
	UIGroup->AddNewControlObject(btn);
	

	// Plane Enemy 
	btn = new Engine::EnemyButton("play/floor.png", "play/dirt.png", "play/enemy-2.png",
		1370, 216,
		information_x, information_y,
		64, 64,
		0, 0, 0, 255);
	btn->SetOnClickCallback(std::bind(&TrainAgentScene::UIBtnClicked, this, 1));
	btn->AddNewInformation(std::string("Cost: ") + std::to_string(PlaneEnemy::Cost));
	btn->AddNewInformation(std::string("HP: ") + std::to_string(static_cast<int>(PlaneEnemy::HP)));
	btn->AddNewInformation(std::string("Speed: ") + std::to_string(static_cast<int>(PlaneEnemy::Speed)));
	btn->SetCostValue(PlaneEnemy::Cost);
	UIGroup->AddNewControlObject(btn);

	// Tank Enemy 
	btn = new Engine::EnemyButton("play/floor.png", "play/dirt.png", "play/enemy-3-full.png",
		1446, 216,
		information_x, information_y,
		64, 64,
		0, 0, 0, 255);
	btn->SetOnClickCallback(std::bind(&TrainAgentScene::UIBtnClicked, this, 2));
	btn->AddNewInformation(std::string("Cost: ") + std::to_string(TankEnemy::Cost));
	btn->AddNewInformation(std::string("HP: ") + std::to_string(static_cast<int>(TankEnemy::HP)));
	btn->AddNewInformation(std::string("Speed: ") + std::to_string(static_cast<int>(TankEnemy::Speed)));
	btn->SetCostValue(TankEnemy::Cost);
	UIGroup->AddNewControlObject(btn);

	// Advanced Tank Enemy 
	btn = new Engine::EnemyButton("play/floor.png", "play/dirt.png", "play/enemy-4.png",
		1522, 216,
		information_x, information_y,
		64, 64,
		0, 0, 0, 255);
	btn->SetOnClickCallback(std::bind(&TrainAgentScene::UIBtnClicked, this, 3));
	btn->AddNewInformation(std::string("Cost: ") + std::to_string(AdvancedTankEnemy::Cost));
	btn->AddNewInformation(std::string("HP: ") + std::to_string(static_cast<int>(AdvancedTankEnemy::HP)));
	btn->AddNewInformation(std::string("Speed: ") + std::to_string(static_cast<int>(AdvancedTankEnemy::Speed)));
	btn->SetCostValue(AdvancedTankEnemy::Cost);
	UIGroup->AddNewControlObject(btn);

	HoverTurretButton *pbtn;
	details.clear();
	details.push_back("Berserk!!");
	pbtn = new HoverTurretButton("play/floor.png", "play/dirt.png",
		Engine::Sprite("play/potion.png", 1294, 292, 0, 0, 0, 0),
		Engine::Sprite("play/potion.png", 1294, 292, 0, 0, 0, 0),
		1294, 292,
		information_x, information_y,
		0, 0, 0, 255,
		BerserkPotion::Price, BerserkPotion::Range, BerserkPotion::Range,
		details);
	pbtn->SetOnClickCallback(std::bind(&TrainAgentScene::UIBtnClicked, this, 5));
	UIGroup->AddNewControlObject(pbtn);

	details.clear();
	details.push_back("Shiled Up!");
	pbtn = new HoverTurretButton("play/floor.png", "play/dirt.png",
		Engine::Sprite("play/potion.png", 1370, 292, 0, 0, 0, 0),
		Engine::Sprite("play/potion.png", 1370, 292, 0, 0, 0, 0),
		1370, 292,
		information_x, information_y,
		0, 0, 0, 255,
		ShieldPotion::Price, ShieldPotion::Range, ShieldPotion::Range,
		details);
	pbtn->SetOnClickCallback(std::bind(&TrainAgentScene::UIBtnClicked, this, 6));
	UIGroup->AddNewControlObject(pbtn);

	details.clear();
	details.push_back("freeze the enemies");
	pbtn = new HoverTurretButton("play/floor.png", "play/dirt.png",
		Engine::Sprite("play/potion.png", 1446, 292, 0, 0, 0, 0),
		Engine::Sprite("play/potion.png", 1446, 292, 0, 0, 0, 0),
		1446, 292,
		information_x, information_y,
		0, 0, 0, 255,
		FrostPotion::Price, FrostPotion::Range, FrostPotion::Range,
		details);
	pbtn->SetOnClickCallback(std::bind(&TrainAgentScene::UIBtnClicked, this, 7));
	UIGroup->AddNewControlObject(pbtn);

	// Background
	// UIGroup->AddNewObject(new Engine::Image("play/sand.png", 1280, 0, 320, 832));

	// // Text
	// UIGroup->AddNewObject(new Engine::Label(std::string("Stage ") + std::to_string(MapId), "pirulen.ttf", 32, 1294, 0));
	// UIGroup->AddNewObject(UIMoney = new Engine::Label(std::string("$") + std::to_string(money), "pirulen.ttf", 24, 1294, 48));
	// UIGroup->AddNewObject(UIScore = new Engine::Label(std::string("Score: ") + std::to_string(total_score), "pirulen.ttf", 24, 1294, 88));
	// UIGroup->AddNewObject(UILives = new Engine::Label(std::string("Life ") + std::to_string(lives), "pirulen.ttf", 24, 1294, 128));
	
	// // Exit button
	// Engine::ImageButton *btn2;
	// btn2 = new Engine::ImageButton("play/dirt.png", "play/floor.png", 1310, 750, 260, 75);
	// btn2->SetOnClickCallback(std::bind(&TrainAgentScene::ExitOnClick, this));
	// UIGroup->AddNewControlObject(btn2);
	// UIGroup->AddNewObject(new Engine::Label("exit", "pirulen.ttf", 32, 1440, 750 + 75.0/2, 0, 0, 0, 255, 0.5, 0.5));
}

void TrainAgentScene::UIBtnClicked(int id)
{
    const Engine::Point SpawnCoordinate = Engine::Point(SpawnGridPoint.x * BlockSize + BlockSize / 2, SpawnGridPoint.y * BlockSize + BlockSize / 2);
	Enemy* enemy = nullptr;
    if (preview)
	{
		UIGroup->RemoveObject(preview->GetObjectIterator());
		preview = nullptr;
	}
    int cost = 0;
    if (id == 0 && money >= SoldierEnemy::Cost)
    {
		EnemyGroup->AddNewObject(enemy = new SoldierEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
        cost = SoldierEnemy::Cost;
    }
	else if (id == 1 && money >= PlaneEnemy::Cost)
    {
        EnemyGroup->AddNewObject(enemy = new PlaneEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
        cost = PlaneEnemy::Cost;
    }
	else if (id == 2 && money >= TankEnemy::Cost)
    {
        EnemyGroup->AddNewObject(enemy = new TankEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
        cost = TankEnemy::Cost;
    }
	else if (id == 3 && money >= AdvancedTankEnemy::Cost)
    {
        EnemyGroup->AddNewObject(enemy = new AdvancedTankEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
        cost = AdvancedTankEnemy::Cost;
    }
	else if (id == 5 && money >= BerserkPotion::Price)
	{
		
		preview = new BerserkPotion(0, 0);
		cost=BerserkPotion::Price;
	}
	else if (id == 6 && money >= ShieldPotion::Price)
	{
		
		preview = new ShieldPotion(0, 0);
		cost=ShieldPotion::Price;
	}
	else if (id == 7 && money >= FrostPotion::Price)
	{
		
		preview = new FrostPotion(0, 0);
		cost=FrostPotion::Price;
	}
    if(enemy)
    {
		EarnMoney(-cost);

		// Check if there is a valid intermediate point
		if(intermediateMapDistance.size())
			enemy->UpdateIntermediatePath(intermediateMapDistance);
		enemy->UpdatePath(mapDistance);
		enemy->Update(ticks);
	}
	if (preview)
	{
		preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
		preview->Tint = al_map_rgba(255, 255, 255, 200);
		preview->Enabled = false;
		preview->Preview = true;
		UIGroup->AddNewObject(preview);
		OnMouseMove(Engine::GameEngine::GetInstance().GetMousePosition().x, Engine::GameEngine::GetInstance().GetMousePosition().y);
	}

	// check pass intermediate point
}


void TrainAgentScene::UpdateTimer(float deltaTime)
{
	remain_time -= deltaTime;

	int I = remain_time;
	float D = remain_time-I;
	std::string minute = std::to_string(I/60);
	std::string second = std::to_string(I%60);
	std::string decimal = std::to_string(D).substr(2, 2);

	if(second.size() == 1)
		second = "0" + second;

	UITime->Text = minute + ":" + second + "." + decimal;
}

void TrainAgentScene::Hit()
{
	PlayScene::Hit();
	EarnMoney(100);
	if (lives <= 0)
	{
		total_score += remain_time * 1000;
		Engine::GameEngine::GetInstance().ChangeScene("win");
	}
}

void TrainAgentScene::UpdateDangerIndicator()
{
	// If we use deltaTime directly, then we might have Bullet-through-paper problem.
	// Reference: Bullet-Through-Paper
	if(remain_time <= DangerTime)
	{
		float pos = DangerTime - remain_time;
	
		if (SpeedMult == 0)
			deathCountDown = -1;
		else 
			SpeedMult = 1;
	
		if(SpeedMult == 0)
		{
			AudioHelper::StopSample(deathBGMInstance);
			playing_danger_bgm = false;
		}
		else if(!playing_danger_bgm)
		{
			deathBGMInstance = AudioHelper::PlaySample("astronomia.ogg", false, AudioHelper::BGMVolume, pos);
			playing_danger_bgm = true;
		}
		float alpha = pos / DangerTime;
		alpha = std::max(0, std::min(255, static_cast<int>(alpha * alpha * 255)));
		dangerIndicator->Tint = al_map_rgba(255, 255, 255, alpha);
	}
	else
	{
		AudioHelper::StopSample(deathBGMInstance);
		playing_danger_bgm = false;
		dangerIndicator->Tint = al_map_rgba(255, 255, 255, 0);
	}
	
}

void TrainAgentScene::UpdatePlaceTurret(float deltaTime)
{
	placeTurretCountDown -= deltaTime;

	if(placeTurretCountDown > 0)
		return; 
	
	ChooseTurretPosition();
	ChooseTurretType();
	PlaceTurret(turret_pos.x, turret_pos.y);

	if(intermediate_point.x != -1 && intermediate_point.y != -1)
		intermediateMapDistance = CalculateBFSDistance(intermediate_point.x, intermediate_point.y);
	UpdateAllEnemyPath();

	placeTurretCountDown = PlaceTurretDuration;
}

void TrainAgentScene::ChooseTurretType()
{
	srand(time(NULL));
	int x = rand();
	switch (x % 4)
	{
	case 0:
		cur_turret = new MachineGunTurret(0, 0);
		break;
	case 1:
		cur_turret = new LaserTurret(0, 0);
		break;
	case 2:
		cur_turret = new MissileTurret(0, 0);
		break;
	case 3:
		cur_turret = new AdvancedMissileTurret(0, 0);
		break;
	default:
		break;
	}

}

void TrainAgentScene::PlaceObject(const int &x, const int &y)
{
	if (preview->GetType()==POTION) PlacePotion(x, y);
}

void TrainAgentScene::PlacePotion(const int &x, const int &y)
{
	if (!preview || preview->GetType() != POTION)
		return;
	EarnMoney(-preview->GetPrice());
	
	// Remove Preview.
	preview->GetObjectIterator()->first = false;
	UIGroup->RemoveObject(preview->GetObjectIterator());
	
	// Construct real turret.
	preview->Position.x = x * BlockSize + BlockSize / 2;
	preview->Position.y = y * BlockSize + BlockSize / 2;
	preview->Enabled = true;
	preview->Preview = false;
	preview->Tint = al_map_rgba(255, 255, 255, 255);
	TowerGroup->AddNewObject(preview);
	
	// To keep responding when paused.
	preview->Update(0);
	
	// Remove Preview.
	preview = nullptr;
}

void TrainAgentScene::PlaceTurret(const int &x, const int &y)
{
	if (!cur_turret || x == -1 || y == -1)
		return;

	for(auto &it : this->TowerGroup->GetObjects())
	{
		Turret* turret = dynamic_cast<Turret*>(it);
		if(!it)
			return;
	}

	
	// Construct real turret.
	cur_turret->Position.x = x * BlockSize + BlockSize / 2;
	cur_turret->Position.y = y * BlockSize + BlockSize / 2;
	cur_turret->Enabled = true;
	cur_turret->Preview = false;
	cur_turret->Tint = al_map_rgba(255, 255, 255, 255);
	TowerGroup->AddNewObject(cur_turret);
	
	// To keep responding when paused.
	cur_turret->Update(0);
	
	cur_turret = nullptr;

	mapState[y][x] = TILE_OCCUPIED;
}

void TrainAgentScene::SetChooseTurretPositionFunc(std::function<void(void)> selectFunc)
{
	ChooseTurretPosition = selectFunc;
}

void TrainAgentScene::TurretPosision_Random()
{
	int x, y;
	srand(time(NULL));

	int mostTry = 10;
	while(mostTry--)
	{
		x = rand();
		y = rand();
		x %= MapWidth;
		y %= MapHeight;
		if(CheckSpaceValid(x, y))
		{
			turret_pos.x = x;
			turret_pos.y = y;
			return;
		}
	}
	turret_pos.x = turret_pos.y = -1;
	LOG(INFO) << "miss one turret";
}

void TrainAgentScene::TurretPosision_RandomPosOnRandomPath()
{
	std::vector<Engine::Point> path = choose_random_path();

	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist(0, path.size() - 1);

	Engine::Point random_point = path.at(dist(rng));
	Engine::Point target_point = closet_valid_space(random_point);

	turret_pos = target_point;
	if(turret_pos.x == -1 || turret_pos.y == -1)
	{
		LOG(INFO) << "miss one turret";
	}
}

std::vector<Engine::Point> TrainAgentScene::choose_random_path()
{
	vector<Engine::Point> path;
	int x = 0;
	int y = 0;
	
	Engine::Point pos(x, y);
	int num = mapDistance[y][x];
	if (num == -1) {
		num = 0;
		Engine::LOG(Engine::ERROR) << "Enemy path finding error";
	}

	path = std::vector<Engine::Point>(num + 1);
	while (num != 0) {
		std::vector<Engine::Point> nextHops;
		for (auto& dir : PlayScene::directions) {
			int x = pos.x + dir.x;
			int y = pos.y + dir.y;
			if (x < 0 || x >= PlayScene::MapWidth || y < 0 || y >= PlayScene::MapHeight || mapDistance[y][x] != num - 1)
				continue;
			nextHops.emplace_back(x, y);
		}
		// There might be multiple shortest path to the end point
		// Choose arbitrary one.
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> dist(0, nextHops.size() - 1);
		pos = nextHops[dist(rng)];
		path[num] = pos;
		num--;
	}
	path[0] = PlayScene::EndGridPoint;
	return path;
}

Engine::Point TrainAgentScene::closet_valid_space(Engine::Point p)
{
	queue<Engine::Point> que;
	vector<vector<bool>> visit(MapHeight, vector<bool>(MapWidth, false));
	que.push(p);
	visit[p.y][p.x] = true;

	while (que.size())
	{
		Engine::Point cur_pos = que.front();
		que.pop();

		if(CheckSpaceValid(cur_pos.x, cur_pos.y))
			return cur_pos;
		
		for(const auto &dir : PlayScene::directions)
		{
			int x = cur_pos.x + dir.x;
			int y = cur_pos.y + dir.y;
			if (x < 0 || x >= PlayScene::MapWidth || y < 0 || y >= PlayScene::MapHeight || visit[y][x])
				continue;
			
			visit[y][x] = true;
			que.push(Engine::Point(x, y));
		}
		

	}
	// can't find any space
	return Engine::Point(-1, -1);
}

void TrainAgentScene::set_intermediate_point(int x, int y)
{
	// cancel intermediate_point
	if(mapState[y][x] != TILE_DIRT || mapDistance[y][x] == -1) // the tile is not valid or can't be reach
	{
		intermediate_point.x = intermediate_point.y = -1;
		intermediateMapDistance.clear();
		for(auto &it : EnemyGroup->GetObjects())
		{
			Enemy* enemy = dynamic_cast<Enemy*>(it);
			enemy->set_pass_intermediate_point(true);
		}
	}
	else
	{
		intermediate_point.x = x;
		intermediate_point.y = y;
		intermediateMapDistance = CalculateBFSDistance(x, y);
		for(auto &it : EnemyGroup->GetObjects())
		{
			Enemy* enemy = dynamic_cast<Enemy*>(it);
			enemy->UpdateIntermediatePath(intermediateMapDistance);
			enemy->set_pass_intermediate_point(false);
		}
	}

	// update enemy path
	// show icon on intermediate point
}

void TrainAgentScene::ActivateCheatMode() 
{
	EarnMoney(10000);
}

void TrainAgentScene::UpdateAllEnemyPath()
{
	for(auto &it : EnemyGroup->GetObjects())
	{
		Enemy *enemy = dynamic_cast<Enemy*>(it);
		
		if(intermediateMapDistance.size() && !enemy->get_pass_intermediate_point())
		{
			enemy->UpdateIntermediatePath(intermediateMapDistance);
		}
		enemy->UpdatePath(mapDistance);
	}
}

bool TrainAgentScene::handle_revive()
{
	if(have_entered_revive_scene)
	{
		remain_time = MaxTimeSpan / difficulty / 2;
		UpdateTimer(0);
		return true;
	}
	return false;
}

void TrainAgentScene::DeconstructTurret(const int &x, const int &y) {}


// void TrainAgentScene::Terminate()
// {
// 	AudioHelper::StopSample(deathBGMInstance);
// 	deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
// 	PlayScene::Terminate();
// }

