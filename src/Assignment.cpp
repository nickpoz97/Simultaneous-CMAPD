//
// Created by nicco on 26/11/2022.
//

#include <limits>
#include <numeric>
#include <cassert>
#include "Assignment.hpp"

Assignment::Assignment(CompressedCoord startPosition, unsigned index, unsigned capacity) :
        startPosition{startPosition},
        index{index},
        capacity{capacity}
    {}

unsigned int Assignment::getCapacity() const {
    return capacity;
}

TimeStep Assignment::getTtd() const {
    return ttd;
}

unsigned Assignment::getIndex() const {
    return index;
}

CompressedCoord Assignment::getStartPosition() const {
    return startPosition;
}

void Assignment::update(Assignment&& assignment) {
    waypoints = std::move(assignment.waypoints);
    ttd = assignment.ttd;
}

void Assignment::setTasks(WaypointsList &&newWaypoints, const std::vector<Task> &tasks, const DistanceMatrix &distanceMatrix) {
    waypoints = std::move(newWaypoints);
    updatePath();
    ttd = computeRealTTD(tasks, distanceMatrix);
}

void Assignment::setTasks(Assignment &&pa, const std::vector<Task> &tasks, const DistanceMatrix &distanceMatrix) {
    setTasks(std::move(pa.waypoints), tasks, distanceMatrix);
    pa.waypoints.clear();
    pa.ttd = 0;
}

const WaypointsList &Assignment::getWaypoints() const {
    return waypoints;
}

bool Assignment::empty() const {
    return waypoints.empty();
}

void Assignment::setTasks(const WaypointsList &newWaypoints, const std::vector<Task> &tasks, const DistanceMatrix &distanceMatrix) {
    waypoints = newWaypoints;
    updatePath();
    ttd = computeRealTTD(tasks, distanceMatrix);
}

void Assignment::setTasks(const Assignment &pa, const std::vector<Task> &tasks, const DistanceMatrix &distanceMatrix) {
    setTasks(pa.getWaypoints(), tasks, distanceMatrix);
}

void
Assignment::insert(const Task &task, Heuristic heuristic, const DistanceMatrix &distanceMatrix, const std::vector<Task> &tasks) {
    WaypointsList::iterator bestStartIt, bestGoalIt;
    TimeStep bestApproxTTD = std::numeric_limits<TimeStep>::max();

    // search for best position for task start and goal
    for(auto waypointStart = waypoints.begin(); waypointStart != waypoints.end() ; ++waypointStart){
        for (auto waypointGoal = std::next(waypointStart); waypointGoal != waypoints.end(); ++waypointGoal){
            insertTaskWaypoints(task, waypointStart, waypointGoal);
            if(checkCapacityConstraint()){
                // todo add heuristic choices
                auto newApproxTtd = computeApproxTTD(tasks, distanceMatrix);
                if(newApproxTtd < bestApproxTTD){
                    bestApproxTTD = newApproxTtd;
                    bestStartIt = waypointStart;
                    bestGoalIt = waypointGoal;
                }
            }
            restorePreviousWaypoints(waypointStart, waypointGoal);
        }
    }

    waypoints.insert(bestStartIt, {task.startLoc, Demand::START, task.index});
    waypoints.insert(bestGoalIt, {task.goalLoc, Demand::GOAL, task.index});

    updatePath();
    ttd = computeRealTTD(tasks, distanceMatrix);
}

void Assignment::restorePreviousWaypoints(WaypointsList::iterator &waypointStart,
                                                 WaypointsList::iterator &waypointGoal) {
    waypoints.erase(waypointStart);
    waypoints.erase(waypointGoal);
}

void Assignment::insertTaskWaypoints(const Task &task, WaypointsList::iterator &waypointStart,
                                            WaypointsList::iterator &waypointGoal) {
    waypoints.insert(waypointStart, {task.startLoc, Demand::START, task.index});
    waypoints.insert(waypointGoal, {task.goalLoc, Demand::GOAL, task.index});
}

bool Assignment::checkCapacityConstraint() {
    unsigned actualWeight = 0;

    for(const auto& waypoint : waypoints){
        actualWeight += static_cast<unsigned>(waypoint.demand);
        if(actualWeight > getCapacity()){
            return false;
        }
    }
    return true;
}

TimeStep Assignment::computeRealTTD(const std::vector<Task> &tasks, const DistanceMatrix &distanceMatrix, WaypointsList::const_iterator lastWaypoint) const{
    assert(lastWaypointIndex < waypoints.size());

    TimeStep cumulatedTTD = 0;
    auto wpIt = waypoints.cbegin();
    unsigned wpIndex = 0;

    Path path; // todo compute path IMPORTANT!!!

    // i is the timestep
    for(int i = 0 ; i < path.size() || wpIt == lastWaypoint; ++i){
        // reached waypoint
        if(path[i] == wpIt->position){
            if(wpIt->demand == Demand::GOAL){
                const Task& task = tasks[wpIt->taskIndex];
                cumulatedTTD += i - task.getIdealGoalTime(distanceMatrix);
            }
            wpIt = std::next(wpIt);
        }
    }

    return cumulatedTTD;
}

// todo this has to be changed
TimeStep Assignment::computeApproxTTD(const std::vector<Task> &tasks, const DistanceMatrix &distanceMatrix) const{
    TimeStep cumulatedTTD = 0;
    auto previousPos = startPosition;

    for(const auto& wp: waypoints){
        const Task& task = tasks[wp.taskIndex];
        if(wp.demand == Demand::GOAL){
            // simplified expression
            cumulatedTTD += distanceMatrix[previousPos][task.startLoc] - task.releaseTime;
            previousPos = task.goalLoc;
        }
    }

    return cumulatedTTD;
}

void Assignment::updatePath() {
    // todo complete this
}

constexpr unsigned Assignment::computeHeuristic(Heuristic heur, TimeStep newtOk, TimeStep oldtOk) {
    auto fMCA = [newtOk, oldtOk](){
        return newtOk - oldtOk;
    };

    auto fRMCAa = [newtOk, oldtOk](){
        return newtOk - oldtOk;
    };


}

bool operator<(const Assignment& a, const Assignment& b){
    return a.sortKey < b.sortKey;
}

TimeStep Assignment::computeRealTTD(const std::vector<Task> &tasks, const DistanceMatrix &distanceMatrix) const {
    return computeRealTTD(tasks, distanceMatrix, waypoints.cend());
}
