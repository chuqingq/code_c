// 42. 接雨水
// https://leetcode-cn.com/problems/trapping-rain-water/
class Solution {
public:
    int trap(vector<int>& height) {
        int val = 0;
        int result = 0;
        for (int i = 0, j = height.size()-1; i <= j; ) {
            if (height[i] <= height[j]) {
                if (val < height[i]) {
                    val = height[i];
                }
                result += val - height[i++];
            } else {
                if (val < height[j]) {
                    val = height[j];
                }
                result += val - height[j--];
            }
        }
        return result;
    }
};
