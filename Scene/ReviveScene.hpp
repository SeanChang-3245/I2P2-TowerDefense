#ifndef REVIVESCENE_HPP
#define REVIVESCENE_HPP
#include <allegro5/allegro_audio.h>
#include <string>
#include "Engine/IScene.hpp"
#include "UI/Component/TextInput.hpp"

class ReviveScene final : public Engine::IScene {
private:
    static const float MaxAnswerTime; 
    float countDownTimer;
	float ticks;
	Engine::TextInputBlock* input_block;
    std::string picture_path;
    std::string question_answer;
	Engine::Label* UITime;


    void getQuestionPicPathAndAnswer();
    int getQuestionCount() const;
    bool CheckAnswer() const;
	std::string getPrevSceneName();

public:
	explicit ReviveScene() = default;
	void Initialize() override final;
	void Terminate() override final;
	void Update(float deltaTime) override final;
	void OnKeyDown(int keyCode) override final;
    void GiveUpOnClick();
	void SubmitOnClick();
	
};

#endif // WINSCENE_HPP
