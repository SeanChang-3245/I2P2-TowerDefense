#include <functional>
#include <memory>

#include "Engine/Collider.hpp"
#include "Engine/GameEngine.hpp"
#include "Image.hpp"
#include "HoverImageButton.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "Engine/LOG.hpp"

const int ROW_SPACING = 30;


namespace Engine {
	HoverImageButton::HoverImageButton(std::string img, std::string imgIn, 
                int img_x, int img_y,
                int inf_x, int inf_y,
                unsigned char r, unsigned char g, unsigned char b, unsigned char a) :
        ImageButton(img, imgIn, img_x, img_y), 
        information_x(inf_x), information_y(inf_y), num_of_row(0),
        text_color_r(r), text_color_g(g), text_color_b(b), text_color_a(a)
    {
        informations = new Group();
        informations->Visible = false;
    }

	HoverImageButton::HoverImageButton(std::string img, std::string imgIn, 
                int img_x, int img_y,
                int inf_x, int inf_y,
                int w, int h,
                unsigned char r, unsigned char g, unsigned char b, unsigned char a) :
        ImageButton(img, imgIn, img_x, img_y, w, h), 
        information_x(inf_x), information_y(inf_y), num_of_row(0),
        text_color_r(r), text_color_g(g), text_color_b(b), text_color_a(a)
    {
        informations = new Group();
        informations->Visible = false;
    }

    void HoverImageButton::OnMouseMove(int mx, int my)
    {
		mouseIn = Collider::IsPointInBitmap(Point((mx - Position.x) * GetBitmapWidth() / Size.x + Anchor.x * GetBitmapWidth(), (my - Position.y) * GetBitmapHeight() / Size.y + Anchor.y * GetBitmapHeight()), bmp);
        
        if(mouseIn)
            informations->Visible = true;
        else
            informations->Visible = false;
        
        if (!mouseIn || !Enabled) 
            bmp = imgOut;
		else
            bmp = imgIn;  
    }

    void HoverImageButton::AddNewInformation(std::string content)
    {
        informations->AddNewObject(new Label(content, "pirulen.ttf", 20, 
                static_cast<float>(1.0 * information_x), static_cast<float>(1.0 * information_y + 1.0* num_of_row * ROW_SPACING), 
                text_color_r, text_color_g, text_color_b, text_color_a));
        num_of_row++;
    }   

    void HoverImageButton::Draw() const
    {
        ImageButton::Draw();
        if(informations->Visible)   
            informations->Draw();
    }
}
