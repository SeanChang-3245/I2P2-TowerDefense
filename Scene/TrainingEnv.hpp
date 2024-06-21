#ifndef TRAININGENV_HPP
#define TRAININGENV_HPP

#include <mlpack/prereqs.hpp>
#include <utility>
#include "Scene/PlayScene.hpp"
#include "Engine/GameEngine.hpp"

// should create different path based on the type of the enemy
// only navigate the enemy from corner to corner, outer function should handle the path from SpawnGridPoint to first tile in map,
//      and the path from corner to EndGridPoint
class EnemyPathFinding
{
public:
    /**
     * Implementation of the state of Cart Pole. Each state is a vector
     * (position, enemy type, map status).
     */
    class State
    {
    public:
        /**
         * Construct a state instance.
         */
        State() : data(dimension)
        { /* Nothing to do here. */
        }

        /**
         * Construct a state instance from given data.
         *
         * @param data Data for the position, enemy type and map status.
         */
        State(const arma::colvec &data) : data(data)
        { /* Nothing to do here */
        }

        //! Modify the internal representation of the state.
        arma::colvec &Data() { return data; }

        //! Get the X coordinate.
        double Position_X() const { return data[0]; }

        //! Modify the X coordinate.
        double &Position_X() { return data[0]; }

        //! Get the Y coordinate.
        double Position_Y() const { return data[1]; }

        //! Modify the Y coordinate.
        double &Position_Y() { return data[1]; }

        //! Get the enemy HP.
        double HP() const { return data[2]; }

        //! Modify the enemy HP.
        double &HP() { return data[2]; }

        //! Get the map status
        arma::colvec MapStatus() const { return data.subvec(static_cast<arma::uword>(3), static_cast<arma::uword>(263)); }

        //! Encode the state to a column vector.
        const arma::colvec &Encode() const { return data; }

        //! Dimension of the encoded state.
        static constexpr size_t dimension = 263;

        // The beginning index of map status in encoded state (inclusive)
        static constexpr size_t mapStatusBegin = 3;

        // The endding index of map status in encoded state (exclusive)
        static constexpr size_t mapStatusEnd = 263;

    private:
        //! Locally-stored (position[2], enemy type[1], mapStatus[260]).
        arma::colvec data;
    };

    /**
     * Implementation of action of Cart Pole.
     */
    class Action
    {
    public:
        enum actions
        {
            LEFT,
            UP,
            RIGHT,
            DOWN
        };

        // To store the action.
        Action::actions action;

        // Track the size of the action space.
        static const size_t size = 4;
    };

    /**
     * @brief Construct a Enemy Path Finding instance using the given constants.
     * @param initHP The initial HP of the enemy
     * @param speed The speed of the enemy.
     * @param maxSteps The number of steps after which the episode
     *    terminates. If the value is 0, there is no limit.
     * @param deltaTime The time interval.
     * @param towardEndReward Reward recieved by agent on walking toward the end point.
     * @param awayEndReward Reward recieved by agent on walking alway from the end point.
     * @param outOfBoundReward Reward recieved by agent on walking out of the map.
     * @param dieReward Reward recieved by agent on being killed.
     * @param doneReward Reward recieved by agent on success. (reach the end point)
     * @param stepExceedReward Reward received by agent on performed step exceeds maxStep
     */
    EnemyPathFinding(const double initHP,
                     const double speed,
                     const size_t maxSteps = 10000,
                     const double deltaTime = 1.0 / Engine::GameEngine::GetInstance().GetFPS(),
                     const double towardEndReward = 1,
                     const double awayEndReward = -0.2,
                     const double outOfBoundReward = -10000,
                     const double dieReward = -10,
                     const double doneReward = 100,
                     const double stepExceedReward = -5) : speed(speed),
                                                           maxSteps(maxSteps),
                                                           deltaTime(deltaTime),
                                                           towardEndReward(towardEndReward),
                                                           awayEndReward(awayEndReward),
                                                           outOfBoundReward(outOfBoundReward),
                                                           dieReward(dieReward),
                                                           doneReward(doneReward),
                                                           stepExceedReward(stepExceedReward),
                                                           stepsPerformed(0),
                                                           killed(false)
    { /* Nothing to do here */
    }

    /**
     * Dynamics of Cart Pole instance. Get reward and next state based on current
     * state and current action.
     *
     * @param state The current state.
     * @param action The current action.
     * @param nextState The next state.
     * @return reward
     */
    double Sample(const State &state,
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

    /**
     * Dynamics of Cart Pole. Get reward based on current state and current
     * action.
     *
     * @param state The current state.
     * @param action The current action.
     * @return reward, it's always 1.0.
     */
    double Sample(const State &state, const Action &action)
    {
        State nextState;
        return Sample(state, action, nextState);
    }

    /**
     * Initial state representation is randomly generated within [-0.05, 0.05].
     *
     * @return Initial state for each episode.
     */
    State InitialSample()
    {
        stepsPerformed = 0;
        return initialSample;
    }

    /**
     * This function checks if the cart has reached the terminal state.
     *
     * @param state The desired state.
     * @return true if state is a terminal state, otherwise false.
     */
    bool IsTerminal(const State &state)
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
    
    /**
     * @brief Set the Initial Sample object
     * 
     * @param x The initial X coordinate of the enemy
     * @param y The initial Y coordinate of the enemy
     * @param mapStatus The map status when the enemy spawn
     */
    void SetInitialSample(double x, double y, const std::vector<std::vector<int>> &mapStatus)
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

    //! Get the number of steps performed.
    size_t StepsPerformed() const { return stepsPerformed; }

private:
    //! Locally-stored maximum number of steps.
    size_t maxSteps;

    //! Locally-stored whether the enemy have been killed.
    bool killed;

    //! Locally-stored time interval.
    double deltaTime;

    //! Locally-stored reward inside a sampling process.
    double rewardGain;

    //! Locally-stored reward the reward when agent walk toward the end.
    double towardEndReward;

    //! Locally-stored reward the reward when agent walk away from the end.
    double awayEndReward;

    //! Locally-stored reward the reward when agent walk out of the map.
    double outOfBoundReward;

    //! Locally-stored done reward. (reach the end point)
    double doneReward;

    //! Locally-stored done reward. (reach the end point)
    double stepExceedReward;

    //! Locally-stored die reward. (being killed by turret)
    double dieReward;

    //! Locally-stored the enemy's speed.
    double speed;

    //! Locally-stored number of steps performed.
    size_t stepsPerformed;

    State initialSample;

    /**
     * @param the direction
     * @return whether the next position is valid (out of bound)
     */
    bool UpdateNextPosition(const State &state, State &nextState, Action::actions dir)
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

    /**
     * @return true when next step is closer to the end point
     * @return false
     */
    bool NextStepGetCloser(const State &state, Action::actions dir) const
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

};

/* ========== State Spec ==========

the parameter in state shoud not be constant

263 items in total

0, 1 : position(x, y), double, 2 items
2    : enemy health, float, 1 item
3~262: mapStatus, TileType(including tower type) , 260 items, row major

========== State Spec ========== */

/* ========== Action Spec ==========

4 items in total

0    : move left
1    : move up
2    : move right
3    : move down

========== Action Spec ========== */

#endif
