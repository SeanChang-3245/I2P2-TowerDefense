#ifndef STAGESELECTHANDWRITESCENE_HPP
#define STAGESELECTHANDWRITESCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>
#include <vector>
#include <utility>
#include "Engine/IScene.hpp"
#include "Engine/Group.hpp"
#include "UI/Component/DrawBoard.hpp"
#include "UI/Component/Label.hpp"

class StageSelectSceneHW final : public Engine::IScene {
private:
	static const float MaxErrorMessageCD;
	int number_of_stages;
	float error_message_cd;
	DrawBoard *TensDigitBoard;
	DrawBoard *UnitsDigitBoard;
	Engine::Label *error_message;

	
	int get_total_stages_count();

public:
	explicit StageSelectSceneHW() = default;
	void Initialize() override;
	void Terminate() override;
	void PlayOnClick(int stage);
	void BackOnClick();
	void SubmitOnClick();
	void Update(float deltaTime) override;
	void OnMouseUp(int button, int mx, int my) override;
	void OnMouseDown(int button, int mx, int my) override;
	void OnMouseMove(int mx, int my) override;
	void PrintErrorMessage(int invalid_stage);
};

#endif 
