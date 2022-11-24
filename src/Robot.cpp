//
// Created by nicco on 11/11/2022.
//

#include <numeric>
#include <limits>
#include "Robot.hpp"

Robot::Robot(CompressedCoord position, unsigned index, unsigned capacity) :
        startPosition{position},
        index{index},
        capacity{capacity}
    {}

void Robot::setTasks(WaypointsList &&newWaypoints, const TasksVector &tasks, const DistanceMatrix &distanceMatrix) {
    waypoints = std::move(newWaypoints);
    ttd = computeTTD(tasks, distanceMatrix);
}

void Robot::setTasks(Robot &&robot, const TasksVector &tasks, const DistanceMatrix &distanceMatrix) {
    setTasks(std::move(robot.waypoints), tasks, distanceMatrix);
    robot.waypoints.clear();
    robot.ttd = 0;
}

unsigned int Robot::getCapacity() const {
    return capacity;
}

const WaypointsList &Robot::getWaypoints() const {
    return waypoints;
}

TimeStep Robot::getTtd() const {
    return ttd;
}

unsigned Robot::getIndex() const {
    return index;
}

CompressedCoord Robot::getStartPosition() const {
    return startPosition;
}

bool Robot::empty() const {
    return waypoints.empty();
}

void Robot::setTasks(const WaypointsList &newWaypoints, const TasksVector &tasks, const DistanceMatrix &distanceMatrix) {
    waypoints = newWaypoints;
    ttd = computeTTD(tasks, distanceMatrix);
}

void Robot::setTasks(const Robot &robot, const TasksVector &tasks, const DistanceMatrix &distanceMatrix) {
    setTasks(robot.getWaypoints(), tasks, distanceMatrix);
    ttd = computeTTD(tasks, distanceMatrix);
}

void
Robot::insert(const Task &task, Heuristic heuristic, const DistanceMatrix &distanceMatrix, const TasksVector &tasks) {
    WaypointsList::iterator bestStartWaypoint, bestGoalWaypoint;
    TimeStep bestTTD = std::numeric_limits<TimeStep>::max();

    // search for best position for task start and goal
    for(auto waypointStart = waypoints.begin(); waypointStart != waypoints.end() ; ++waypointStart){
        for (auto waypointGoal = std::next(waypointStart); waypointGoal != waypoints.end(); ++waypointGoal){
            insertTaskWaypoints(task, waypointStart, waypointGoal);
            if(checkCapacityConstraint()){
                bestTTD = updateBestWaypoints(bestTTD, bestStartWaypoint, bestGoalWaypoint, distanceMatrix, tasks);
            }
            restorePreviousWaypoints(waypointStart, waypointGoal);
        }
    }

    waypoints.insert(bestStartWaypoint, {task.startLoc, Demand::START, task.index});
    waypoints.insert(bestGoalWaypoint, {task.goalLoc, Demand::GOAL, task.index});
}

void Robot::restorePreviousWaypoints(WaypointsList::iterator &waypointStart,
                                     WaypointsList::iterator &waypointGoal) {
    waypoints.erase(waypointStart);
    waypoints.erase(waypointGoal);
}

void Robot::insertTaskWaypoints(const Task &task, WaypointsList::iterator &waypointStart,
                                WaypointsList::iterator &waypointGoal) {
    waypoints.insert(waypointStart, {task.startLoc, Demand::START, task.index});
    waypoints.insert(waypointGoal, {task.goalLoc, Demand::GOAL, task.index});
}

bool Robot::checkCapacityConstraint() {
    unsigned actualWeight = 0;

    for(const auto& waypoint : waypoints){
        actualWeight += static_cast<unsigned>(waypoint.demand);
        if(actualWeight > getCapacity()){
            return false;
        }
    }
    return true;
}

TimeStep Robot::updateBestWaypoints(TimeStep bestTTD, WaypointsList::iterator &bestStart, WaypointsList::iterator &bestEnd,
                                    const DistanceMatrix &distanceMatrix, const TasksVector &tasks) {
    // todo finish this
}

TimeStep Robot::computeTTD(const std::vector<Task> &tasks, const DistanceMatrix &distanceMatrix) const{
    int i = 0;
    TimeStep cumulatedTTD = 0;

    for(const auto& wp: waypoints){
        const Task& task = tasks[wp.taskIndex];
        if(wp.demand == Demand::GOAL){
            cumulatedTTD += i - task.releaseTime - distanceMatrix[task.startLoc][task.goalLoc];
        }
        ++i;
    }

    return cumulatedTTD;
}
