#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
#include <stack>
#include "Defines.h"

using namespace std;

// 战场坐标点
struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
    
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
    
    Point operator+(const Point& other) const {
        return Point(x + other.x, y + other.y);
    }
    
    double distance(const Point& other) const {
        return sqrt(pow(x - other.x, 2) + pow(y - other.y, 2));
    }
};

// 长沙战场地形
class ChangshaTerrain {
private:
    vector<vector<int>> terrain;
    const int SIZE = 20; // 20x20战场
    
public:
    ChangshaTerrain() {
        terrain.resize(SIZE, vector<int>(SIZE, 0));
        initChangshaFeatures();
    }
    
    void initChangshaFeatures() {
        // 岳麓山雷达区 (坐标5,5)
        addRadarZone({5,5}, 3);
        
        // 湘江航道 (Y=10区域)
        for(int x=0; x<SIZE; x++) {
            setTerrain(x, 10, static_cast<int>(ThreatLevel::WATER_SURFACE));
        }
        
        // 黄花机场防空区 (坐标15,15)
        addAirDefenseZone({15,15}, 2);
    }
    
    void addRadarZone(Point center, int radius) {
        for(int x=0; x<SIZE; x++) {
            for(int y=0; y<SIZE; y++) {
                double dist = center.distance({x, y});
                if(dist <= radius) {
                    setTerrain(x, y, static_cast<int>(ThreatLevel::RADAR_COVERED));
                }
            }
        }
    }
    
    void addAirDefenseZone(Point center, int radius) {
        for(int x=center.x-radius; x<=center.x+radius; x++) {
            for(int y=center.y-radius; y<=center.y+radius; y++) {
                if(x>=0 && x<SIZE && y>=0 && y<SIZE) {
                    setTerrain(x, y, static_cast<int>(ThreatLevel::AIR_DEFENSE_ZONE));
                }
            }
        }
    }
    
    void setTerrain(int x, int y, int type) {
        if(x>=0 && x<SIZE && y>=0 && y<SIZE) {
            // 只设置更高威胁等级
            if(type > terrain[x][y]) {
                terrain[x][y] = type;
            }
        }
    }
    
    // 获取区域风险值
    int getRisk(Point p) const {
        if(p.x<0 || p.x>=SIZE || p.y<0 || p.y>=SIZE) 
            return 100; // 边界高危险
        
        switch(terrain[p.x][p.y]) {
            case static_cast<int>(ThreatLevel::SAFE): return 1;
            case static_cast<int>(ThreatLevel::RADAR_COVERED): return 10;
            case static_cast<int>(ThreatLevel::WATER_SURFACE): return 3;
            case static_cast<int>(ThreatLevel::AIR_DEFENSE_ZONE): return 15;
            default: return 5;
        }
    }
    
    void display() const {
        cout << "长沙战场地图:\n";
        cout << "  R=雷达区 ~=水面 A=防空区\n";
        cout << "----------------------------\n";
        
        for(int y=0; y<SIZE; y++) {
            for(int x=0; x<SIZE; x++) {
                char c = '.';
                switch(terrain[x][y]) {
                    case static_cast<int>(ThreatLevel::RADAR_COVERED): c = 'R'; break;
                    case static_cast<int>(ThreatLevel::WATER_SURFACE): c = '~'; break;
                    case static_cast<int>(ThreatLevel::AIR_DEFENSE_ZONE): c = 'A'; break;
                }
                cout << c;
            }
            cout << endl;
        }
        cout << "----------------------------\n";
    }
};

// 战术DFS路径规划器
class StealthPathFinder {
private:
    const vector<Point> DIRECTIONS = {{1,0}, {0,1}, {-1,0}, {0,-1}};
    
public:
    vector<Point> findPath(const ChangshaTerrain& terrain, Point start, Point target) {
        const int SIZE = 20;
        vector<vector<bool>> visited(SIZE, vector<bool>(SIZE, false));
        vector<vector<int>> minRisk(SIZE, vector<int>(SIZE, INT_MAX));
        vector<vector<Point>> prev(SIZE, vector<Point>(SIZE, {-1,-1}));
        
        // 优先队列模拟 (简易版)
        minRisk[start.x][start.y] = terrain.getRisk(start);
        visited[start.x][start.y] = true;
        
        // DFS核心算法
        dfs(terrain, start, target, visited, minRisk, prev, minRisk[start.x][start.y]);
        
        // 重构路径
        return reconstructPath(prev, start, target);
    }
    
private:
    void dfs(const ChangshaTerrain& terrain, Point current, Point target,
             vector<vector<bool>>& visited, 
             vector<vector<int>>& minRisk,
             vector<vector<Point>>& prev,
             int currentRisk) {
        
        if(current == target) return;
        
        // 按风险排序方向 (战术优化)
        vector<pair<Point, int>> dirRisk;
        for(const auto& dir : DIRECTIONS) {
            Point next = current + dir;
            if(next.x>=0 && next.x<20 && next.y>=0 && next.y<20) {
                int risk = terrain.getRisk(next);
                dirRisk.push_back({dir, risk});
            }
        }
        sort(dirRisk.begin(), dirRisk.end(), 
             [](const auto& a, const auto& b) { return a.second < b.second; });
        
        for(const auto& [dir, risk] : dirRisk) {
            Point next = current + dir;
            int nextTotalRisk = currentRisk + risk;
            
            if(nextTotalRisk < minRisk[next.x][next.y]) {
                minRisk[next.x][next.y] = nextTotalRisk;
                prev[next.x][next.y] = current;
                visited[next.x][next.y] = true;
                dfs(terrain, next, target, visited, minRisk, prev, nextTotalRisk);
            }
        }
    }
    
    vector<Point> reconstructPath(const vector<vector<Point>>& prev, 
                                 Point start, Point target) {
        vector<Point> path;
        Point current = target;
        
        while(!(current == start)) {
            path.push_back(current);
            current = prev[current.x][current.y];
            if(current.x == -1) return {}; // 路径不可达
        }
        
        path.push_back(start);
        reverse(path.begin(), path.end());
        return path;
    }
};

int main(int argc, char* argv[]) {
    bool testMode = argc > 1 && string(argv[1]) == "--test";
    
    ChangshaTerrain terrain;
    StealthPathFinder pathFinder;
    
    if(testMode) {
        cout << "==== 长沙战区路径规划测试 ====\n";
        terrain.display();
    }
    
    // 从指挥部(1,1)到目标点(18,18)
    Point start = {1, 1};
    Point target = {18, 18};
    
    auto path = pathFinder.findPath(terrain, start, target);
    
    if(path.empty()) {
        cerr << "警告：无法规划安全路径！" << endl;
        return 1;
    }
    
    cout << "\n作战路线规划完成！路径长度: " << path.size() << endl;
    
    if(testMode) {
        cout << "安全路径: ";
        for(auto& p : path) {
            cout << "(" << p.x << "," << p.y << ") ";
        }
        cout << endl;
        
        // 计算路径总风险
        int totalRisk = 0;
        for(const auto& p : path) {
            totalRisk += terrain.getRisk(p);
        }
        cout << "路径总风险值: " << totalRisk << endl;
    }
    
    return 0;
}