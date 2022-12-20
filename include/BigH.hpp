#ifndef SIMULTANEOUS_CMAPD_BIGH_HPP
#define SIMULTANEOUS_CMAPD_BIGH_HPP

#include <set>
#include "SmallH.hpp"

using SmallHComp = std::function<bool(const SmallH&,const SmallH&)>;

class BigH {
public:
    explicit BigH(Heuristic h);
    void insert(SmallH&& smallH);
    std::pair<int, Assignment> extractTopTop();
private:
    static SmallHComp getComparator(Heuristic h);
    std::set<SmallH, SmallHComp> smallHSet;
};


#endif //SIMULTANEOUS_CMAPD_BIGH_HPP
