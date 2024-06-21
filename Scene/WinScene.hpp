#ifndef WINSCENE_HPP
#define WINSCENE_HPP
#include <allegro5/allegro_audio.h>
#include "Engine/IScene.hpp"
#include "UI/Component/TextInput.hpp"

class WinScene final : public Engine::IScene {
private:
	float ticks;
	ALLEGRO_SAMPLE_ID bgmId;
	Engine::TextInputBlock* input_block;

public:
	explicit WinScene() = default;
	void Initialize() override;
	void Terminate() override;
	void Update(float deltaTime) override;
	void BackOnClick();
	void OnKeyDown(int keyCode) override;

	// [TODO]
	void handleUserInput();
};

#endif // WINSCENE_HPP
