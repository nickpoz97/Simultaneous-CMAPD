#ifndef SIMULTANEOUS_CMAPD_SMALLH_HPP
#define SIMULTANEOUS_CMAPD_SMALLH_HPP

#include <vector>
#include <boost/heap/fibonacci_heap.hpp>
#include "Assignment.hpp"
#include "Status.hpp"
#include "AgentInfo.hpp"

// todo check if heap is max or min
using SmallHFibHeap = boost::heap::fibonacci_heap<Assignment, boost::heap::compare<std::greater<>>>;
using SmallHHandles = std::vector<SmallHFibHeap::handle_type>;

class SmallH {
public:
    SmallH(const std::vector<AgentInfo> &agentsInfos, int taskId, int v, const Status &status);

    PathWrapper extractTopAndReset();
    [[nodiscard]] TimeStep getTopMCA() const;

    void updateTopElements(const Path &fixedPath, const Status &status);

    void addTaskToAgent(int k, int otherTaskId, const Status &status);

    int getTaskId() const;

private:
    SmallHHandles heapHandles;
    SmallHFibHeap heap;
    int taskId;
    int v;

    static std::pair<SmallHFibHeap, SmallHHandles>
    initializeHeap(const std::vector<AgentInfo> &agentsInfos, int taskId, const Status &status);
};


#endif //SIMULTANEOUS_CMAPD_SMALLH_HPP
