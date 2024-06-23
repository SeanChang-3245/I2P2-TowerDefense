#include <allegro5/color.h>
#include <allegro5/allegro_primitives.h>
#include <cmath>
#include <utility>

#include "Engine/IScene.hpp"
#include "Enemy/Enemy.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IObject.hpp"
#include "Engine/IScene.hpp"
#include "Scene/PlayScene.hpp"
#include "Scene/NormalPlayScene.hpp"
#include "Engine/Point.hpp"
#include "Turret.hpp"
#include "Engine/Collider.hpp"
#include "UI/Component/ImageButton.hpp"

PlayScene* Turret::getPlayScene() {
	return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}

Turret::Turret(std::string imgBase, std::string imgTurret, float x, float y, float radius, int price, float coolDown, Turret_Type _type) :
	Sprite(imgTurret, x, y), price(price), coolDown(coolDown), imgBase(imgBase, x, y), type(_type)

{
	CollisionRadius = radius;
	Freeze=0;
	Berserker=0;	
	mouseIn=0;
	MenuVisible=0;
}
#include <iostream>
void Turret::Update(float deltaTime) {
	Sprite::Update(deltaTime);
	PlayScene* scene = getPlayScene();
	if (Freeze) return;
	imgBase.Position = Position;
	imgBase.Tint = Tint;
	if (!Enabled)
		return;
	if (Target) {
		Engine::Point diff = Target->Position - Position;
		if (diff.Magnitude() > CollisionRadius) {
			Target->lockedTurrets.erase(lockedTurretIterator);
			Target = nullptr;
			lockedTurretIterator = std::list<Turret*>::iterator();
		}
	}
	if (!Target) {
		// Lock first seen target.
		// Can be improved by Spatial Hash, Quad Tree, ...
		// However simply loop through all enemies is enough for this program.
		for (auto& it : scene->EnemyGroup->GetObjects()) {
			Engine::Point diff = it->Position - Position;
			if (diff.Magnitude() <= CollisionRadius) {
				Target = dynamic_cast<Enemy*>(it);
				Target->lockedTurrets.push_back(this);
				lockedTurretIterator = std::prev(Target->lockedTurrets.end());
				break;
			}
		}
	}

	// make the turret points to the target enemy
	// if (Target) {
	// 	Engine::Point originRotation = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
	// 	Engine::Point targetRotation = (Target->Position - Position).Normalize();
	// 	float maxRotateRadian = rotateRadian * deltaTime;
	// 	float cosTheta = originRotation.Dot(targetRotation);
	// 	// Might have floating-point precision error.
	// 	if (cosTheta > 1) cosTheta = 1;
	// 	else if (cosTheta < -1) cosTheta = -1;
	// 	float radian = acos(cosTheta);
	// 	Engine::Point rotation;
	// 	if (abs(radian) <= maxRotateRadian)
	// 		rotation = targetRotation;
	// 	else
	// 		rotation = ((abs(radian) - maxRotateRadian) * originRotation + maxRotateRadian * targetRotation) / radian;
	// 	// Add 90 degrees (PI/2 radian), since we assume the image is oriented upward.
	// 	Rotation = atan2(rotation.y, rotation.x) + ALLEGRO_PI / 2;
	// 	// Shoot reload.
	// 	reload -= deltaTime;
	// 	if (reload <= 0) {
	// 		// shoot.
	// 		reload = coolDown;
	// 		CreateBullet();
	// 	}
	// }

	// make the turret points to the target enemy with leading
	if (Target) {
		Engine::Point originRotation = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
		
		float estimatedTime = (Target->Position - this->Position).Magnitude() / bullet_speed;
		Engine::Point predictedPosistion = Target->Position + estimatedTime * Target->Velocity;

		Engine::Point targetRotation = (predictedPosistion - this->Position).Normalize();
		float maxRotateRadian = rotateRadian * deltaTime;
		float cosTheta = originRotation.Dot(targetRotation);
		// Might have floating-point precision error.
		if (cosTheta > 1) cosTheta = 1;
		else if (cosTheta < -1) cosTheta = -1;
		float radian = acos(cosTheta);
		Engine::Point rotation;
		if (abs(radian) <= maxRotateRadian)
			rotation = targetRotation;
		else
			rotation = ((abs(radian) - maxRotateRadian) * originRotation + maxRotateRadian * targetRotation) / radian;
		// Add 90 degrees (PI/2 radian), since we assume the image is oriented upward.
		Rotation = atan2(rotation.y, rotation.x) + ALLEGRO_PI / 2;
		// Shoot reload.
		reload -= deltaTime+(Berserker==1)*deltaTime;
		if (reload <= 0) {
			// shoot.
			reload = coolDown;
			CreateBullet();
		}
	}
	if (Reloadbtn)
	{
		if (scene->GetMoney()>=25) Reloadbtn->Enabled=1;
		else Reloadbtn->Enabled=0;
	}
	if (Rangebtn)
	{
		if (scene->GetMoney()>=25) Rangebtn->Enabled=1;
		else Rangebtn->Enabled=0;
	}
}
void Turret::Draw() const {
	if (Preview) {
		if (type==TURRET) al_draw_filled_circle(Position.x, Position.y, CollisionRadius, al_map_rgba(0, 255, 0, 50));
		else if (type==POTION) al_draw_filled_circle(Position.x, Position.y, CollisionRadius, al_map_rgba(0, 255, 0, 50));
	}
	imgBase.Draw();
	Sprite::Draw();
	if (PlayScene::DebugMode) {
		// Draw target radius.
		al_draw_circle(Position.x, Position.y, CollisionRadius, al_map_rgb(0, 0, 255), 2);
	}
	if (type==POTION && Enabled) al_draw_filled_circle(Position.x, Position.y, CollisionRadius, al_map_rgba(255, 0, 0, 50));
}
int Turret::GetPrice() const {
	return price;
}

