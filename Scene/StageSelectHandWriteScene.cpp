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
#include "StageSelectHandWriteScene.hpp"
#include "Engine/LOG.hpp"
#include "UI/Component/DrawBoard.hpp"
#include "ML_Macro.hpp"

#if USE_ML
static const int RowSpacing = 200;
static const int ColSpacing = 450;
const float StageSelectSceneHW::MaxErrorMessageCD = 1.5;

void StageSelectSceneHW::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;


    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 420, halfH * 3 / 2 + 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&StageSelectSceneHW::BackOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW - 220, halfH * 3 / 2 + 100, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW + 20, halfH * 3 / 2 + 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&StageSelectSceneHW::SubmitOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Submit", "pirulen.ttf", 48, halfW + 220, halfH * 3 / 2 + 100, 0, 0, 0, 255, 0.5, 0.5));

    TensDigitBoard = new DrawBoard(halfW - 580, 50);
    AddNewObject(TensDigitBoard);
    UnitsDigitBoard = new DrawBoard(halfW + 20, 50);
    AddNewObject(UnitsDigitBoard);

    error_message = new Engine::Label("", "pirulen.ttf", 60, halfW, halfH, 255, 0, 0, 255, 0.5, 0.5);
    AddNewObject(error_message);
    error_message->Visible = false;
    error_message_cd = 0;
}

void StageSelectSceneHW::Terminate() {
    Engine::LOG(Engine::INFO) << "terminate stage select hand write scene";
	IScene::Terminate();
}

void StageSelectSceneHW::BackOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}

void StageSelectSceneHW::PlayOnClick(int stage) {
    PlayScene* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play-normal"));
    scene->MapId = stage;
    scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play-reverse"));
    scene->MapId = stage;
    scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play-survival"));
    scene->MapId = stage;

    Engine::GameEngine::GetInstance().ChangeScene("difficulty-select");
}


int StageSelectSceneHW::get_total_stages_count()
{
    auto dirIter = std::filesystem::directory_iterator("Resource/maps");

    int fileCount = std::count_if(
	    begin(dirIter),
	    end(dirIter),
	    [](auto& entry) { return entry.is_regular_file(); }
    );
    return fileCount;  
}

void StageSelectSceneHW::OnMouseUp(int button, int mx, int my)
{
    TensDigitBoard->OnMouseUp(button, mx, my);
    UnitsDigitBoard->OnMouseUp(button, mx, my);
    IScene::OnMouseUp(button, mx, my);
}

void StageSelectSceneHW::OnMouseDown(int button, int mx, int my)
{
    TensDigitBoard->OnMouseDown(button, mx, my);
    UnitsDigitBoard->OnMouseDown(button, mx, my);
    IScene::OnMouseDown(button, mx, my);
}

void StageSelectSceneHW::OnMouseMove(int mx, int my)
{
    TensDigitBoard->OnMouseMove(mx, my);
    UnitsDigitBoard->OnMouseMove(mx, my);
    IScene::OnMouseMove(mx, my);
}

void StageSelectSceneHW::SubmitOnClick()
{
    int ten = TensDigitBoard->GetNumber();
    int one = UnitsDigitBoard->GetNumber();

    number_of_stages = get_total_stages_count();

    int choose_stage = ten*10 + one;

    if(choose_stage <= 0 || choose_stage > number_of_stages)
    {
        PrintErrorMessage(choose_stage);
        return;
    }

    PlayScene* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play-normal"));
    scene->MapId = choose_stage;
    scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play-reverse"));
    scene->MapId = choose_stage;
    scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play-survival"));
    scene->MapId = choose_stage;

    Engine::GameEngine::GetInstance().ChangeScene("difficulty-select");
}

void StageSelectSceneHW::PrintErrorMessage(int invalid_stage)
{
    error_message_cd = MaxErrorMessageCD;

    std::string str = std::to_string(invalid_stage) + " is invalid";
    error_message->Text = str;
    error_message->Visible = true;
}

void StageSelectSceneHW::Update(float deltaTime)
{
    error_message_cd -= deltaTime;
    if(error_message_cd <= 0)
    {
        error_message_cd = 0;
        error_message->Visible = false;
    }
}

#endif