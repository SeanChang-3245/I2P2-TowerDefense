#ifndef DIFFICULTYSELECTSCENE_HPP
#define DIFFICULTYSELECTSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>
#include "Engine/IScene.hpp"

class DifficultySelectScene final : public Engine::IScene {
private:
	std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
public:
	explicit DifficultySelectScene() = default;
	void Initialize() override;
	void Terminate() override;
	void PlayOnClick(float difficulty);
	void BackOnClick();
};

#endif 
