// 32. 最长有效括号
//https://leetcode-cn.com/submissions/detail/306836645/
class Solution {
public:
    int longestValidParentheses(string s) {
        // vector<int> v;
        vector<int> v(s.size()+1);
        for (auto i = 0; i < s.size(); i++) {
            if (s[i] == '(') {
                // v.push_back(-1);
                v[i] = -1;
            }
            else {
                // 找到上一个-1，改成1，并push_back(1)；否则push_back(-2)
                // auto it2 = find(v.rbegin(), v.rend(), -1);
                int last = -1;
                for (auto j = i-1; j >= 0; j--) {
                    if (v[j] == -1) {
                        last = j;
                        break;
                    }
                }
                // if (it2 == v.rend()) {
                if (last == -1) {
                    // v.push_back(-2);
                    v[i] = -2;
                }
                else {
                    // *it2 = 1;
                    v[last] = 1;
                    // v.push_back(1);
                    v[i] = 1;
                }
            }
        }
        // 遍历v，计算最长的连续1
        int max = 0;
        int cur = 0;
        // v.push_back(-1);
        for (auto i: v) {
            if (i == 1) {
                cur += 1;
            }
            else {
                // 更新max
                if (cur > max) {
                    max = cur;
                }
                cur = 0;
            }
        }

        return max;
    }
};
