#include <allegro5/allegro.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <vector>
#include <queue>
#include <string>
#include <memory>

#include "Engine/Point.hpp"
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
#include "NormalPlayScene.hpp"
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
#include "Turret/Potion.hpp"
#include "Turret/FrostPotion.hpp"
#include "Turret/BerserkPotion.hpp"


void NormalPlayScene::Initialize()
{
	Engine::LOG(Engine::INFO) << "enter NormalPlayScene::initialize\n";
	PlayScene::Initialize();

	// enter from revive scene
	if(have_entered_revive_scene)
	{
		ClearCloseEnemy();
		return;
	}

	deathCountDown = -1;
	ReadEnemyWave();
	preview = nullptr;
}

void NormalPlayScene::Update(float deltaTime)
{
	UpdateDangerIndicator();
	for (int i = 0; i < SpeedMult; i++)
	{
		IScene::Update(deltaTime);
		UpdateSpawnEnemy(deltaTime);
	}
	if (preview)
	{
		preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
		// To keep responding when paused.
		preview->Update(deltaTime);
	}
}

void NormalPlayScene::OnMouseDown(int button, int mx, int my)
{
	if ((button == 1) && !imgTarget->Visible && preview)
	{
		// Cancel turret construct.
		UIGroup->RemoveObject(preview->GetObjectIterator());
		preview = nullptr;
	}
	IScene::OnMouseDown(button, mx, my);
}

void NormalPlayScene::OnMouseMove(int mx, int my)
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

void NormalPlayScene::PlaceObject(const int &x, const int &y)
{
	if (preview->GetType()==TURRET) PlaceTurret(x, y);
	else if (preview->GetType()==POTION) PlacePotion(x, y);
}

void NormalPlayScene::OnMouseUp(int button, int mx, int my)
{
	IScene::OnMouseUp(button, mx, my);
	if (!imgTarget->Visible)
		return;
	const int x = mx / BlockSize;
	const int y = my / BlockSize;
	// left click
	if (button == 1)
	{ 
		if (mapState[y][x] != TILE_OCCUPIED || preview->GetType()==POTION)
		{
			PlaceObject(x, y);
		}
		if (mapState[y][x] == TILE_OCCUPIED)
		{
			DeconstructTurret(x, y);
			mapDistance = CalculateBFSDistance();
		}
	}
	OnMouseMove(mx, my);
}

void NormalPlayScene::OnKeyDown(int keyCode)
{
	PlayScene::OnKeyDown(keyCode);
	if (keyCode == ALLEGRO_KEY_Q)
	{
		// Hotkey for MachineGunTurret.
		UIBtnClicked(0);
	}
	else if (keyCode == ALLEGRO_KEY_W)
	{
		// Hotkey for LaserTurret.
		UIBtnClicked(1);
	}
	else if (keyCode == ALLEGRO_KEY_E)
	{
		// Hotkey for MissileTurret.
		UIBtnClicked(2);
	}
	else if (keyCode == ALLEGRO_KEY_R)
	{
		// Hotkey for AdvancedMissileTurret.
		UIBtnClicked(3);
	}
	else if (keyCode == ALLEGRO_KEY_T)
	{
		// Hotkey for Shovel.
		UIBtnClicked(4);
	}
}

