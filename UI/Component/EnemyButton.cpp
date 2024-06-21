#include <functional>
#include <memory>
#include <string>

#include "Engine/Collider.hpp"
#include "Engine/GameEngine.hpp"
#include "Image.hpp"
#include "EnemyButton.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "Engine/LOG.hpp"
#include "Scene/PlayScene.hpp"

namespace Engine {
	EnemyButton::EnemyButton(std::string img, std::string imgIn, std::string enemyPic,
                int img_x, int img_y,
                int inf_x, int inf_y,
                int w, int h,
                unsigned char r, unsigned char g, unsigned char b, unsigned char a) :
        HoverImageButton(img, imgIn, img_x, img_y, inf_x, inf_y, w, h, r, g, b, a),
        enemyPic(enemyPic, img_x, img_y, w, h, 0, 0)
    {

    }

    void EnemyButton::Draw() const
    {
        HoverImageButton::Draw();
        enemyPic.Draw();
    }

    PlayScene* EnemyButton::getPlayScene() const
    {
    	return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
    }

    void EnemyButton::Update(float deltaTime)
    {
	    ImageButton::Update(deltaTime);
	    if (getPlayScene()->GetMoney() >= cost) 
        {
		    Enabled = true;
		    enemyPic.Tint = al_map_rgba(255, 255, 255, 255);
        } 
        else 
        {
            Enabled = false;
            enemyPic.Tint = al_map_rgba(0, 0, 0, 160);
        }
    }

    void EnemyButton::SetCostValue(int cost)
    {
        this->cost = cost;
    }
}

