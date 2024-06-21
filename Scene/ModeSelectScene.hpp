#ifndef MODESELECTSCENE_HPP
#define MODESELECTSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>
#include "Engine/IScene.hpp"

class ModeSelectScene final : public Engine::IScene {
private:
    enum PlayMode
    {
        NORMAL, SURVIVAL, REVERSE
    };

public:
	explicit ModeSelectScene() = default;
	void Initialize() override;
	void Terminate() override;
	void PlayOnClick(PlayMode mode);
	void BackOnClick();
};

#endif 
