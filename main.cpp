// [main.cpp]
// This is the entry point of your game.
// You can register your scenes here, and start the game.
#include "Engine/GameEngine.hpp"
#include "Engine/LOG.hpp"
#include "Scene/LoseScene.hpp"
#include "Scene/NormalPlayScene.hpp"
#include "Scene/ReversePlayScene.hpp"
#include "Scene/StageSelectScene.hpp"
#include "Scene/DifficultySelectScene.hpp"
#include "Scene/ModeSelectScene.hpp"
#include "Scene/WinScene.hpp"
#include "Scene/StartScene.h"
#include "Scene/SettingsScene.hpp"
#include "Scene/ScoreboardScene.hpp"
#include "Scene/MapCreateScene.hpp"
#include "Scene/ReviveScene.hpp"
#include "Scene/SurvivalPlayScene.hpp"
#include "DebugMacro.hpp"
#include <iostream>
using namespace std;

int main(int argc, char **argv) {
	Engine::LOG::SetConfig(true, PRINT_LOG_VERBOSE);
	Engine::GameEngine& game = Engine::GameEngine::GetInstance();

    // TODO: [HACKATHON-1-SCENE] (3/4): Register Scenes here
	game.AddNewScene("start", new StartScene());

	game.AddNewScene("stage-select", new StageSelectScene());
	game.AddNewScene("difficulty-select", new DifficultySelectScene());
	game.AddNewScene("mode-select", new ModeSelectScene());
	
	game.AddNewScene("map-create", new MapCreateScene());
	game.AddNewScene("settings", new SettingsScene());
	game.AddNewScene("win", new WinScene());
	
	game.AddNewScene("play-normal", new NormalPlayScene());
	game.AddNewScene("play-reverse", new ReversePlayScene());
	game.AddNewScene("play-survival", new SurvivalPlayScene());
	
	game.AddNewScene("lose", new LoseScene());
	game.AddNewScene("scoreboard", new ScoreboardScene());
	game.AddNewScene("revive", new ReviveScene());

    // TODO: [HACKATHON-1-SCENE] (4/4): Change the start scene

	// game.Start("revive", 60, 1600, 832);
	game.Start("start", 60, 1600, 832);
	return 0;
}

