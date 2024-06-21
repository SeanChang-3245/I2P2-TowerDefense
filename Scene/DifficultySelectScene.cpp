#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "PlayScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "UI/Component/Slider.hpp"
#include "DifficultySelectScene.hpp"
#include "Engine/LOG.hpp"

static const float Easy = 1.0; 
static const float Normal = 1.2; 
static const float Hard = 1.5; 

void DifficultySelectScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    Engine::ImageButton* btn;
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH * 3 / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&DifficultySelectScene::BackOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&DifficultySelectScene::PlayOnClick, this, Easy));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("easy", "pirulen.ttf", 48, halfW, halfH / 2, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH /2 + 100, 400, 100);
    btn->SetOnClickCallback(std::bind(&DifficultySelectScene::PlayOnClick, this, Normal));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("normal", "pirulen.ttf", 48, halfW, halfH / 2 + 150, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH /2 + 250, 400, 100);
    btn->SetOnClickCallback(std::bind(&DifficultySelectScene::PlayOnClick, this, Hard));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("hard", "pirulen.ttf", 48, halfW, halfH / 2 + 300, 0, 0, 0, 255, 0.5, 0.5));

    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
	bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
}
void DifficultySelectScene::Terminate() {
    Engine::LOG(Engine::INFO) << "terminate difficulty select scene";
	AudioHelper::StopSample(bgmInstance);
	bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
	IScene::Terminate();
}
void DifficultySelectScene::BackOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}
void DifficultySelectScene::PlayOnClick(float difficulty) {
    PlayScene* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play-normal"));
    scene->difficulty = difficulty;
    scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play-reverse"));
    scene->difficulty = difficulty;
    scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play-survival"));
    scene->difficulty = difficulty;
    
    // Engine::GameEngine::GetInstance().ChangeScene("play-normal");
    Engine::GameEngine::GetInstance().ChangeScene("mode-select");
}