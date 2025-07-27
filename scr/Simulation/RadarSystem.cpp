#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <map>
#include "Defines.h"

using namespace std;

// 辅助函数：计算两点间距离
double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
    // 简化的平面距离计算 (公里)
    return sqrt(pow(lat1-lat2, 2) + pow(lon1-lon2, 2)) * 100;
}

class RadarSystem {
private:
    // 雷达参数
    string name;
    double frequency; // GHz
    double power;     // kW
    double lat;       // 纬度
    double lon;       // 经度
    double efficiency; // 雷达效率系数
    
    // 干扰源结构体
    struct Jammer {
        string name;
        double lat;
        double lon;
        double power;    // 干扰功率 (kW)
        double frequency; // 干扰频率 (GHz)
        Jammer(string n, double la, double lo, double p, double f) 
            : name(n), lat(la), lon(lo), power(p), frequency(f) {}
    };
    vector<Jammer> jammers;
    
    // 地形衰减因子
    map<ThreatLevel, double> terrainFactors;

public:
    RadarSystem(string name, double freq, double pwr, double latitude, double longitude) 
        : name(name), frequency(freq), power(pwr), 
          lat(latitude), lon(longitude), efficiency(0.85) {
        
        // 初始化地形因子 (C++11 兼容方式)
        terrainFactors[ThreatLevel::SAFE] = 1.0;
        terrainFactors[ThreatLevel::RADAR_COVERED] = 1.2;
        terrainFactors[ThreatLevel::WATER_SURFACE] = 0.8;
        terrainFactors[ThreatLevel::AIR_DEFENSE_ZONE] = 1.1;
    }
    
    void addJammer(string jammerName, double latitude, double longitude, 
                  double jamPower, double jamFreq) {
        jammers.push_back(Jammer(jammerName, latitude, longitude, jamPower, jamFreq));
    }
    
    void setEfficiency(double eff) {
        efficiency = eff;
    }
    
    double detectProbability(double targetLat, double targetLon, 
                            double targetRCS, ThreatLevel terrainType) {
        // 计算距离
        double dist = calculateDistance(lat, lon, targetLat, targetLon);
        if (dist < 0.1) dist = 0.1;  // 防止除零错误
        
        // 基础探测概率 (雷达方程简化版)
        double baseProb = (power * targetRCS * efficiency) / pow(dist, 1.5);
        baseProb = min(0.95, baseProb);  // 上限限制
        
        // 干扰影响
        for(size_t i = 0; i < jammers.size(); i++) {
            const Jammer& jammer = jammers[i];
            double jamDist = calculateDistance(jammer.lat, jammer.lon, targetLat, targetLon);
            if (jamDist < 0.1) jamDist = 0.1;  // 防止除零错误
            
            double freqMatch = 1.0 - 0.1 * fabs(frequency - jammer.frequency);
            double jamEffect = (jammer.power * freqMatch) / pow(jamDist, 1.2);
            baseProb *= max(0.2, 1.0 - jamEffect);
        }
        
        // 地形影响
        if (terrainFactors.find(terrainType) != terrainFactors.end()) {
            baseProb *= terrainFactors[terrainType];
        }
        
        // 概率范围限制
        baseProb = max(0.0, min(1.0, baseProb));
        return baseProb;
    }
    
    void displayStatus() const {
        cout << "\n==== " << name << " 状态报告 ====\n";
        cout << "位置: (" << fixed << setprecision(4) << lat << ", " << lon << ")\n";
        cout << "频率: " << frequency << " GHz\n";
        cout << "功率: " << power << " kW\n";
        cout << "效率: " << fixed << setprecision(1) << efficiency * 100 << "%\n";
        
        if(!jammers.empty()) {
            cout << "\n当前干扰源:\n";
            for(size_t i = 0; i < jammers.size(); i++) {
                const Jammer& jammer = jammers[i];
                cout << "- " << jammer.name << " (位置: " << jammer.lat << ", " << jammer.lon
                     << ", 功率: " << jammer.power << " kW, 频率: "
                     << jammer.frequency << " GHz)\n";
            }
        }
        cout << "==============================\n";
    }
};

int main() {
    // 创建岳麓山雷达站
    RadarSystem yueluRadar("岳麓山雷达站", 5.8, 100, 
                          YUELU_MOUNTAIN_LAT, YUELU_MOUNTAIN_LON);
    yueluRadar.setEfficiency(0.9);
    
    // 添加干扰源 (敌方电子战单位)
    yueluRadar.addJammer("电子干扰车1", 28.17, 112.90, 50, 5.6);
    yueluRadar.addJammer("无人机干扰器", 28.19, 112.92, 30, 5.8);
    
    // 显示雷达状态
    yueluRadar.displayStatus();
    
    // 目标结构体定义
    struct Target {
        string name;
        double lat;
        double lon;
        double rcs;
        ThreatLevel terrain;
        Target(string n, double la, double lo, double r, ThreatLevel t) 
            : name(n), lat(la), lon(lo), rcs(r), terrain(t) {}
    };
    
    // 模拟不同目标
    vector<Target> targets;
    targets.push_back(Target("坦克连队", 28.18, 112.94, 15.0, ThreatLevel::RADAR_COVERED));
    targets.push_back(Target("运输船队", 28.20, 112.97, 8.0, ThreatLevel::WATER_SURFACE));
    targets.push_back(Target("无人机群", 28.19, 112.95, 2.0, ThreatLevel::SAFE));
    
    cout << "\n==== 目标探测分析 ====\n";
    cout << "-------------------------------------------------\n";
    cout << "| 目标名称     | 位置             | RCS   | 探测概率 |\n";
    cout << "-------------------------------------------------\n";
    
    for(size_t i = 0; i < targets.size(); i++) {
        const Target& target = targets[i];
        double detectProb = yueluRadar.detectProbability(
            target.lat, target.lon, target.rcs, target.terrain);
        
        cout << "| " << left << setw(12) << target.name
             << " | " << fixed << setprecision(4) << target.lat << ", " << setw(6) << left << target.lon
             << " | " << setw(5) << fixed << setprecision(1) << target.rcs
             << " | " << setw(7) << fixed << setprecision(1) << detectProb*100 << "% |\n";
    }
    cout << "-------------------------------------------------\n";
    
    return 0;
}