Turret_Type Turret::GetType() const{
	return type;
}

void Turret::UIBtnClicked()
{

}

void Turret::OnMouseMove(int mx, int my)
{
	PlayScene* scene = getPlayScene();
	int BlockSize=scene->BlockSize;
	Engine::Point diff = Position - Engine::Point(mx, my);
	mouseIn = diff.Magnitude()<15;
}

void Turret::OnMouseDown(int button, int mx, int my) {
	NormalPlayScene* scene = dynamic_cast<NormalPlayScene*>(getPlayScene());
	if (scene->preview) return ;
	if ((button == 1) && mouseIn) {
		if (!MenuVisible)
		{
			ShowMenu();
			MenuVisible=1;
		}
		else 
		{
			DestroyMenu();
			MenuVisible=0;
		}
	}
	else if (!mouseIn && MenuVisible && !(Reloadbtn->GetMouseIn() || Rangebtn->GetMouseIn()))
	{
		DestroyMenu();
		MenuVisible=0;
	}
}

void Turret::TurretClicked()
{
	if(!MenuVisible) DestroyMenu();
	else ShowMenu();
	MenuVisible=!MenuVisible;
}

void Turret::ShowMenu()
{
	PlayScene* scene = getPlayScene();
	int BlockSize=scene->BlockSize;
	// Atkbtn = new Engine::ImageButton("play/AtkUp.png", "play/AtkUp_Hovered.png", Position.x + BlockSize/2, Position.y + BlockSize/2, scene->BlockSize, scene->BlockSize);
	// Atkbtn->SetOnClickCallback(std::bind(&Turret::AtkUpClick, this));
	// scene->UIGroup->AddNewControlObject(Atkbtn);
	Rangebtn = new Engine::ImageButton("play/RangeUp.png", "play/RangeUp_Hovered.png", Position.x + BlockSize/2, Position.y + BlockSize/2, scene->BlockSize, scene->BlockSize);
	Rangebtn->SetOnClickCallback(std::bind(&Turret::RangeUpClick, this));
	Rangebtn->Enabled=1;
	scene->UIGroup->AddNewControlObject(Rangebtn);
	Reloadbtn = new Engine::ImageButton("play/ReloadUp.png", "play/ReloadUp_Hovered.png", Position.x - BlockSize/2*3, Position.y + BlockSize/2, scene->BlockSize, scene->BlockSize);
	Reloadbtn->Enabled=1;
	Reloadbtn->SetOnClickCallback(std::bind(&Turret::ReloadUpClick, this));
	scene->UIGroup->AddNewControlObject(Reloadbtn);
}

void Turret::DestroyMenu()
{
	PlayScene* scene = getPlayScene();
	// if (Atkbtn) scene->UIGroup->RemoveControlObject(Atkbtn->GetControlIterator(), Atkbtn->GetObjectIterator());
	if (Rangebtn)
	{
		scene->UIGroup->RemoveControlObject(Rangebtn->GetControlIterator(), Rangebtn->GetObjectIterator());
		Rangebtn=nullptr;
	}
	if (Reloadbtn)
	{
		scene->UIGroup->RemoveControlObject(Reloadbtn->GetControlIterator(), Reloadbtn->GetObjectIterator());
		Reloadbtn=nullptr;
	}
}

void Turret::ReloadUpClick()
{
	PlayScene* scene = getPlayScene();
	coolDown=coolDown/3*2;
	scene->EarnMoney(-1*25);
}

void Turret::RangeUpClick()
{
	PlayScene* scene = getPlayScene();
	CollisionRadius=CollisionRadius/2*3;
	scene->EarnMoney(-1*25);
}