void NormalPlayScene::ReadEnemyWave()
{
	// if the map is user defined, then use the same enemy file as Stage 2
	std::string filename = std::string("Resource/enemy") + std::to_string(std::min(MapId, 2)) + ".txt";
	Engine::LOG(Engine::INFO) << "Loaded Resource<text>: " << filename;

	// Read enemy file.
	int type, repeat;
	float wait;
	enemyWaveData.clear();
	std::ifstream fin(filename);
	while (fin >> type && fin >> wait && fin >> repeat)
	{
		for (int i = 0; i < 1.0 * repeat * difficulty; i++)
			enemyWaveData.emplace_back(type, wait);
	}
	fin.close();
}
void NormalPlayScene::ConstructUI()
{
	PlayScene::ConstructUI();

	std::vector<std::string> details;
	HoverTurretButton *btn;
	const int information_x = 1294;
	const int information_y = 400;

	// Turret 1
	btn = new HoverTurretButton("play/floor.png", "play/dirt.png",
		Engine::Sprite("play/tower-base.png", 1294, 176, 0, 0, 0, 0),
		Engine::Sprite("play/turret-1.png", 1294, 176 - 8, 0, 0, 0, 0),
		1294, 176,
		information_x, information_y,
		0, 0, 0, 255,
		MachineGunTurret::Price, MachineGunTurret::Range, MachineGunTurret::Damage, MachineGunTurret::Reload);
	btn->SetOnClickCallback(std::bind(&NormalPlayScene::UIBtnClicked, this, 0));
	UIGroup->AddNewControlObject(btn);

	// Turret 2
	btn = new HoverTurretButton("play/floor.png", "play/dirt.png",
		Engine::Sprite("play/tower-base.png", 1370, 176, 0, 0, 0, 0),
		Engine::Sprite("play/turret-2.png", 1370, 176 - 8, 0, 0, 0, 0),
		1370, 176,
		information_x, information_y,
		0, 0, 0, 255,
		LaserTurret::Price, LaserTurret::Range, LaserTurret::Damage, LaserTurret::Reload);
	btn->SetOnClickCallback(std::bind(&NormalPlayScene::UIBtnClicked, this, 1));
	UIGroup->AddNewControlObject(btn);

	// Turret 3
	btn = new HoverTurretButton("play/floor.png", "play/dirt.png",
		Engine::Sprite("play/tower-base.png", 1446, 176, 0, 0, 0, 0),
		Engine::Sprite("play/turret-3.png", 1446, 176, 0, 0, 0, 0),
		1446, 176,
		information_x, information_y,
		0, 0, 0, 255,
		MissileTurret::Price, MissileTurret::Range, MissileTurret::Damage, MissileTurret::Reload);
	btn->SetOnClickCallback(std::bind(&NormalPlayScene::UIBtnClicked, this, 2));
	UIGroup->AddNewControlObject(btn);

	// Turret 4
	btn = new HoverTurretButton("play/floor.png", "play/dirt.png",
		Engine::Sprite("play/tower-base.png", 1522, 176, 0, 0, 0, 0),
		Engine::Sprite("play/turret-6.png", 1522, 176, 0, 0, 0, 0),
		1522, 176,
		information_x, information_y,
		0, 0, 0, 255,
		AdvancedMissileTurret::Price, AdvancedMissileTurret::Range, AdvancedMissileTurret::Damage, AdvancedMissileTurret::Reload);
	btn->SetOnClickCallback(std::bind(&NormalPlayScene::UIBtnClicked, this, 3));
	UIGroup->AddNewControlObject(btn);

	// Tool 1
	details.clear();
	details.push_back("return half price");
	btn = new HoverTurretButton("play/floor.png", "play/dirt.png",
		Engine::Sprite("play/shovel.png", 1294, 252, 0, 0, 0, 0),
		Engine::Sprite("play/shovel.png", 1294, 252, 0, 0, 0, 0),
		1294, 252,
		information_x, information_y,
		0, 0, 0, 255,
		details);
	btn->SetOnClickCallback(std::bind(&NormalPlayScene::UIBtnClicked, this, 4));
	UIGroup->AddNewControlObject(btn);

	details.clear();
	details.push_back("freeze the enemies");
	btn = new HoverTurretButton("play/floor.png", "play/dirt.png",
		Engine::Sprite("play/potion.png", 1370, 252, 0, 0, 0, 0),
		Engine::Sprite("play/potion.png", 1370, 252, 0, 0, 0, 0),
		1370, 252,
		information_x, information_y,
		0, 0, 0, 255,
		FrostPotion::Price, FrostPotion::Range, FrostPotion::Range,
		details);
	btn->SetOnClickCallback(std::bind(&NormalPlayScene::UIBtnClicked, this, 5));
	UIGroup->AddNewControlObject(btn);

	details.clear();
	details.push_back("freeze the enemies");
	btn = new HoverTurretButton("play/floor.png", "play/dirt.png",
		Engine::Sprite("play/potion.png", 1370, 252, 0, 0, 0, 0),
		Engine::Sprite("play/potion.png", 1370, 252, 0, 0, 0, 0),
		1370, 252,
		information_x, information_y,
		0, 0, 0, 255,
		FrostPotion::Price, FrostPotion::Range, FrostPotion::Range,
		details);
	btn->SetOnClickCallback(std::bind(&NormalPlayScene::UIBtnClicked, this, 5));
	UIGroup->AddNewControlObject(btn);

	details.clear();
	details.push_back("Berserk!!");
	btn = new HoverTurretButton("play/floor.png", "play/dirt.png",
		Engine::Sprite("play/potion.png", 1444, 252, 0, 0, 0, 0),
		Engine::Sprite("play/potion.png", 1444, 252, 0, 0, 0, 0),
		1444, 252,
		information_x, information_y,
		0, 0, 0, 255,
		BerserkPotion::Price, BerserkPotion::Range, BerserkPotion::Range,
		details);
	btn->SetOnClickCallback(std::bind(&NormalPlayScene::UIBtnClicked, this, 6));
	UIGroup->AddNewControlObject(btn);
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
	// btn2->SetOnClickCallback(std::bind(&NormalPlayScene::ExitOnClick, this));
	// UIGroup->AddNewControlObject(btn2);
	// UIGroup->AddNewObject(new Engine::Label("exit", "pirulen.ttf", 32, 1440, 750 + 75.0/2, 0, 0, 0, 255, 0.5, 0.5));

	// Danger indicator
	// int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	// int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	// int shift = 135 + 25;
	// dangerIndicator = new Engine::Sprite("play/benjamin.png", w - shift, h - shift - 100);
	// dangerIndicator->Tint.a = 0;
	// UIGroup->AddNewObject(dangerIndicator);
}

