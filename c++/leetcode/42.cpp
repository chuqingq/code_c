// 42. 接雨水
// https://leetcode-cn.com/problems/trapping-rain-water/
class Solution {
public:
    int trap(vector<int>& height) {
        int i = 0;
        int j = height.size()-1;
        int val = 0;
        vector<int> h(height.size(), 0);

        for (; i <= j; ) {
            if (height[i] <= height[j]) {
                if (val < height[i]) {
                    val = height[i];
                }
                h[i] = val;
                i++;
            } else {
                if (val < height[j]) {
                    val = height[j];
                }
                h[j] = val;
                j--;
            }
        }

        int result = 0;
        for (int i = 0; i < height.size(); i++) {
            result += h[i]-height[i];
        }

        return result;
    }
};
