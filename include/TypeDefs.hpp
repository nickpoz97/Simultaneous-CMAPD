#ifndef SIMULTANEOUS_CMAPD_TYPEDEFS_HPP
#define SIMULTANEOUS_CMAPD_TYPEDEFS_HPP

#include <vector>
#include <list>
#include "Coord.hpp"

using TimeStep = int;

using PickupDropoffTime = std::pair<TimeStep, TimeStep>;

enum class Demand : int{
    START = 1,
    GOAL = -1
};

enum class Heuristic{
    MCA,
    RMCA_A,
    RMCA_R
};

struct AgentInfo{
    CompressedCoord startPos;
    int capacity;
    int index;
};

#endif //SIMULTANEOUS_CMAPD_TYPEDEFS_HPP