void NormalPlayScene::UIBtnClicked(int id)
{
	if (preview)
	{
		UIGroup->RemoveObject(preview->GetObjectIterator());
		preview = nullptr;
	}
	// TODO: [CUSTOM-TURRET]: On callback, create the turret.
	if (id == 0 && money >= MachineGunTurret::Price)
		preview = new MachineGunTurret(0, 0);
	else if (id == 1 && money >= LaserTurret::Price)
		preview = new LaserTurret(0, 0);
	else if (id == 2 && money >= MissileTurret::Price)
		preview = new MissileTurret(0, 0);
	else if (id == 3 && money >= AdvancedMissileTurret::Price)
		preview = new AdvancedMissileTurret(0, 0);
	else if (id == 4)
		preview = new Shovel(0, 0);
	else if (id == 5 && money >= FrostPotion::Price)
		preview = new FrostPotion(0, 0);
	else if (id == 6 && money >= BerserkPotion::Price)
		preview = new BerserkPotion(0, 0);


	if (!preview)
		return;
	preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
	preview->Tint = al_map_rgba(255, 255, 255, 200);
	preview->Enabled = false;
	preview->Preview = true;
	UIGroup->AddNewObject(preview);
	OnMouseMove(Engine::GameEngine::GetInstance().GetMousePosition().x, Engine::GameEngine::GetInstance().GetMousePosition().y);
}

void NormalPlayScene::PlaceTurret(const int &x, const int &y)
{
	if (!preview || preview->GetType() != TURRET)
		return;
	// Check if turret can be place at (x,y)
	if (!CheckSpaceValid(x, y))
	{
		Engine::Sprite *sprite;
		GroundEffectGroup->AddNewObject(sprite = new DirtyEffect("play/target-invalid.png", 1, x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2));
		sprite->Rotation = 0;
		return;
	}

	for(auto &it : this->TowerGroup->GetObjects())
	{
		Turret* turret = dynamic_cast<Turret*>(it);
		if(!it)
			return;
	}

	// Purchase.
	// mapState[y][x] = TILE_OCCUPIED;
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
	TowerGroup->AddNewControlObject(preview);
	// To keep responding when paused.
	preview->Update(0);
	
	// Remove Preview.
	preview = nullptr;

	mapState[y][x] = TILE_OCCUPIED;
	// OnMouseMove(mx, my);
}

 void NormalPlayScene::PlacePotion(const int &x, const int &y)
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
	TowerGroup->AddNewControlObject(preview);

	// To keep responding when paused.
	preview->Update(0);
	
	// Remove Preview.
	preview = nullptr;

	// mapState[y][x] = TILE_OCCUPIED;
	// OnMouseMove(mx, my);
	
 }

void NormalPlayScene::DeconstructTurret(const int &x, const int &y)
{
	if (!preview || preview->GetType() != TOOL)
		return;
	// Check if turret can be place at (x,y)
	Engine::Point cur_mouse_position(x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2);

	Turret *target_turret = nullptr;
	for(auto &it : this->TowerGroup->GetObjects())
	{
		Turret* turret = dynamic_cast<Turret*>(it);
		if(!turret->Visible)
			continue;
		if(cur_mouse_position == turret->Position)
		{
			target_turret = turret;
			break;
		}
	}
	
	if(target_turret == nullptr)
		return;

	// Get money back.
	EarnMoney(target_turret->GetPrice() / 2);

	// Remove Preview.
	preview->GetObjectIterator()->first = false;
	UIGroup->RemoveObject(preview->GetObjectIterator());

	// Delete target turret.
	TowerGroup->RemoveControlObject(target_turret->GetControlIterator(), target_turret->GetObjectIterator());
	// To keep responding when paused.
	preview->Update(0);
	
	// Remove Preview.
	preview = nullptr;

	// Give Back Space
	mapState[y][x] = originalMapState[y][x];
}

void NormalPlayScene::Hit()
{
	PlayScene::Hit();
	if (lives <= 0)
	{
		if(have_entered_revive_scene)
			Engine::GameEngine::GetInstance().ChangeScene("lose");
		else
		{
			have_entered_revive_scene = true;
			entering_revive_scene = true;
			Engine::GameEngine::GetInstance().ChangeScene("revive");
		}
	}
}

