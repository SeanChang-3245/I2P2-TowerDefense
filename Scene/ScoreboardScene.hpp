#ifndef SCOREBOARDSCENE_HPP
#define SCOREBOARDSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>
#include <utility>
#include <vector>
#include <string>
#include "Engine/IScene.hpp"

struct ScoreboardData
{
    std::string player_name;
    std::string score;
    std::string date_day;
    std::string date_hour;
    ScoreboardData() : date_day(std::string("01/01")) {}
};

class ScoreboardScene final : public Engine::IScene
{
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;

    // first entry store the scoreboard raw data read from txt, second entry store the Group* which is correspond to the first entry
    std::vector<std::pair<ScoreboardData, Group *>> data_ptr_pairs;

    using P = std::pair<ScoreboardData, Group *>;
    using CmpTy = bool (*)(const P &, const P &);

    // store all kinds of comparator, use current_cmp to specify which on to use
    std::vector<CmpTy> cmps;
    // store row elements group, which a collection of Label of player_name, score, date
    Group *UIScoreboard;
    // specify the index in the data_ptr_pairs which is the index of the first row element in the current page
    int scoreboard_start;
    // total number of scoreboard data
    int num_of_record;
    // specify the sorting comparator use for sorting the data
    int current_cmp;

    // assign all kinds of comparator to the array of comparator, "cmps"
    void init_sorting_comparators();
    // read in data_ptr_pairs, and render the initial scoreboard using ascending_date comparator
    void init_scoreboard_data();
    // change row_group_element's visibility depending on scoreboard_start
    void update_element_visible();
    // from empty UIScoreboard, create all row element base on their order in the data_ptr_pairs
    void generate_all_row_element();
    Group *create_row_group_element(std::string first, std::string second, std::string third, int offset);

public:
    explicit ScoreboardScene() = default;
    void Initialize() override;
    void Terminate() override;
    void BackOnClick();
    void NextOnClick();
    void PrevOnClick();
    void SortOnClick();
};

static bool ascending_score(const std::pair<ScoreboardData, Engine::Group*> &lhs, const std::pair<ScoreboardData, Engine::Group*> &rhs);
static bool descending_score(const std::pair<ScoreboardData, Engine::Group*> &lhs, const std::pair<ScoreboardData, Engine::Group*> &rhs);
static bool ascending_name(const std::pair<ScoreboardData, Engine::Group*> &lhs, const std::pair<ScoreboardData, Engine::Group*> &rhs);
static bool descending_name(const std::pair<ScoreboardData, Engine::Group*> &lhs, const std::pair<ScoreboardData, Engine::Group*> &rhs);
static bool ascending_date(const std::pair<ScoreboardData, Engine::Group*> &lhs, const std::pair<ScoreboardData, Engine::Group*> &rhs);
static bool descending_date(const std::pair<ScoreboardData, Engine::Group*> &lhs, const std::pair<ScoreboardData, Engine::Group*> &rhs);

#endif // SCOREBOARDSCENE_HPP
