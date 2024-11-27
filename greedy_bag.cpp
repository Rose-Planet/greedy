#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <fstream>  // 用于文件操作

using namespace std;

// 物品结构体
struct Item {
    int value;  // 物品的价值
    int weight; // 物品的重量
    float unitValue; // 单位重量的价值 (价值/重量)
    float ratio;  // 物品被选中的比例 (0 ≤ xi ≤ 1)
};

// 根据贪心策略 - 贪价值选择物品
float greedyByValue(vector<Item>& items, int capacity) {
    int currentWeight = 0;
    float totalValue = 0.0;

    for (auto& item : items) {
        if (currentWeight + item.weight <= capacity) {
            // 如果物品能完全放入背包
            totalValue += item.value;
            currentWeight += item.weight;
            item.ratio = 1.0; // 完全放入背包
        } else {
            // 如果物品不能完全放入背包，取物品的一部分
            int remainingCapacity = capacity - currentWeight;
            totalValue += item.value * (float)remainingCapacity / item.weight;
            item.ratio = (float)remainingCapacity / item.weight; // 部分放入背包
            break;
        }
    }

    return totalValue;
}

// 根据贪心策略 - 贪重量选择物品
float greedyByWeight(vector<Item>& items, int capacity) {
    int currentWeight = 0;
    float totalValue = 0.0;

    for (auto& item : items) {
        if (currentWeight + item.weight <= capacity) {
            // 如果物品能完全放入背包
            totalValue += item.value;
            currentWeight += item.weight;
            item.ratio = 1.0; // 完全放入背包
        } else {
            // 如果物品不能完全放入背包，取物品的一部分
            int remainingCapacity = capacity - currentWeight;
            totalValue += item.value * (float)remainingCapacity / item.weight;
            item.ratio = (float)remainingCapacity / item.weight; // 部分放入背包
            break;
        }
    }

    return totalValue;
}

// 根据贪心策略 - 贪单位重量价值选择物品
float greedyByUnitValue(vector<Item>& items, int capacity) {
    // 按单位重量价值降序排序
    sort(items.begin(), items.end(), [](const Item& a, const Item& b) {
        return a.unitValue > b.unitValue;
    });

    int currentWeight = 0;
    float totalValue = 0.0;

    for (auto& item : items) {
        if (currentWeight + item.weight <= capacity) {
            // 如果物品能完全放入背包
            totalValue += item.value;
            currentWeight += item.weight;
            item.ratio = 1.0; // 完全放入背包
        } else {
            // 如果物品不能完全放入背包，取物品的一部分
            int remainingCapacity = capacity - currentWeight;
            totalValue += item.value * (float)remainingCapacity / item.weight;
            item.ratio = (float)remainingCapacity / item.weight; // 部分放入背包
            break;
        }
    }

    return totalValue;
}

// 写入带有 BOM 的 UTF-8 编码的 CSV 文件
void writeResultsToCSV(const string& filename, const string& strategy, float maxValue, const vector<Item>& items) {
    ofstream outFile(filename, ios::binary | ios::app); // 以二进制方式打开文件，追加模式
    if (outFile.is_open()) {
        // 写入 UTF-8 BOM
        outFile.put(0xEF);
        outFile.put(0xBB);
        outFile.put(0xBF);

        // 写入策略、最大价值、物品选择比例
        outFile << strategy << ", " << fixed << setprecision(2) << maxValue << ", (";
        for (size_t i = 0; i < items.size(); ++i) {
            outFile << fixed << setprecision(2) << items[i].ratio;
            if (i < items.size() - 1) outFile << ", ";
        }
        outFile << ")" << endl;
    } else {
        cout << "无法打开文件!" << endl;
    }
}

int main() {
    int capacity = 20;  // 背包容量为 20
    // 物品的价值和重量
    vector<Item> items = {
        {25, 18, 0, 0},  // 物品 1，价值 25，重量 18
        {24, 15, 0, 0},  // 物品 2，价值 24，重量 15
        {15, 10, 0, 0}   // 物品 3，价值 15，重量 10
    };

    // 计算每个物品的单位重量价值
    for (auto& item : items) {
        item.unitValue = (float)item.value / item.weight;
    }

    // 创建并初始化CSV文件，写入表头
    ofstream outFile("greedy_bag_results.csv", ios::binary);
    if (outFile.is_open()) {
        // 写入 UTF-8 BOM
        outFile.put(0xEF);
        outFile.put(0xBB);
        outFile.put(0xBF);

        outFile << "贪心策略,最大价值,物品选择比例" << endl;
        outFile.close();
    } else {
        cout << "无法创建CSV文件!" << endl;
        return 1;
    }

    // 贪心法 - 贪价值
    vector<Item> itemsCopy = items; // 复制原始物品信息
    float valueByValue = greedyByValue(itemsCopy, capacity);
    writeResultsToCSV("greedy_bag_results.csv", "贪价值", valueByValue, itemsCopy);

    // 贪心法 - 贪重量
    for (auto& item : itemsCopy) item.ratio = 0;
    float valueByWeight = greedyByWeight(itemsCopy, capacity);
    writeResultsToCSV("greedy_bag_results.csv", "贪重量", valueByWeight, itemsCopy);

    // 贪心法 - 贪单位重量价值
    for (auto& item : itemsCopy) item.ratio = 0;
    float valueByUnitValue = greedyByUnitValue(itemsCopy, capacity);
    writeResultsToCSV("greedy_bag_results.csv", "贪单位重量价值", valueByUnitValue, itemsCopy);

    cout << "结果已保存至 'greedy_bag_results.csv' 文件中。" << endl;

    return 0;
}