void NormalPlayScene::UpdateDangerIndicator()
{
	// If we use deltaTime directly, then we might have Bullet-through-paper problem.
	// Reference: Bullet-Through-Paper
	if (SpeedMult == 0)
		deathCountDown = -1;
	else if (deathCountDown != -1)
		SpeedMult = 1;
	// Calculate danger zone.
	std::vector<float> reachEndTimes;
	for (auto &it : EnemyGroup->GetObjects())
	{
		reachEndTimes.push_back(dynamic_cast<Enemy *>(it)->reachEndTime);
	}
	// Can use Heap / Priority-Queue instead. But since we won't have too many enemies, sorting is fast enough.
	std::sort(reachEndTimes.begin(), reachEndTimes.end());
	float newDeathCountDown = -1;
	int danger = lives;
	for (auto &it : reachEndTimes)
	{
		if (it <= DangerTime)
		{
			danger--;
			if (danger <= 0)
			{
				// Death Countdown
				float pos = DangerTime - it;
				if (it > deathCountDown)
				{
					// Restart Death Count Down BGM.
					AudioHelper::StopSample(deathBGMInstance);
					if (SpeedMult != 0)
						deathBGMInstance = AudioHelper::PlaySample("astronomia.ogg", false, AudioHelper::BGMVolume, pos);
				}
				float alpha = pos / DangerTime;
				alpha = std::max(0, std::min(255, static_cast<int>(alpha * alpha * 255)));
				dangerIndicator->Tint = al_map_rgba(255, 255, 255, alpha);
				newDeathCountDown = it;
				break;
			}
		}
	}
	deathCountDown = newDeathCountDown;
	if (SpeedMult == 0)
		AudioHelper::StopSample(deathBGMInstance);
	if (deathCountDown == -1 && lives > 0)
	{
		AudioHelper::StopSample(deathBGMInstance);
		dangerIndicator->Tint.a = 0;
	}
	if (SpeedMult == 0)
		deathCountDown = -1;
}

void NormalPlayScene::UpdateSpawnEnemy(float deltaTime)
{
	// Check if we should create new enemy.
	ticks += deltaTime;
	if (enemyWaveData.empty() || DIRECT_WIN)
	{
		if (EnemyGroup->GetObjects().empty() || DIRECT_WIN)
		{
			// Free resources.
			/*delete TileMapGroup;
			delete GroundEffectGroup;
			delete DebugIndicatorGroup;
			delete TowerGroup;
			delete EnemyGroup;
			delete BulletGroup;
			delete EffectGroup;
			delete UIGroup;
			delete imgTarget;*/
			Engine::GameEngine::GetInstance().ChangeScene("win");
		}
		return;
	}
	auto current = enemyWaveData.front();
	if (ticks < current.second)
		return;
	ticks -= current.second;
	enemyWaveData.pop_front();
	const Engine::Point SpawnCoordinate = Engine::Point(SpawnGridPoint.x * BlockSize + BlockSize / 2, SpawnGridPoint.y * BlockSize + BlockSize / 2);
	Enemy *enemy = nullptr;
	switch (current.first)
	{
	case 1:
		EnemyGroup->AddNewObject(enemy = new SoldierEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
		break;
	case 2:
		EnemyGroup->AddNewObject(enemy = new PlaneEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
		break;
	case 3:
		EnemyGroup->AddNewObject(enemy = new TankEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
		break;
	case 4:
		EnemyGroup->AddNewObject(enemy = new AdvancedTankEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
		break;
	// TODO: [CUSTOM-ENEMY]: You need to modify 'Resource/enemy1.txt', or 'Resource/enemy2.txt' to spawn the 4th enemy.
	//         The format is "[EnemyId] [TimeDelay] [Repeat]".
	// TODO: [CUSTOM-ENEMY]: Enable the creation of the enemy.
	default:
		return;;
	}
	enemy->UpdatePath(mapDistance);
	// Compensate the time lost.
	enemy->Update(ticks);
}

void NormalPlayScene::ActivateCheatMode()
{
	EarnMoney(10000);
	EffectGroup->AddNewObject(new Plane());
}

bool NormalPlayScene::handle_revive()
{
	if(have_entered_revive_scene)
	{
		lives = 5;
		UILives->Text = std::string("Life ") + std::to_string(lives);
		return true;
	}
	return false;
}

void NormalPlayScene::ClearCloseEnemy()
{
	PlayScene *play_scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
	for(auto ptr : play_scene->EnemyGroup->GetObjects())
	{
		Enemy* enemy = dynamic_cast<Enemy*>(ptr);
		Engine::Point pos = enemy->Position;
		const int x = pos.x / PlayScene::BlockSize;
		const int y = pos.y / PlayScene::BlockSize;

		if(play_scene->mapDistance[y][x] <= 10)
			enemy->Hit(INFINITY);
	}
}