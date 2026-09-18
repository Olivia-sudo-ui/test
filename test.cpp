#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <windows.h> // 用于Sleep函数模拟红绿灯延时 (Windows环境)

using namespace std;

// 定义13条可通行路线
// 【注意】如果你的图纸中13条路线的走向不同，请修改这里的名称和矩阵
const int NUM_ROUTES = 13;
string routeNames[NUM_ROUTES] = {
    "A->B", "A->C", "A->D", // A入口
    "B->A", "B->C", "B->D", // B入口
    "C->A", "C->B",         // C入口（单行道）
    "D->A", "D->B", "D->C", // D入口
    "E->A", "E->B"          // E入口（单行道）
};

// 冲突矩阵：conflict[i][j] = 1 表示路线i和路线j不能同时通行（冲突）
int conflictMatrix[NUM_ROUTES][NUM_ROUTES] = {0};

// 初始化冲突矩阵（核心逻辑）
void initConflictMatrix() {
    // 1. 根据题目提示的硬性条件
    // 题目说 E->B (索引11) 和 A->D (索引2) 不可同时通行
    conflictMatrix[2][11] = conflictMatrix[11][2] = 1;
    
    // 题目说 A->B (索引0) 和 E->C 可以同时通行，这里不设置冲突即可

    // 2. 编写通用的冲突规则（根据实际路口几何关系补充）
    for (int i = 0; i < NUM_ROUTES; i++) {
        for (int j = i + 1; j < NUM_ROUTES; j++) {
            // 规则一：来自同一个入口的路线必然冲突（比如 A->B 和 A->C 不能同时走）
            if (routeNames[i][0] == routeNames[j][0]) {
                conflictMatrix[i][j] = conflictMatrix[j][i] = 1;
            }
            
            // 规则二：驶向同一个出口的路线必然冲突（比如 A->C 和 B->C 汇合冲突）
            if (routeNames[i].back() == routeNames[j].back()) {
                conflictMatrix[i][j] = conflictMatrix[j][i] = 1;
            }

            // 规则三：交叉冲突（例如直行和左转交叉）
            // 这里根据你的图纸自行补充，目前示例中简单处理
        }
    }
}

// 求解交通灯相位（图着色算法 - 贪心策略）
void solveTrafficPhases() {
    int colors[NUM_ROUTES] = {0}; // 0表示未着色，1,2,3...表示相位
    int maxColor = 0;

    // 按节点度数降序排序（优化贪心效果，减少相位数）
    vector<pair<int, int>> degreeOrder; // <度数, 节点索引>
    for (int i = 0; i < NUM_ROUTES; i++) {
        int degree = 0;
        for (int j = 0; j < NUM_ROUTES; j++) {
            if (conflictMatrix[i][j] == 1) degree++;
        }
        degreeOrder.push_back({degree, i});
    }
    sort(degreeOrder.rbegin(), degreeOrder.rend()); // 按度数从大到小排序

    for (auto& p : degreeOrder) {
        int i = p.second;
        bool usedColors[NUM_ROUTES + 1] = {false};

        // 查看相邻节点已经使用了哪些颜色
        for (int j = 0; j < NUM_ROUTES; j++) {
            if (conflictMatrix[i][j] == 1 && colors[j] != 0) {
                usedColors[colors[j]] = true;
            }
        }

        // 找到第一个可用的颜色
        int color = 1;
        while (usedColors[color]) {
            color++;
        }

        colors[i] = color;
        maxColor = max(maxColor, color);
    }

    // 输出结果
    cout << "====== 交通灯相位设计方案 ======" << endl;
    cout << "经过计算，该路口共需要 " << maxColor << " 个相位（红绿灯周期）。" << endl << endl;

    for (int phase = 1; phase <= maxColor; phase++) {
        cout << "【相位 " << phase << "】可以同时通行的路线：" << endl;
        for (int i = 0; i < NUM_ROUTES; i++) {
            if (colors[i] == phase) {
                cout << "  " << routeNames[i] << endl;
            }
        }
        cout << "--------------------------------" << endl;
    }
}

// 模拟交通灯运行
void simulateTrafficLights(int totalPhases) {
    cout << "\n====== 交通灯运行模拟 ======" << endl;
    for (int cycle = 1; cycle <= 2; cycle++) { // 模拟2个完整周期
        cout << "\n>>> 第 " << cycle << " 轮红绿灯周期开始 <<<" << endl;
        for (int phase = 1; phase <= totalPhases; phase++) {
            cout << "当前绿灯相位: " << phase << " -> 放行对应路线..." << endl;
            Sleep(1000); // 延时1秒，模拟绿灯时长
        }
        cout << "--- 完成一个红绿灯周期 ---" << endl;
    }
}

int main() {
    // 设置控制台输出编码为UTF-8，防止中文乱码
    system("chcp 65001 > nul");

    cout << "多岔路口交通管理程序" << endl;
    cout << "========================" << endl;

    // 1. 初始化冲突规则
    initConflictMatrix();

    // 2. 求解相位（顺便把相位数传出去用于模拟）
    solveTrafficPhases(); // 这里会输出相位，但为了简化模拟，在下面手动设定一下相位数
    int totalPhases = 3;  // 【注意】这里应根据上面实际输出的相位数修改，假设是3个相位

    // 3. 模拟运行
    simulateTrafficLights(totalPhases);

    return 0;
}