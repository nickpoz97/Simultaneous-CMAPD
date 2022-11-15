//
// Created by nicco on 12/11/2022.
//

#ifndef SIMULTANEOUS_CMAPD_SCMAPD_HPP
#define SIMULTANEOUS_CMAPD_SCMAPD_HPP

#include <filesystem>
#include <unordered_set>
#include "Robot.hpp"
#include "PBS.h"

enum class Heuristic{
    MCA,
    RMCA_A,
    RMCA_R
};

using Assignment = std::vector<Robot>;

struct ComparePartialAssignment{
    bool operator()(const Robot& a, const Robot& b);
};

struct CompareTotalHeap{
    bool operator()(const Assignment & a, const Assignment & b);
};

using PartialAssignmentHeap = std::priority_queue<Assignment, std::vector<Assignment>, CompareTotalHeap>;

template<Heuristic heuristic>
class SCMAPD {
public:
    SCMAPD(
        DistanceMatrix && distanceMatrix,
        Assignment && robots,
        std::unordered_set<Task> && tasks,
        PBS&& pbs
    );

    ~SCMAPD();

    void solve();
private:
    const DistanceMatrix distanceMatrix;
    Assignment assignment;
    std::unordered_set<Task> unassignedTasks;
    PartialAssignmentHeap partialAssignmentsHeap;
    PBS pbs;

    SequenceOfReadyTasks insert(const Task &task, const SequenceOfReadyTasks& taskSequence);
    static PartialAssignmentHeap buildPartialAssignmentHeap(const Assignment & robots,
                                                     const std::unordered_set<Task> & tasks);
};

#include "../src/SCMAPD.hpp.i"

#endif //SIMULTANEOUS_CMAPD_SCMAPD_HPP
