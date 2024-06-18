#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>
#include <filesystem>
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "PlayScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "UI/Component/Slider.hpp"
#include "StageSelectScene.hpp"
#include "Engine/LOG.hpp"
#include "ML_Macro.hpp"

static const int RowSpacing = 200;
static const int ColSpacing = 450;


void StageSelectScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    ConstructStageButton();
    update_button_accessibility();

    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH * 3 / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&StageSelectScene::BackOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH / 2 + 250, 400, 100);
    btn->SetOnClickCallback(std::bind(&StageSelectScene::ScoreboardOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Scoreboard", "pirulen.ttf", 36, halfW, halfH / 2 + 300, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 625, halfH * 3 / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&StageSelectScene::PrevOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Prev", "pirulen.ttf", 48, halfW - 425, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW + 225, halfH * 3 / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&StageSelectScene::NextOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Next", "pirulen.ttf", 48, halfW + 425, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));


#if USE_ML
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png",halfW - 200, halfH * 3 / 2 + 70, 400, 100);
    btn->SetOnClickCallback(std::bind(&StageSelectScene::HandWriteOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Hand Write", "pirulen.ttf", 36, halfW, halfH * 3 / 2 + 120, 0, 0, 0, 255, 0.5, 0.5));   
#endif

    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
	bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
}
void StageSelectScene::Terminate() {
    Engine::LOG(Engine::INFO) << "terminate stage select scene";
	AudioHelper::StopSample(bgmInstance);
	bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
	IScene::Terminate();
}
void StageSelectScene::BackOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("start");
}
void StageSelectScene::PlayOnClick(int stage) {
    PlayScene* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play-normal"));
    scene->MapId = stage;
    scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play-reverse"));
    scene->MapId = stage;
    scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play-survival"));
    scene->MapId = stage;

    Engine::GameEngine::GetInstance().ChangeScene("difficulty-select");
}
void StageSelectScene::ScoreboardOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("scoreboard");
}

void StageSelectScene::NextOnClick() 
{
    int maxPage = (number_of_stages+5) / 6 - 1;
    current_page = std::min(current_page + 1, maxPage);
    update_button_accessibility();
}

void StageSelectScene::PrevOnClick() 
{
    current_page = std::max(current_page - 1, 0);
    update_button_accessibility();
}

void StageSelectScene::update_button_accessibility()
{
    for(int i = 0; i < number_of_stages; ++i)
    {
        stage_buttons[i].first->Visible = false;
        stage_buttons[i].first->Enabled = false;
        stage_buttons[i].second->Visible = false;
    }

    for(int i = current_page*6; i < std::min(current_page*6+6, (int)stage_buttons.size()); ++i)
    {
        stage_buttons[i].first->Visible = true;
        stage_buttons[i].first->Enabled = true;
        stage_buttons[i].second->Visible = true;
    }
}

int StageSelectScene::get_total_stages_count()
{
    auto dirIter = std::filesystem::directory_iterator("Resource/maps");

    int fileCount = std::count_if(
	    begin(dirIter),
	    end(dirIter),
	    [](auto& entry) { return entry.is_regular_file(); }
    );
    return fileCount;  
}

void StageSelectScene::ConstructStageButton()
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    current_page = 0;
    number_of_stages = get_total_stages_count();
    stage_buttons.assign(number_of_stages, {nullptr, nullptr});

    Engine::ImageButton* btn;
    Engine::Label* label;
    for(int i = 0; i < number_of_stages; ++i)
    {
        btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", 
                    halfW - 650 + (i%3) * ColSpacing, halfH / 2 - 100 + ((i/3) % 2) * RowSpacing, 
                    400, 100);
        btn->SetOnClickCallback(std::bind(&StageSelectScene::PlayOnClick, this, i+1));
        AddNewControlObject(btn);

        label = new Engine::Label(std::string("Stage ") + std::to_string(i+1), "pirulen.ttf", 48, 
                    halfW - 450 + (i%3) * ColSpacing, halfH / 2 - 50 + ((i/3) % 2) * RowSpacing, 
                    0, 0, 0, 255, 0.5, 0.5);
        AddNewObject(label);

        btn->Enabled = btn->Visible = label->Visible = false;

        stage_buttons[i].first = btn;
        stage_buttons[i].second = label;
    }
}

#if USE_ML

void StageSelectScene::HandWriteOnClick()
{
    Engine::GameEngine::GetInstance().ChangeScene("stage-select-hw");
}

#endif