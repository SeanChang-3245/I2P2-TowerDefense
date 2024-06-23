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
                  State &nextState);

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
    bool IsTerminal(const State &state);
    
    /**
     * @brief Set the Initial Sample object
     * 
     * @param x The initial X coordinate of the enemy
     * @param y The initial Y coordinate of the enemy
     * @param mapStatus The map status when the enemy spawn
     */
    void SetInitialSample(double x, double y, const std::vector<std::vector<int>> &mapStatus);

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
    bool UpdateNextPosition(const State &state, State &nextState, Action::actions dir);

    /**
     * @return true when next step is closer to the end point
     * @return false
     */
    bool NextStepGetCloser(const State &state, Action::actions dir) const;

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
