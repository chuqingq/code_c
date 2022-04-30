// 41. 缺失的第一个正数
// https://leetcode-cn.com/problems/first-missing-positive/
class Solution {
public:
    vector<bool> v;
    int firstMissingPositive(vector<int>& nums) {
        v.resize(nums.size()+1);
        for (auto n: nums) {
            if (n > 0 && n < v.size()) {
                v[n] = true;
            }
        }
        for (int i = 1; i < v.size(); i++) {
            if (!v[i]) {
                return i;
            }
        }
        return v.size();
    }
};
