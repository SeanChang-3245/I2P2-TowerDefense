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
#include "Scene/StageSelectHandWriteScene.hpp"
#include "Scene/DataGeneratingScene.hpp"
#include "DebugMacro.hpp"
#include "ML_Macro.hpp"
#include <iostream>
using namespace std;

int main(int argc, char **argv) {
	Engine::LOG::SetConfig(true, PRINT_LOG_VERBOSE);
	Engine::GameEngine& game = Engine::GameEngine::GetInstance();

#if GEN_TRAIN_DATA
	game.AddNewScene("gen-data", new DataGeneratingScene());
	game.Start("gen-data", 60, 1600, 832);
	return 0;
#endif

#ifdef TRAINING_AGENT


#endif

	game.AddNewScene("start", new StartScene());

	game.AddNewScene("stage-select", new StageSelectScene());
	game.AddNewScene("difficulty-select", new DifficultySelectScene());
	game.AddNewScene("mode-select", new ModeSelectScene());

#if USE_ML
	game.AddNewScene("stage-select-hw", new StageSelectSceneHW());
#endif
	
	game.AddNewScene("map-create", new MapCreateScene());
	game.AddNewScene("settings", new SettingsScene());
	game.AddNewScene("win", new WinScene());
	
	game.AddNewScene("play-normal", new NormalPlayScene());
	game.AddNewScene("play-reverse", new ReversePlayScene());
	game.AddNewScene("play-survival", new SurvivalPlayScene());
	
	game.AddNewScene("lose", new LoseScene());
	game.AddNewScene("scoreboard", new ScoreboardScene());
	game.AddNewScene("revive", new ReviveScene());

	game.Start("start", 60, 1600, 832);
	return 0;
}


// ============ Run following code to see if mlpack can be used ============ //  

// #define MLPACK_PRINT_INFO
// #define MLPACK_PRINT_WARN
// #include <mlpack.hpp>

// using namespace arma;
// using namespace mlpack;
// using namespace std;
// using namespace mlpack::rl;
// using namespace ens;


// int main()
// {
// 	FFN<MeanSquaredError, GaussianInitialization> network(MeanSquaredError(),
//       GaussianInitialization(0, 0.001));
//     network.Add<Linear>(128);
//     network.Add<ReLU>();
//     network.Add<Linear>(128);
//     network.Add<ReLU>();
//     network.Add<Linear>(2);
//     // cout << "Before train Network parameters: " << network.Parameters() << "\n\n";
//     SimpleDQN<> model(network);
//     // SimpleDQN<> model(4, 64, 32, 2);

//     GreedyPolicy<CartPole> policy(1.0, 1000, 0.1, 0.99);
//     RandomReplay<CartPole> replayMethod(10, 10000);

//     TrainingConfig config;
//     config.StepSize() = 0.01;
//     config.Discount() = 0.9;
//     config.TargetNetworkSyncInterval() = 100;
//     config.ExplorationSteps() = 100;
//     config.DoubleQLearning() = false;
//     config.StepLimit() = 200;

//     QLearning<CartPole, decltype(model), AdamUpdate, decltype(policy)>
//         agent(config, model, policy, replayMethod);

//     arma::running_stat<double> averageReturn;
//     size_t episodes = 0;
//     bool converged = true;
//     while (true)
//     {
//         double episodeReturn = agent.Episode();
//         averageReturn(episodeReturn);
//         episodes += 1;

//         if (episodes > 10000)
//         {
//             std::cout << "Cart Pole with DQN failed." << std::endl;
//             converged = false;
//             break;
//         }

//         /**
//          * Reaching running average return 35 is enough to show it works.
//          */
//         std::cout << "Average return: " << averageReturn.mean()
//                   << " Episode return: " << episodeReturn << std::endl;
//         if (averageReturn.mean() > 35 && episodes > 10)
//             break;
//     }
//     if (converged)
//         std::cout << "Hooray! Q-Learning agent successfully trained" << std::endl;
//     return 0;
// }
