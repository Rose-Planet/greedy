#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono> // 用于测量运行时间

using namespace std;
using namespace chrono; // 方便使用计时相关类

// 结构体表示一个活动
struct Activity {
    int index; // 活动编号
    int start; // 活动开始时间
    int end;   // 活动结束时间
};

// 按活动结束时间升序排序
bool compareByEndTime(const Activity &a, const Activity &b) {
    return a.end < b.end;
}

// 使用贪心算法选择活动
vector<int> activitySelection(vector<Activity> &activities) {
    // 按结束时间排序
    sort(activities.begin(), activities.end(), compareByEndTime);

    vector<int> selectedActivities; // 保存选中的活动编号
    int lastEndTime = 0; // 上一个选中活动的结束时间

    for (const auto &activity : activities) {
        if (activity.start >= lastEndTime) {
            // 当前活动的开始时间不冲突，则选中该活动
            selectedActivities.push_back(activity.index);
            lastEndTime = activity.end; // 更新最后选中活动的结束时间
        }
    }

    return selectedActivities;
}

int main() {
    // 输入活动的开始时间和结束时间
    vector<Activity> activities = {
        {1, 1, 4}, {2, 3, 5}, {3, 0, 6}, {4, 5, 7},
        {5, 3, 8}, {6, 5, 9}, {7, 6, 10}, {8, 8, 11},
        {9, 8, 12}, {10, 2, 13}, {11, 12, 14}
    };

    // 开始计时
    auto startTime = high_resolution_clock::now();

    // 使用贪心算法选择活动
    vector<int> selectedActivities = activitySelection(activities);

    // 结束计时
    auto endTime = high_resolution_clock::now();

    // 计算运行时间
    auto duration = duration_cast<nanoseconds>(endTime - startTime);

    // 输出结果
    cout << "活动安排方案: ";
    for (int i : selectedActivities) {
        cout << i << " ";
    }
    cout << endl;

    // 打印运行时间
    cout << "运行时间: " << duration.count() << " 纳秒" << endl;

    return 0;
}
