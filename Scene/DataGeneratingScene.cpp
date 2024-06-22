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
#include "Scene/DataGeneratingScene.hpp"
#include "Engine/LOG.hpp"
#include "UI/Component/DrawBoard.hpp"
#include "ML_Macro.hpp"

static const int RowSpacing = 200;
static const int ColSpacing = 450;
const float DataGeneratingScene::MaxErrorMessageCD = 1.5;

void DataGeneratingScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;


    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 420, halfH * 3 / 2 + 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&DataGeneratingScene::ClearOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Clear", "pirulen.ttf", 48, halfW - 220, halfH * 3 / 2 + 100, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW + 20, halfH * 3 / 2 + 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&DataGeneratingScene::SubmitOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Submit", "pirulen.ttf", 48, halfW + 220, halfH * 3 / 2 + 100, 0, 0, 0, 255, 0.5, 0.5));

    Board = new DrawBoard(w*3/4 - 560 / 2, 50);
    Board->SetFileOutput("../ML_Resource/my_train.csv");
    AddNewObject(Board);

    input_block = new Engine::TextInputBlock(350, halfH, 0, 0, 0, 255, 0, 0.5);
    input_block->set_default_message("enter label (0~9)");
    AddNewObject(input_block);

    error_message = new Engine::Label("Please enter correct label", "pirulen.ttf", 60, halfW, halfH, 255, 0, 0, 255, 0.5, 0.5);
    AddNewObject(error_message);
    error_message->Visible = false;
    error_message_cd = 0;
}

void DataGeneratingScene::Terminate() {
    Engine::LOG(Engine::INFO) << "terminate data generating scene";
	IScene::Terminate();
}

void DataGeneratingScene::ClearOnClick() 
{
    Board->ClearBoard();
}

void DataGeneratingScene::OnMouseUp(int button, int mx, int my)
{
    Board->OnMouseUp(button, mx, my);
    IScene::OnMouseUp(button, mx, my);
}

void DataGeneratingScene::OnMouseDown(int button, int mx, int my)
{
    Board->OnMouseDown(button, mx, my);
    IScene::OnMouseDown(button, mx, my);
}

void DataGeneratingScene::OnMouseMove(int mx, int my)
{
    Board->OnMouseMove(mx, my);
    IScene::OnMouseMove(mx, my);
}

void DataGeneratingScene::SubmitOnClick()
{
    if(input_block->get_text_length() != 1 || !isdigit(input_block->get_text_inputed()[0]))
    {
        PrintErrorMessage();
        return;
    }
    Board->OutputDataPoint(std::atoi(input_block->get_text_inputed().c_str()));
    Board->ClearBoard();
    input_block->clear_text_inputed();
}

void DataGeneratingScene::PrintErrorMessage()
{
    error_message_cd = MaxErrorMessageCD;
    error_message->Visible = true;
}

void DataGeneratingScene::Update(float deltaTime)
{
    error_message_cd -= deltaTime;
    if(error_message_cd <= 0)
    {
        error_message_cd = 0;
        error_message->Visible = false;
    }
    IScene::Update(deltaTime);
}

void DataGeneratingScene::OnKeyDown(int keyCode)
{
    if(ALLEGRO_KEY_0 <= keyCode && keyCode <= ALLEGRO_KEY_9)
        input_block->OnKeyDown(keyCode);
    if(keyCode == ALLEGRO_KEY_BACKSPACE)
        input_block->OnKeyDown(keyCode);
    if(keyCode == ALLEGRO_KEY_ENTER)
        SubmitOnClick();
    IScene::OnKeyDown(keyCode);
}
