#ifndef SIMULTANEOUS_CMAPD_SCMAPD_HPP
#define SIMULTANEOUS_CMAPD_SCMAPD_HPP

#include <filesystem>
#include <unordered_set>
#include <list>
#include <vector>
#include "Status.hpp"
#include "Assignment.hpp"

// taskIndex, robot
using SmallH = std::pair<unsigned, std::vector<Assignment>>;

inline auto compareSmallH = [](const SmallH & a, const SmallH & b){
    return (a.second[0]) < (b.second[0]);
};

// heap of assignments that differ by tasks
using BigH = std::list<SmallH>;

class SCMAPD {
public:
    SCMAPD(
        cmapd::AmbientMapInstance&& ambientMapInstance,
        std::vector<Assignment> &&robots,
        std::vector<Task> && tasksVector
    );

    void solve(Heuristic heuristic, TimeStep cutOffTime);
private:
    Status status;
    BigH bigH;

    static BigH
    buildPartialAssignmentHeap(const Status &status);

    static Assignment
    initializePartialAssignment(const Status &status, int taskIndex, const Assignment &robot);

    Assignment extractTop();

    void updateSmallHTop(int assignmentIndex, int v, std::vector<Assignment> &partialAssignments);
};

#endif //SIMULTANEOUS_CMAPD_SCMAPD_HPP
