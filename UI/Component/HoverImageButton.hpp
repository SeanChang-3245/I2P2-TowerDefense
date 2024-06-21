#ifndef HOVERIMAGEBUTTON_HPP
#define HOVERIMAGEBUTTON_HPP

#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "Engine/Group.hpp"
#include <list>
#include <string>


namespace Engine 
{
	/// <summary>
	/// A clickable button, changes image and show detail when mouse hover.
	/// </summary>
    class HoverImageButton : public Engine::ImageButton
    {
    protected:
        // the backgroud image when showing detail
        // Image *background_img;
        // store all the detail Label 
        Group *informations;
        int num_of_row;
        int information_x;
        int information_y;
        unsigned char text_color_r;
        unsigned char text_color_g;
        unsigned char text_color_b;
        unsigned char text_color_a;

    public:
		explicit HoverImageButton(std::string img, std::string imgIn, 
                int  img_x, int  img_y,
                int  inf_x , int  inf_y,
                unsigned char r, unsigned char g, unsigned char b, unsigned char a);
                
		explicit HoverImageButton(std::string img, std::string imgIn, 
                int  img_x, int  img_y,
                int  inf_x , int  inf_y,
                int w, int h,
                unsigned char r, unsigned char g, unsigned char b, unsigned char a);

        void OnMouseMove(int mx, int my) override;
        void AddNewInformation(std::string content);
        void Draw() const override;

    };


}


#endif