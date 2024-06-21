#ifndef TEXT_INPUT_HPP
#define TEXT_INPUT_HPP

#include <allegro5/bitmap.h>
#include <functional>
#include <memory>
#include <string>

#include "Engine/Group.hpp"
#include "Image.hpp"
#include "Label.hpp"

#define MAX_NAME_LEN 10

namespace Engine {

    class TextInputBlock : public Group
    {
    private:
		std::string filename;
        Label* default_message;
        Label* user_entered_text;
		Image* background_img;

    public:
		TextInputBlock(float x, float y, unsigned char r, unsigned char g, unsigned char b, unsigned char a, float anchorX, float anchorY);
		void OnKeyDown(int keyCode) override;
		// void OnKeyUp(int keyCode) override;
	
		void Draw() const override;
		void save_input_to_file(int score);
		void save_input_to_file(int score, std::string date);
		int get_text_length() const;
		void set_default_message(std::string str);
		std::string get_text_inputed() const;
    };
}

#endif