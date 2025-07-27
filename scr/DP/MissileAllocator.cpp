#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include "Defines.h"

using namespace std;

class MissileAllocator {
public:
    struct Target {
        int id;
        string name;
        int threatLevel;  // 威胁等级 (1-10)
        double hitProb;   // 单枚导弹命中概率
        int minMissiles;  // 最低所需导弹
        int maxMissiles;  // 最大可分配导弹
        
        Target(int i, string n, int tl, double hp, int minM, int maxM)
            : id(i), name(n), threatLevel(tl), hitProb(hp), minMissiles(minM), maxMissiles(maxM) {}
    };
    
    struct AllocationResult {
        vector<int> missileCounts;
        double totalThreatReduction;
    };
    
    AllocationResult allocateMissiles(const vector<Target>& targets, int totalMissiles) {
        int n = targets.size();
        
        // 初始化DP表和分配表
        vector<vector<double> > dp(n+1, vector<double>(totalMissiles+1, 0.0));
        vector<vector<vector<int> > > allocTable(n+1, 
            vector<vector<int> >(totalMissiles+1, vector<int>(n, 0)));
        
        for(int i=1; i<=n; i++) {
            const Target& t = targets[i-1];
            for(int j=0; j<=totalMissiles; j++) {
                // 默认不分配导弹给当前目标
                dp[i][j] = dp[i-1][j];
                allocTable[i][j] = allocTable[i-1][j];
                
                // 尝试分配不同数量的导弹
                int maxToAssign = min(j, t.maxMissiles);
                for(int k = t.minMissiles; k <= maxToAssign; k++) {
                    double killProb = 1.0 - pow(1.0 - t.hitProb, k);
                    double threatReduction = t.threatLevel * killProb;
                    
                    if(dp[i-1][j-k] + threatReduction > dp[i][j]) {
                        dp[i][j] = dp[i-1][j-k] + threatReduction;
                        allocTable[i][j] = allocTable[i-1][j-k];
                        allocTable[i][j][i-1] = k;
                    }
                }
            }
        }
        
        // 返回最优分配方案
        AllocationResult result;
        result.missileCounts = allocTable[n][totalMissiles];
        result.totalThreatReduction = dp[n][totalMissiles];
        return result;
    }
};

int main() {
    MissileAllocator allocator;
    
    // 敌方目标列表 (长沙战区)
    vector<MissileAllocator::Target> targets;
    targets.push_back(MissileAllocator::Target(1, "岳麓山雷达站", 9, 0.5, 2, 4));
    targets.push_back(MissileAllocator::Target(2, "湘江运输船队", 7, 0.7, 1, 3));
    targets.push_back(MissileAllocator::Target(3, "黄花机场机库", 8, 0.6, 2, 4));
    targets.push_back(MissileAllocator::Target(4, "步兵集结地", 4, 0.8, 1, 2));
    
    int totalMissiles = 8; // 可用导弹总数
    
    MissileAllocator::AllocationResult result = allocator.allocateMissiles(targets, totalMissiles);
    
    cout << "==== 长沙战区导弹火力分配方案 ====\n";
    cout << "总导弹数: " << totalMissiles << "\n";
    cout << "预计威胁消除值: " << fixed << setprecision(2) << result.totalThreatReduction << "/36\n\n";
    
    cout << "目标分配详情:\n";
    cout << "--------------------------------------------\n";
    cout << "| 目标名称       | 分配导弹 | 摧毁概率 | 威胁消除 |\n";
    cout << "--------------------------------------------\n";
    
    double totalReduction = 0.0;
    for(size_t i=0; i<targets.size(); i++) {
        const MissileAllocator::Target& t = targets[i];
        int missiles = result.missileCounts[i];
        double killProb = 1.0 - pow(1.0 - t.hitProb, missiles);
        double threatReduction = t.threatLevel * killProb;
        totalReduction += threatReduction;
        
        cout << "| " << left << setw(14) << t.name 
             << " | " << setw(8) << missiles 
             << " | " << setw(7) << fixed << setprecision(1) << killProb*100 << "%"
             << " | " << setw(8) << fixed << setprecision(2) << threatReduction
             << " |\n";
    }
    
    cout << "--------------------------------------------\n";
    cout << "| 总计          | " << setw(8) << totalMissiles 
         << " |         | " << setw(8) << fixed << setprecision(2) << totalReduction << " |\n";
    cout << "--------------------------------------------\n";
    
    return 0;
}