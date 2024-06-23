#include "Scene/TrainingEnv.hpp"


using namespace mlpack;

double EnemyPathFinding::Sample(const State &state, 
                                const Action &action, 
                                State &nextState)
{
    rewardGain = 0;
    // Update the number of steps performed.
    stepsPerformed++;

    // Update states, if out of bound than add negative reward
    if (!UpdateNextPosition(state, nextState, action.action))
        rewardGain += outOfBoundReward;

    // Check if the episode has terminated.
    bool done = IsTerminal(nextState);

    // Do not reward agent if it failed.
    if (done && maxSteps != 0 && stepsPerformed >= maxSteps)
        return doneReward;

    if(NextStepGetCloser(state, action.action))
        rewardGain += towardEndReward;
    else
        rewardGain += awayEndReward;

    return rewardGain;
}

bool EnemyPathFinding::IsTerminal(const State &state)
{
    if (maxSteps != 0 && stepsPerformed >= maxSteps)
    {
        rewardGain += stepExceedReward;
        mlpack::Log::Info << "Episode terminated due to the maximum number of steps"
                                "being taken.";
        return true;
    }
    else if (state.Position_X() / PlayScene::BlockSize == PlayScene::MapWidth - 1 &&
                state.Position_Y() / PlayScene::BlockSize == PlayScene::MapHeight - 1)
    {
        rewardGain += doneReward;
        mlpack::Log::Info << "Episode terminated due to agent reaching end point";
        return true;
    }
    else if (killed)
    {
        rewardGain += dieReward;
        mlpack::Log::Info << "Episode terminated due to agent being killed.";
        return true;
    }
    return false;
}

void EnemyPathFinding::SetInitialSample(double x, double y, const std::vector<std::vector<int>> &mapStatus)
{
    initialSample.Position_X() = x;
    initialSample.Position_Y() = y;
    for(int i = 0; i < PlayScene::MapHeight; ++i)
    {
        for(int j = 0; j < PlayScene::MapWidth; ++j)
        {
            initialSample.Data()[State::mapStatusBegin + i * PlayScene::MapWidth + j] = mapStatus[i][j];
        }
    }
}

bool EnemyPathFinding::UpdateNextPosition(const State &state, State &nextState, Action::actions dir)
{
    const double xMin = 0;
    const double yMin = 0;
    const double xMax = PlayScene::MapSize.x;
    const double yMax = PlayScene::MapSize.y;

    switch (dir)
    {
    case Action::actions::LEFT:
        nextState.Position_X() = state.Position_X() - speed * deltaTime;
        break;
    case Action::actions::UP:
        nextState.Position_Y() = state.Position_Y() - speed * deltaTime;
        break;
    case Action::actions::RIGHT:
        nextState.Position_X() = state.Position_X() + speed * deltaTime;
        break;
    case Action::actions::DOWN:
        nextState.Position_Y() = state.Position_Y() - speed * deltaTime;
        break;
    default:
        break;
    }

    if (nextState.Position_X() < xMin)
    {
        nextState.Position_X() = xMin;
        return false;
    }
    if (nextState.Position_X() > xMax)
    {
        nextState.Position_X() = xMax;
        return false;
    }
    if (nextState.Position_Y() < yMin)
    {
        nextState.Position_Y() = yMin;
        return false;
    }
    if (nextState.Position_Y() > yMax)
    {
        nextState.Position_Y() = yMax;
        return false;
    }

    return true;
}

bool EnemyPathFinding::NextStepGetCloser(const State &state, Action::actions dir) const
{
    const int mapHeight = PlayScene::MapHeight;
    const int mapWidth = PlayScene::MapWidth;
    std::vector<std::vector<int>> mapState(mapHeight, std::vector<int>(mapWidth));
    
    arma::colvec encode = state.MapStatus();
    for(int i = 0; i < mapHeight; ++i)
        for(int j = 0; j < mapWidth; ++j)
            mapState[i][j] = static_cast<int>(encode[i*mapWidth + j]);


    // Reverse BFS to find path.
    std::vector<std::vector<int>> mapDist(mapHeight, std::vector<int>(std::vector<int>(mapWidth, -1)));
    std::queue<Engine::Point> que;
    // Push end point.
    // BFS from end point.

    que.push(Engine::Point(mapWidth - 1, mapHeight - 1));
    mapDist[mapHeight - 1][mapWidth - 1] = 0;

    int cur_dist = 0;
    while (!que.empty())
    {
        // TODO: [BFS PathFinding] (1/1): Implement a BFS starting from the most right-bottom block in the map.
        //               For each step you should assign the corresponding distance to the most right-bottom block.
        //               mapState[y][x] is TILE_DIRT if it is empty.
        int s = que.size();
        while (s--)
        {
            Engine::Point p = que.front();
            que.pop();

            for (const auto &dir : PlayScene::directions)
            {
                int nextX = (dir + p).x;
                int nextY = (dir + p).y;

                if (nextX < 0 || nextX >= mapWidth ||
                    nextY < 0 || nextY >= mapHeight ||
                    mapState[nextY][nextX] == PlayScene::TILE_FLOOR ||
                    mapState[nextY][nextX] == PlayScene::TILE_OCCUPIED ||
                    mapDist[nextY][nextX] != -1) // the tile is floor but its shortest distance has been updated
                    continue;

                mapDist[nextY][nextX] = cur_dist + 1;
                que.emplace(Engine::Point(nextX, nextY));
            }
        }
        ++cur_dist;
    }

    const int gridX = state.Position_X() / PlayScene::BlockSize;
    const int gridY = state.Position_Y() / PlayScene::BlockSize;

    switch (dir)
    {
    case Action::actions::LEFT :
        if(gridX - 1 < 0 || mapDist[gridY][gridX-1] == -1 || mapDist[gridY][gridX-1] > mapDist[gridY][gridX])
            return false;
        break;
    case Action::actions::UP :
        if(gridY - 1 < 0 || mapDist[gridY-1][gridX] == -1 || mapDist[gridY-1][gridX] > mapDist[gridY][gridX])
            return false;
        break;
    case Action::actions::DOWN :
        if(gridX + 1 >= mapWidth || mapDist[gridY][gridX+1] == -1 || mapDist[gridY][gridX+1] > mapDist[gridY][gridX])
            return false;
        break;
    case Action::actions::RIGHT :
        if(gridY + 1 >= mapHeight || mapDist[gridY+1][gridX] == -1 || mapDist[gridY+1][gridX] > mapDist[gridY][gridX])
            return false;
        break;
    default:
        break;
    }

    return true;
}