#include <functional>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "ScoreboardScene.hpp"
#include "PlayScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/LOG.hpp"

using P = std::pair<ScoreboardData, Engine::Group *>;
std::ifstream &operator>>(std::ifstream &fin, ScoreboardData &data)
{
	fin >> data.player_name >> data.score >> data.date_day >> data.date_hour;
	return fin;
}

void ScoreboardScene::Initialize()
{
	Engine::LOG(Engine::INFO) << "enter scoreboard scene init";

	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	int halfW = w / 2;
	int halfH = h / 2;

	AddNewObject(new Engine::Label("Scoreboard", "pirulen.ttf", 48, halfW, halfH / 5, 0, 255, 0, 255, 0.5, 0.5));

	Engine::ImageButton *btn;
	btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 200, halfH * 7 / 4 - 50, 400, 100);
	btn->SetOnClickCallback(std::bind(&ScoreboardScene::BackOnClick, this));
	AddNewControlObject(btn);
	AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));

	btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 700, halfH * 7 / 4 - 50, 400, 100);
	btn->SetOnClickCallback(std::bind(&ScoreboardScene::PrevOnClick, this));
	AddNewControlObject(btn);
	AddNewObject(new Engine::Label("Prev Page", "pirulen.ttf", 48, halfW - 500, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));

	btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW + 300, halfH * 7 / 4 - 50, 400, 100);
	btn->SetOnClickCallback(std::bind(&ScoreboardScene::NextOnClick, this));
	AddNewControlObject(btn);
	AddNewObject(new Engine::Label("Next Page", "pirulen.ttf", 48, halfW + 500, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));

	btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 100, halfH / 5 + 40, 200, 50);
	btn->SetOnClickCallback(std::bind(&ScoreboardScene::SortOnClick, this));
	AddNewControlObject(btn);
	AddNewObject(new Engine::Label("sort", "pirulen.ttf", 36, halfW, halfH / 5 + 65, 0, 0, 0, 255, 0.5, 0.5));

	AddNewObject(UIScoreboard = new Group());
	init_sorting_comparators();
	init_scoreboard_data();

	bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
}

void ScoreboardScene::Terminate()
{
	Engine::LOG(Engine::INFO) << "terminate scoreboard scene";

	data_ptr_pairs.clear();
	AudioHelper::StopSample(bgmInstance);
	bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
	IScene::Terminate();
}

void ScoreboardScene::BackOnClick()
{
	Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}

// change currentcmp
// clear all object in UI group, data_ptr_pair
void ScoreboardScene::SortOnClick()
{
	current_cmp = (current_cmp + 1) % cmps.size();
	UIScoreboard->Clear();
	sort(data_ptr_pairs.begin(), data_ptr_pairs.end(), cmps[current_cmp]);
	generate_all_row_element();
	update_element_visible();

	Engine::LOG(Engine::INFO) << "row numbers: " << data_ptr_pairs.size();
	Engine::LOG(Engine::INFO) << "current cmp: " << current_cmp;
}

void ScoreboardScene::PrevOnClick()
{
	scoreboard_start -= 5;
	scoreboard_start = std::max(0, scoreboard_start);
	update_element_visible();
}

void ScoreboardScene::NextOnClick()
{
	scoreboard_start += 5;
	scoreboard_start = std::min(scoreboard_start, ((int)data_ptr_pairs.size()-1) / 5 * 5);
	update_element_visible();
}

void ScoreboardScene::init_scoreboard_data()
{
	std::string filename = std::string("Resource/scoreboard.txt");
	std::ifstream fin(filename);

	ScoreboardData tmp;
	while (fin >> tmp)
		data_ptr_pairs.emplace_back(tmp, nullptr);

	sort(data_ptr_pairs.begin(), data_ptr_pairs.end(), cmps[current_cmp]);
	generate_all_row_element();
	update_element_visible();
}

void ScoreboardScene::generate_all_row_element()
{
	num_of_record = data_ptr_pairs.size();
	scoreboard_start = 0;
	UIScoreboard->AddNewObject(create_row_group_element("name", "score", "date", 0));
	for (int i = 0; i < data_ptr_pairs.size(); ++i)
	{
		auto data = data_ptr_pairs[i].first;
		std::string date = data.date_day + ' ' + data.date_hour;
		data_ptr_pairs[i].second = create_row_group_element(data.player_name, data.score, date, i % 5 + 1);
		UIScoreboard->AddNewObject(data_ptr_pairs[i].second);
	}
}

// update which row group should be visible
void ScoreboardScene::update_element_visible()
{
	for (const auto &[data, group] : data_ptr_pairs)
		group->Visible = false;

	for (int i = scoreboard_start; i < std::min(scoreboard_start + 5, num_of_record); ++i)
		data_ptr_pairs[i].second->Visible = true;
}

Engine::Group *ScoreboardScene::create_row_group_element(std::string first, std::string second, std::string third, int offset)
{
	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;

	Group *retp = new Group();
	retp->AddNewObject(new Engine::Label(first, "pirulen.ttf", 48, w * 1 / 4, h * 1 / 4 + offset * 75, 0, 255, 0, 200, 0.5, 0.5));
	retp->AddNewObject(new Engine::Label(second, "pirulen.ttf", 48, w * 2 / 4, h * 1 / 4 + offset * 75, 0, 255, 0, 200, 0.5, 0.5));
	retp->AddNewObject(new Engine::Label(third, "pirulen.ttf", 48, w * 3 / 4, h * 1 / 4 + offset * 75, 0, 255, 0, 200, 0.5, 0.5));

	return retp;
}

/* ==================================================================================================
	sorting comparators
================================================================================================== */

static bool ascending_date(const P &lhs, const P &rhs)
{
	std::string ldate = lhs.first.date_day + lhs.first.date_hour;
	std::string rdate = rhs.first.date_day + rhs.first.date_hour;
	return ldate < rdate;
}

static bool descending_date(const P &lhs, const P &rhs)
{
	std::string ldate = lhs.first.date_day + lhs.first.date_hour;
	std::string rdate = rhs.first.date_day + rhs.first.date_hour;
	return ldate > rdate;
}

static bool ascending_name(const P &lhs, const P &rhs)
{
	return lhs.first.player_name < rhs.first.player_name;
}

static bool descending_name(const P &lhs, const P &rhs)
{
	return lhs.first.player_name > rhs.first.player_name;
}

static bool ascending_score(const P &lhs, const P &rhs)
{
	int lv  = atoi(lhs.first.score.c_str());
	int rv  = atoi(rhs.first.score.c_str());
	return lv < rv;
}

static bool descending_score(const P &lhs, const P &rhs)
{
	int lv  = atoi(lhs.first.score.c_str());
	int rv  = atoi(rhs.first.score.c_str());
	return lv > rv;
}

void ScoreboardScene::init_sorting_comparators()
{
	current_cmp = 0;
	cmps.resize(6);
	cmps[0] = &ascending_date;
	cmps[1] = &descending_date;
	cmps[2] = &ascending_name;
	cmps[3] = &descending_name;
	cmps[4] = &ascending_score;
	cmps[5] = &descending_score;
}
