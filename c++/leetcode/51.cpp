// 51. N 皇后
// https://leetcode-cn.com/problems/n-queens/

class Solution {
public:
    void printpos(pair<int, int> pos) {
        cout << "pos:" << pos.first << "," << pos.second << "\n";
    }
    void printres(const vector<string>& res) {
        cout << "\n";
        for (auto s: res) {
            cout << s << "\n";
        }
    }
    vector<vector<string>> solveNQueens(int n) {
        vector<vector<string>> results;
        // 单次结果，初始化
        vector<string> res(n, string(n, '.'));
        for (auto& r: res) {
            r[0] = 'Q';
        }
        
        // 起始位置：最后一行，第一个字符
        auto pos = make_pair(n-1, -1);
        // 不停的尝试找下一个解法
        while (next(res, pos)) {
            // printres(res);
            // 找到最先不合法的位置
            pos = illegal(res);
            // printpos(pos);
            // 如果全都合法
            if (pos.first == n) {
                results.push_back(res);
                pos = make_pair(n-1, res[n-1].find('Q'));
            }
            // 否则循环找下一个解
        }
        return results;
    }

    // res从(i,j)处选择下一个值，不管是否合法
    bool next(vector<string>& res, pair<int, int> pos) {
        if (pos.first < 0) {
            return false;
        }
        if (pos.second < (int)(res.size()-1)) {
            if (pos.second >= 0) {
                res[pos.first][pos.second] = '.';
            }
            res[pos.first][pos.second+1] = 'Q';
            return true;
        }
        if (pos.first > 0) {
            res[pos.first][pos.second] = '.';
            res[pos.first][0] = 'Q';
            pos.first -= 1;
            pos.second = res[pos.first].find('Q');
            return next(res, pos);
        }
        return false;
    }

    // 找到最先不合法的行0~n-1。n表示都合法
    pair<int, int> illegal(const vector<string>& res) {
        vector<int> d(res.size());
        for (int i = 0; i < res.size(); i++) {
            d[i] = res[i].find('Q');
            for (int j = 0; j < i; j++) {
                if (d[i] == d[j] || (d[i]-d[j]) == (j-i) || (d[i]-d[j]) == (i-j)) {
                    return make_pair(i, d[i]);
                }
            }
        }
        return make_pair(res.size(), -1);
    }

};
