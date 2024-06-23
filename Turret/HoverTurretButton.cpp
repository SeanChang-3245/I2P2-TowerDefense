#include <allegro5/color.h>
#include <string>

#include "Engine/GameEngine.hpp"
#include "Engine/IScene.hpp"
#include "Scene/PlayScene.hpp"
#include "HoverTurretButton.hpp"
#include "Engine/LOG.hpp"


PlayScene* HoverTurretButton::getPlayScene() {
	return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}

HoverTurretButton::HoverTurretButton(std::string img, std::string imgIn, Engine::Sprite Base, Engine::Sprite Turret, 
			int  img_x, int img_y,
			int inf_x, int inf_y,
			unsigned char r, unsigned char g, unsigned char b, unsigned char a, 
			int cost, int range, int damage, float reload) :
		cost(cost), Base(Base), Turret(Turret),
		HoverImageButton(img, imgIn, img_x, img_y, inf_x, inf_y, r, g, b, a)
{
	AddNewInformation("Cost: " + std::to_string(cost));
	AddNewInformation("Range: " + std::to_string(range));
	AddNewInformation("Damage: " + std::to_string(damage));
	AddNewInformation("Reload: " + std::to_string(reload).substr(0, 3));
}

HoverTurretButton::HoverTurretButton(std::string img, std::string imgIn, Engine::Sprite Base, Engine::Sprite Turret, 
            int img_x, int img_y,
            int inf_x, int inf_y,
            unsigned char r, unsigned char g, unsigned char b, unsigned char a, 
            int cost, int radius, int duration, 
			std::vector<std::string> details) :
			cost(cost), Base(Base), Turret(Turret),
			HoverImageButton(img, imgIn, img_x, img_y, inf_x, inf_y, r, g, b, a)

{
	for(const auto &str : details)
	{
		AddNewInformation(str);
	}
}

HoverTurretButton::HoverTurretButton(std::string img, std::string imgIn, Engine::Sprite Base, Engine::Sprite Turret, 
			int  img_x, int img_y,
			int inf_x, int inf_y,
			unsigned char r, unsigned char g, unsigned char b, unsigned char a, 
			int cost,
			std::vector<std::string> details) :
		Base(Base), Turret(Turret), cost(cost),
		HoverImageButton(img, imgIn, img_x, img_y, inf_x, inf_y, r, g, b, a)
{
	for(const auto &str : details)
	{
		AddNewInformation(str);
	}
}


void HoverTurretButton::Update(float deltaTime) {
	HoverImageButton::Update(deltaTime);
	if (getPlayScene()->GetMoney() >= cost) {
		Enabled = true;
		Base.Tint = Turret.Tint = al_map_rgba(255, 255, 255, 255);
	} else {
		Enabled = false;
		Base.Tint = Turret.Tint = al_map_rgba(0, 0, 0, 160);
	}
}

void HoverTurretButton::Draw() const {
	HoverImageButton::Draw();
	Base.Draw();
	Turret.Draw();
}
