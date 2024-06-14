#include <allegro5/allegro.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <vector>
#include <queue>
#include <string>
#include <memory>

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
#include "SurvivalPlayScene.hpp"
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

const float SurvivalPlayScene::EnemySpawnDuration = 0.5;
const float SurvivalPlayScene::PlayerAutoEarnMoneyDuration = 2;

void SurvivalPlayScene::Initialize()
{
	Engine::LOG(Engine::INFO) << "enter SurvivalPlayScene::initialize\n";
	PlayScene::Initialize();

	// enter from revive scene
	if(have_entered_revive_scene)
	{
		ClearCloseEnemy();
		return;
	}

    money = 300;
    computer_money = 15;
    elapsed_time = 0;
    enemy_spawn_cd = EnemySpawnDuration;
    player_auto_earn_money_cd = PlayerAutoEarnMoneyDuration;
    next_enemy_type = rand() % EnemyTypes + 1;


	deathCountDown = -1;
	preview = nullptr;
}

void SurvivalPlayScene::Update(float deltaTime)
{
	UpdateDangerIndicator();

	
	for (int i = 0; i < SpeedMult; i++)
	{
        player_auto_earn_money_cd -= deltaTime;
        if(player_auto_earn_money_cd <= 0)
        {
            EarnMoney(5);
            player_auto_earn_money_cd = PlayerAutoEarnMoneyDuration;
        }

        computer_money += (elapsed_time / 800 * difficulty);
        elapsed_time += deltaTime;
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

void SurvivalPlayScene::OnMouseDown(int button, int mx, int my)
{
	if ((button == 1) && !imgTarget->Visible && preview)
	{
		// Cancel turret construct.
		UIGroup->RemoveObject(preview->GetObjectIterator());
		preview = nullptr;
	}
	IScene::OnMouseDown(button, mx, my);
}

void SurvivalPlayScene::OnMouseMove(int mx, int my)
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

void SurvivalPlayScene::OnMouseUp(int button, int mx, int my)
{
	IScene::OnMouseUp(button, mx, my);
	if (!imgTarget->Visible)
		return;
	const int x = mx / BlockSize;
	const int y = my / BlockSize;
	// left click
	if (button == 1)
	{ 
		if (mapState[y][x] != TILE_OCCUPIED)
		{
			PlaceTurret(x, y);
		}
		if (mapState[y][x] == TILE_OCCUPIED)
		{
			DeconstructTurret(x, y);
			mapDistance = CalculateBFSDistance();
		}
	}
	OnMouseMove(mx, my);
}

void SurvivalPlayScene::OnKeyDown(int keyCode)
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

void SurvivalPlayScene::ConstructUI()
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
	btn->SetOnClickCallback(std::bind(&SurvivalPlayScene::UIBtnClicked, this, 0));
	UIGroup->AddNewControlObject(btn);

	// Turret 2
	btn = new HoverTurretButton("play/floor.png", "play/dirt.png",
		Engine::Sprite("play/tower-base.png", 1370, 176, 0, 0, 0, 0),
		Engine::Sprite("play/turret-2.png", 1370, 176 - 8, 0, 0, 0, 0),
		1370, 176,
		information_x, information_y,
		0, 0, 0, 255,
		LaserTurret::Price, LaserTurret::Range, LaserTurret::Damage, LaserTurret::Reload);
	btn->SetOnClickCallback(std::bind(&SurvivalPlayScene::UIBtnClicked, this, 1));
	UIGroup->AddNewControlObject(btn);

	// Turret 3
	btn = new HoverTurretButton("play/floor.png", "play/dirt.png",
		Engine::Sprite("play/tower-base.png", 1446, 176, 0, 0, 0, 0),
		Engine::Sprite("play/turret-3.png", 1446, 176, 0, 0, 0, 0),
		1446, 176,
		information_x, information_y,
		0, 0, 0, 255,
		MissileTurret::Price, MissileTurret::Range, MissileTurret::Damage, MissileTurret::Reload);
	btn->SetOnClickCallback(std::bind(&SurvivalPlayScene::UIBtnClicked, this, 2));
	UIGroup->AddNewControlObject(btn);

	// Turret 4
	btn = new HoverTurretButton("play/floor.png", "play/dirt.png",
		Engine::Sprite("play/tower-base.png", 1522, 176, 0, 0, 0, 0),
		Engine::Sprite("play/turret-6.png", 1522, 176, 0, 0, 0, 0),
		1522, 176,
		information_x, information_y,
		0, 0, 0, 255,
		AdvancedMissileTurret::Price, AdvancedMissileTurret::Range, AdvancedMissileTurret::Damage, AdvancedMissileTurret::Reload);
	btn->SetOnClickCallback(std::bind(&SurvivalPlayScene::UIBtnClicked, this, 3));
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
	btn->SetOnClickCallback(std::bind(&SurvivalPlayScene::UIBtnClicked, this, 4));
	UIGroup->AddNewControlObject(btn);
}

void SurvivalPlayScene::UIBtnClicked(int id)
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


	if (!preview)
		return;
	preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
	preview->Tint = al_map_rgba(255, 255, 255, 200);
	preview->Enabled = false;
	preview->Preview = true;
	UIGroup->AddNewObject(preview);
	OnMouseMove(Engine::GameEngine::GetInstance().GetMousePosition().x, Engine::GameEngine::GetInstance().GetMousePosition().y);
}

