#include <functional>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "PlayScene.hpp"
#include "Engine/Point.hpp"
#include "ReviveScene.hpp"
#include "Engine/LOG.hpp"
#include "Enemy/Enemy.hpp"

const float ReviveScene::MaxAnswerTime = 60;

// should support give up button and enter button

void ReviveScene::Initialize() {
	Engine::LOG(Engine::INFO) << "enter ReviveScene::initialize\n";

	ticks = 0;
	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	int halfW = w / 2;
	int halfH = h / 2;

	countDownTimer = MaxAnswerTime;


    getQuestionPicPathAndAnswer();

	AddNewObject(new Engine::Image(picture_path, halfW, halfH, 0, 0, 0.5, 0.5));


	Engine::ImageButton* btn;
	btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 450, halfH * 7 / 4 - 50, 400, 100);
	btn->SetOnClickCallback(std::bind(&ReviveScene::GiveUpOnClick, this));
	AddNewControlObject(btn);
	AddNewObject(new Engine::Label("Give Up", "pirulen.ttf", 48, halfW - 250, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));

	btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW + 50, halfH * 7 / 4 - 50, 400, 100);
	btn->SetOnClickCallback(std::bind(&ReviveScene::SubmitOnClick, this));
	AddNewControlObject(btn);
	AddNewObject(new Engine::Label("Submit", "pirulen.ttf", 48, halfW + 250, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));


	AddNewObject(UITime = new Engine::Label(std::to_string(countDownTimer).substr(5), "pirulen.ttf", 24, 1294, 168, 255, 255, 255));



	input_block = new Engine::TextInputBlock(halfW, halfH / 4 + 50, 0, 0, 0, 255, 0.5, 0.5);
    input_block->set_default_message("Enter Your Answer");
	AddNewControlObject(input_block);
}

void ReviveScene::Terminate() 
{
	IScene::Terminate();
}

void ReviveScene::Update(float deltaTime) 
{
	countDownTimer -= deltaTime;
	if(countDownTimer <= 0)
	{
		GiveUpOnClick();
		return;
	}


	int I = countDownTimer;
	float D = countDownTimer-I;
	std::string minute = std::to_string(I/60);
	std::string second = std::to_string(I%60);
	std::string decimal = std::to_string(D).substr(2, 2);

	if(second.size() == 1)
		second = "0" + second;

	UITime->Text = minute + ":" + second + "." + decimal;

}

void ReviveScene::GiveUpOnClick() 
{
	Engine::LOG(Engine::INFO) << "give up on click";
	Engine::GameEngine::GetInstance().GetPreviousScene()->Terminate();
	Engine::GameEngine::GetInstance().ChangeScene("lose");
}

void ReviveScene::SubmitOnClick() 
{
    if(CheckAnswer())
	{
		std::string prev_scene = getPrevSceneName();
	    Engine::GameEngine::GetInstance().ChangeScene(getPrevSceneName());
	}
    else
	{
		std::cout << Engine::GameEngine::GetInstance().GetSceneName(Engine::GameEngine::GetInstance().GetPreviousScene()) << std::endl;
		if (Engine::GameEngine::GetInstance().GetSceneName(Engine::GameEngine::GetInstance().GetPreviousScene())=="play-survival")
		{
			Engine::GameEngine::GetInstance().GetPreviousScene()->Terminate();
	    	Engine::GameEngine::GetInstance().ChangeScene("win");
		}
		else
		{
			Engine::GameEngine::GetInstance().GetPreviousScene()->Terminate();
	    	Engine::GameEngine::GetInstance().ChangeScene("lose");
		}
	}
}

void ReviveScene::OnKeyDown(int keyCode)
{
	input_block->OnKeyDown(keyCode);
	if(keyCode == ALLEGRO_KEY_ENTER)
        SubmitOnClick();
}

void ReviveScene::getQuestionPicPathAndAnswer()
{
    srand(time(NULL));

    int fileCnt = getQuestionCount();
    int question_num = rand() % fileCnt;
    
    // choose random question
    picture_path = "revive/question_" + std::to_string(question_num) + ".png";

    // get corresponding answer stored in question_answer.txt
    std::string filename = std::string("Resource/question_answer.txt");
	std::ifstream fin(filename);

    std::string tmp;
    std::vector<std::string> answers;
    while(fin >> tmp)
        answers.push_back(tmp);
    question_answer = answers[question_num];
}

int ReviveScene::getQuestionCount() const
{
    auto dirIter = std::filesystem::directory_iterator("Resource/images/revive");

    int fileCount = std::count_if(
	    begin(dirIter),
	    end(dirIter),
	    [](auto& entry) { return entry.is_regular_file(); }
    );
    return fileCount;  
}

bool ReviveScene::CheckAnswer() const
{
    if(input_block->get_text_inputed() == question_answer)
        return true;
    return false;
}

std::string ReviveScene::getPrevSceneName() {
	PlayScene* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetPreviousScene());
	return Engine::GameEngine::GetInstance().GetSceneName(scene);
}

