#ifndef BCS_DEFINES_H
#define BCS_DEFINES_H

// 长沙关键坐标
constexpr double YUELU_MOUNTAIN_LAT = 28.18;
constexpr double YUELU_MOUNTAIN_LON = 112.93;
constexpr double XIANG_RIVER_LAT = 28.20;
constexpr double XIANG_RIVER_LON = 112.97;
constexpr double HUANGHUA_AIRPORT_LAT = 28.19;
constexpr double HUANGHUA_AIRPORT_LON = 113.22;

// 军事单位类型
enum class MilitaryUnit {
    INFANTRY,    // 步兵
    TANK,        // 坦克
    ARTILLERY,   // 火炮
    DRONE,       // 无人机
    RADAR,       // 雷达站
    AIR_DEFENSE  // 防空系统
};

// 战场风险等级
enum class ThreatLevel {
    SAFE = 1,        // 安全区
    RADAR_COVERED,   // 雷达覆盖区
    WATER_SURFACE,   // 水面区域
    AIR_DEFENSE_ZONE // 防空区域
};

#endif // BCS_DEFINES_H