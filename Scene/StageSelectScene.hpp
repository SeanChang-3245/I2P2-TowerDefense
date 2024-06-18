#ifndef STAGESELECTSCENE_HPP
#define STAGESELECTSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>
#include <vector>
#include <utility>
#include "Engine/IScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "ML_Macro.hpp"

class StageSelectScene final : public Engine::IScene {
private:
	// store the pointer of each stage's entry button and its label
	// use to manipulate visibility and accessibility when switching pages
	std::vector<std::pair<Engine::ImageButton*, Engine::Label*>> stage_buttons;
	int number_of_stages;
	// one page contain 6 stages
	// start from 0
	int current_page;
	std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
	
	void ConstructStageButton();
	int get_total_stages_count();
	// update button based on current page
	void update_button_accessibility();

public:
	explicit StageSelectScene() = default;
	void Initialize() override;
	void Terminate() override;
	void PlayOnClick(int stage);
    void ScoreboardOnClick();
	void BackOnClick();
	void PrevOnClick();
	void NextOnClick();

#if USE_ML
	void HandWriteOnClick();
#endif

};

#endif // STAGESELECTSCENE_HPP
