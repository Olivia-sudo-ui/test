#include <stdio.h>
#include <string.h>

// 总通行路线数量
#define ROUTE_NUM 13

int main()
{
    // 13条可通行路线名称
    const char *routes[ROUTE_NUM] = {
        "A→B", "A→C", "A→D",
        "B→A", "B→C", "B→D",
        "D→A", "D→B", "D→C",
        "E→A", "E→B", "E→C", "E→D"
    };

    // 冲突邻接矩阵：1=路线冲突不可同时通行，0=无冲突可同时通行
    int conflict[ROUTE_NUM][ROUTE_NUM] = {
        {0,0,0, 0,1,1, 1,1,0, 0,1,0,0},   // A→B
        {0,0,0, 1,1,1, 1,0,0, 1,1,0,0},   // A→C
        {0,0,0, 1,0,0, 0,0,0, 0,1,1,0},   // A→D
        {0,1,1, 0,0,0, 0,1,1, 0,1,1,1},   // B→A
        {1,1,0, 0,0,0, 1,1,0, 1,0,0,1},   // B→C
        {1,1,0, 0,0,0, 0,0,0, 0,1,0,0},   // B→D
        {1,1,0, 0,1,0, 0,0,0, 1,0,0,0},   // D→A
        {1,0,0, 1,1,0, 0,0,0, 1,0,0,1},   // D→B
        {0,0,0, 1,0,0, 0,0,0, 0,0,0,0},   // D→C
        {0,1,0, 0,1,0, 1,1,0, 0,0,0,0},   // E→A
        {1,1,1, 1,0,1, 0,0,0, 0,0,0,0},   // E→B
        {0,0,1, 1,0,0, 0,0,0, 0,0,0,0},   // E→C
        {0,0,0, 1,1,0, 0,1,0, 0,0,0,0}    // E→D
    };

    int degrees[ROUTE_NUM];   // 每个顶点（路线）的度数（冲突数）
    int vertices[ROUTE_NUM];  // 顶点索引，用于按度数排序
    int color[ROUTE_NUM];     // 着色结果，-1表示未着色

    // 1. 计算每个顶点的度数
    for (int i = 0; i < ROUTE_NUM; i++) {
        degrees[i] = 0;
        for (int j = 0; j < ROUTE_NUM; j++) {
            degrees[i] += conflict[i][j];
        }
        vertices[i] = i; // 初始化顶点索引
    }

    // 2. 按度数从大到小排序顶点索引（选择排序）
    for (int i = 0; i < ROUTE_NUM - 1; i++) {
        int max_idx = i;
        for (int j = i + 1; j < ROUTE_NUM; j++) {
            if (degrees[vertices[j]] > degrees[vertices[max_idx]]) {
                max_idx = j;
            }
        }
        // 交换索引
        int temp = vertices[i];
        vertices[i] = vertices[max_idx];
        vertices[max_idx] = temp;
    }

    // 3. 初始化着色数组为-1（未着色）
    memset(color, -1, sizeof(color));
    int current_color = 0;

    // 4. Welsh-Powell 贪心着色算法
    while (1) {
        // 找到第一个未着色的顶点
        int first_uncolored = -1;
        for (int i = 0; i < ROUTE_NUM; i++) {
            if (color[vertices[i]] == -1) {
                first_uncolored = vertices[i];
                break;
            }
        }
        if (first_uncolored == -1) {
            break; // 所有顶点已着色，结束
        }

        // 给第一个未着色顶点分配当前颜色
        color[first_uncolored] = current_color;

        // 给所有与已着色顶点不冲突的未着色顶点分配当前颜色
        for (int i = 0; i < ROUTE_NUM; i++) {
            int v = vertices[i];
            if (color[v] == -1) {
                int is_safe = 1;
                // 检查是否与所有已着当前色的顶点都不冲突
                for (int u = 0; u < ROUTE_NUM; u++) {
                    if (color[u] == current_color && conflict[v][u] == 1) {
                        is_safe = 0;
                        break;
                    }
                }
                if (is_safe) {
                    color[v] = current_color;
                }
            }
        }
        current_color++;
    }

    // 5. 输出信号相位方案
    printf("=== 五岔路口信号相位方案 ===\n");
    printf("最少需要 %d 个信号相位\n\n", current_color);

    for (int phase = 0; phase < current_color; phase++) {
        printf("第 %d 相位可通行路线：\n", phase + 1);
        printf("  ");
        int first_route = 1;
        for (int i = 0; i < ROUTE_NUM; i++) {
            if (color[i] == phase) {
                if (!first_route) {
                    printf("、");
                }
                printf("%s", routes[i]);
                first_route = 0;
            }
        }
        printf("\n\n");
    }

    return 0;
}