void SurvivalPlayScene::PlaceTurret(const int &x, const int &y)
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
	TowerGroup->AddNewObject(preview);
	
	// To keep responding when paused.
	preview->Update(0);
	
	// Remove Preview.
	preview = nullptr;

	mapState[y][x] = TILE_OCCUPIED;
	// OnMouseMove(mx, my);
}

void SurvivalPlayScene::DeconstructTurret(const int &x, const int &y)
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
	TowerGroup->RemoveObject(target_turret->GetObjectIterator());

	// To keep responding when paused.
	preview->Update(0);
	
	// Remove Preview.
	preview = nullptr;

	// Give Back Space
	mapState[y][x] = originalMapState[y][x];
}

void SurvivalPlayScene::Hit()
{
	PlayScene::Hit();
	if (lives <= 0)
	{
		if(have_entered_revive_scene)
			Engine::GameEngine::GetInstance().ChangeScene("win");
		else
		{
			have_entered_revive_scene = true;
			entering_revive_scene = true;
			Engine::GameEngine::GetInstance().ChangeScene("revive");
		}
	}
}

void SurvivalPlayScene::UpdateDangerIndicator()
{
	// Engine::LOG(Engine::INFO) << "alpha: " << dangerIndicator->Tint.a;
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

void SurvivalPlayScene::UpdateSpawnEnemy(float deltaTime)
{
	ticks += deltaTime;
    // Engine::LOG(Engine::INFO) << "computer money: " << computer_money;
	// Check if we should create new enemy.

    enemy_spawn_cd -= deltaTime;
	if (ticks < EnemySpawnDuration)
		return;

    if(enemy_spawn_cd > 0)
        return;

    enemy_spawn_cd = EnemySpawnDuration;
	ticks -= EnemySpawnDuration;

	const Engine::Point SpawnCoordinate = Engine::Point(SpawnGridPoint.x * BlockSize + BlockSize / 2, SpawnGridPoint.y * BlockSize + BlockSize / 2);
	Enemy *enemy = nullptr;

    if(next_enemy_type == 1 && computer_money >= SoldierEnemy::Cost)
    {
		EnemyGroup->AddNewObject(enemy = new SoldierEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
        computer_money -= SoldierEnemy::Cost;
    }
    else if(next_enemy_type == 2 && computer_money >= PlaneEnemy::Cost)
    {
		EnemyGroup->AddNewObject(enemy = new PlaneEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
        computer_money -= PlaneEnemy::Cost;
    }
    else if(next_enemy_type == 3 && computer_money >= TankEnemy::Cost)
	{
        EnemyGroup->AddNewObject(enemy = new TankEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
        computer_money -= TankEnemy::Cost;
    }	
    else if(next_enemy_type == 4 && computer_money >= AdvancedTankEnemy::Cost)
	{
        EnemyGroup->AddNewObject(enemy = new AdvancedTankEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
        computer_money -= AdvancedTankEnemy::Cost;
    }	

    if(enemy)
    {
        srand(time(NULL));
        next_enemy_type = rand() % EnemyTypes + 1;
        enemy->UpdatePath(mapDistance);
        // Compensate the time lost.
        enemy->Update(ticks);
    }
}

void SurvivalPlayScene::ActivateCheatMode()
{
	EarnMoney(10000);
	EffectGroup->AddNewObject(new Plane());
}

bool SurvivalPlayScene::handle_revive()
{
	if(have_entered_revive_scene)
	{
		lives = 5;
		UILives->Text = std::string("Life ") + std::to_string(lives);
		return true;
	}
	return false;
}

void SurvivalPlayScene::ClearCloseEnemy()
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