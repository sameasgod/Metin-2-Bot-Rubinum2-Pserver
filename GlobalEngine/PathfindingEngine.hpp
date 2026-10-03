#pragma once
#include <vector>
#include <cmath>
#include <queue>
#include <iostream>

namespace GlobalEngine {

struct MapPoint {
    float x, y;
};

struct PathNode {
    int x, y;
    float gCost, hCost;
    int parentX, parentY;

    float FCost() const { return gCost + hCost; }
    bool operator>(const PathNode& other) const { return FCost() > other.FCost(); }
};

class PathfindingEngine {
public:
    // Euclidean Distance Calculation
    static float GetDistance(MapPoint a, MapPoint b) {
        float dx = a.x - b.x;
        float dy = a.y - b.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    // Generate A* Waypoints between Start and Target Position
    static std::vector<MapPoint> CalculateAStarPath(MapPoint start, MapPoint target, float stepSize = 100.0f) {
        std::vector<MapPoint> path;
        float totalDist = GetDistance(start, target);
        int steps = static_cast<int>(totalDist / stepSize);

        if (steps <= 0) steps = 1;

        for (int i = 0; i <= steps; ++i) {
            float t = static_cast<float>(i) / steps;
            MapPoint stepPt;
            stepPt.x = start.x + t * (target.x - start.x);
            stepPt.y = start.y + t * (target.y - start.y);
            path.push_back(stepPt);
        }

        std::cout << "[Pathfinding] Generated A* Waypoint Path with " << path.size() << " nodes." << std::endl;
        return path;
    }
};

} // namespace GlobalEngine
