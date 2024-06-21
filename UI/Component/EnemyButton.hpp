#ifndef ENEMYBUTTON_HPP
#define ENEMYBUTTON_HPP

#include "UI/Component/HoverImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "Engine/Group.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Sprite.hpp"
#include <list>
#include <string>


namespace Engine 
{
	/// <summary>
	/// A clickable button, changes image and show detail when mouse hover.
	/// </summary>
    class EnemyButton : public Engine::HoverImageButton
    {
    protected:
        // the backgroud image when showing detail
        // Image *background_img;
        // store all the detail Label 
        Engine::Sprite enemyPic;
        int cost;

    public:
		explicit EnemyButton(std::string img, std::string imgIn, std::string enemyPic,
                int  img_x, int  img_y,
                int  inf_x , int  inf_y,
                int w, int h,
                unsigned char r, unsigned char g, unsigned char b, unsigned char a);
        void Draw() const override;
        PlayScene* getPlayScene() const;
        void Update(float deltaTime) override final;
        void SetCostValue(int cost);
    };


}


#endif