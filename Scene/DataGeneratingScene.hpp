#ifndef DATAGENERATINGSCENE_HPP
#define DATAGENERATINGSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>
#include <vector>
#include <utility>
#include "Engine/IScene.hpp"
#include "Engine/Group.hpp"
#include "UI/Component/DrawBoard.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/TextInput.hpp"

class DataGeneratingScene final : public Engine::IScene {
private:
	static const float MaxErrorMessageCD;
	float error_message_cd;
	DrawBoard *Board;
    Engine::TextInputBlock *input_block;
	Engine::Label *error_message;

public:
	explicit DataGeneratingScene() = default;
	void Initialize() override;
	void Terminate() override;
	void SubmitOnClick();
    void ClearOnClick();
	void Update(float deltaTime) override;
	void OnMouseUp(int button, int mx, int my) override;
	void OnMouseDown(int button, int mx, int my) override;
	void OnMouseMove(int mx, int my) override;
    void OnKeyDown(int keyCode) override;
	void PrintErrorMessage();
};

#endif 
