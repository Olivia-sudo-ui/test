#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// 1. 定义13条通行路线
struct Route {
    int id;
    string name;
    int degree;
};

vector<Route> nodes = {
    {0, "A->B"}, {1, "A->C"}, {2, "A->D"},  // A入口
    {3, "B->A"}, {4, "B->C"}, {5, "B->D"},  // B入口
    {6, "D->A"}, {7, "D->B"}, {8, "D->C"},  // D入口
    {9, "E->A"}, {10, "E->B"}, {11, "E->C"}, {12, "E->D"} // E入口
};

const int N = 13;
bool adj[N][N] = {false};

// 添加冲突边
void addConflict(int u, int v) {
    adj[u][v] = adj[v][u] = true;
}

// 构建冲突图（根据教材图1.3(b)的连线关系）
void buildGraph() {
    // 0:AB, 1:AC, 2:AD
    addConflict(0, 4); addConflict(0, 5); addConflict(0, 6); addConflict(0, 7); addConflict(0, 9); addConflict(0, 10);
    addConflict(1, 4); addConflict(1, 5); addConflict(1, 6); addConflict(1, 7); addConflict(1, 9); addConflict(1, 10); addConflict(1, 11);
    addConflict(2, 4); addConflict(2, 5); addConflict(2, 6); addConflict(2, 7); addConflict(2, 9); addConflict(2, 10); addConflict(2, 11);

    // 4:BC, 5:BD
    addConflict(4, 6); addConflict(4, 7); addConflict(4, 10); addConflict(4, 11); // 注意：4与9不冲突（都在相位2）
    addConflict(5, 6); addConflict(5, 7); addConflict(5, 10); addConflict(5, 11); // 注意：5与9不冲突

    // 9:EA
    addConflict(9, 6); addConflict(9, 7); addConflict(9, 10); addConflict(9, 11);

    // 6:DA, 7:DB
    addConflict(6, 10); addConflict(6, 11); // 6与7不冲突（都在相位3）
    addConflict(7, 10); addConflict(7, 11);

    // 10:EB, 11:EC
    // 10与11不冲突（都在相位4）
}

// 计算冲突度数
void calculateDegrees() {
    for (int i = 0; i < N; ++i) {
        nodes[i].degree = 0;
        for (int j = 0; j < N; ++j) {
            if (adj[i][j]) nodes[i].degree++;
        }
    }
}

// 检查颜色是否安全
bool isSafe(int node, int color, const vector<int>& colors) {
    for (int i = 0; i < N; ++i) {
        if (adj[node][i] && colors[i] == color) return false;
    }
    return true;
}

// 回溯搜索函数
bool solve(int idx, const vector<int>& sortedNodes, int numColors, vector<int>& colors) {
    if (idx == sortedNodes.size()) return true;
    int node = sortedNodes[idx];
    for (int c = 1; c <= numColors; ++c) {
        if (isSafe(node, c, colors)) {
            colors[node] = c;
            if (solve(idx + 1, sortedNodes, numColors, colors)) return true;
            colors[node] = -1; // 回溯
        }
    }
    return false;
}

int main() {
    cout << "========== 多岔路口交通管理 - 求解过程 ==========" << endl;
    cout << "步骤1：构建图模型。将13条路线抽象为节点，冲突关系抽象为边。" << endl;
    buildGraph();
    
    cout << "步骤2：计算每个节点的冲突度数，并按度数从大到小排序。" << endl;
    calculateDegrees();
    
    vector<int> sortedNodes(N);
    for (int i = 0; i < N; ++i) sortedNodes[i] = i;
    sort(sortedNodes.begin(), sortedNodes.end(), [](int a, int b) {
        return nodes[a].degree > nodes[b].degree;
    });

    cout << "排序后的节点处理顺序（优先处理冲突最多的节点）: ";
    for (int n : sortedNodes) cout << nodes[n].name << " ";
    cout << "\n\n";

    cout << "步骤3：开始图着色搜索（采用回溯法寻找最优解）。" << endl;
    
    vector<int> colors(N, -1);
    
    // 强制将孤立节点（右转路线）初始化为0号色（常通相位）
    colors[3] = 0;  // B->A
    colors[8] = 0;  // D->C
    colors[12] = 0; // E->D
    
    // 提取剩余需要着色的节点
    vector<int> remainingNodes;
    for (int n : sortedNodes) {
        if (colors[n] == -1) remainingNodes.push_back(n);
    }

    // 尝试用1、2、3、4号颜色为剩余节点着色（加上0号色共5个相位）
    bool found = false;
    for (int numColors = 1; numColors <= 4; ++numColors) {
        fill(colors.begin(), colors.end(), -1);
        colors[3] = 0; colors[8] = 0; colors[12] = 0; // 每次重置时保留0号色
        
        if (solve(0, remainingNodes, numColors, colors)) {
            found = true;
            break;
        }
    }

    if (found) {
        cout << "步骤4：搜索完成！成功找到最少相位分配。\n" << endl;

        cout << "========== 最终相位分配结果 ==========" << endl;
        for (int c = 0; c <= 4; ++c) {
            if (c == 0) {
                cout << "相位 0 (0号色灯 / 常通相位，右转车辆无冲突，常亮):" << endl;
            } else {
                cout << "相位 " << c << " (" << c << "号色灯 / 轮转相位):" << endl;
            }
            
            for (int i = 0; i < N; ++i) {
                if (colors[i] == c) {
                    cout << "  - " << nodes[i].name << endl;
                }
            }
            cout << "-------------------------------------------" << endl;
        }
    } else {
        cout << "步骤4：搜索失败，未能找到合适的相位分配。" << endl;
    }
    
    return 0;
}