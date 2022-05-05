class Solution {
public:
    vector<vector<string>> solveNQueens(int n) {
        vector<vector<int>> rs;
        vector<int> r(1, -1);
        while (next(n, r)) {
            // printr(r);
            if (legal(r)) { // 只保留最后一个不合法的
                if (r.size() == n) {
                    rs.push_back(r);
                } else {
                    r.push_back(-1);
                }
            }
        }
        return to_results(n, rs);
    }

    // 获取下一个可能。无论是否合法
    bool next(int n, vector<int>& r) {
        for (auto it = r.rbegin(); it != r.rend(); it++) {
            if (*it < n-1) {
                *it += 1;
                return true;
            } else {
                *it = 0;
            }
        }
        return false;
    }

    // 返回是r是否合法。如果不合法，只保留第一个不合法的值。
    bool legal(vector<int>& r) {
        for (int i = 0; i < r.size(); i++) {
            for (int j = 0; j < i; j++) {
                if (r[i] == r[j] || (r[i]-r[j]) == (i-j) || (r[i]-r[j]) == (j-i)) {
                    r.resize(i+1);
                    return false;
                }
            }
        }
        return true;
    }

    vector<vector<string>> to_results(int n, const vector<vector<int>>& rs) {
        vector<vector<string>> results(rs.size(), vector(n, string(n, '.')));
        for (int i = 0; i < rs.size(); i++) {
            for (int j = 0; j < n; j++) {
                results[i][j][rs[i][j]] = 'Q';
            }
        }
        return results;
    }

    // -----------------------

    void printr(const vector<int> r) {
        cout << "r:";
        for (auto i: r) {
            cout << i << ",";
        }
        cout << "\n";
    }

    void printres(const vector<string>& res) {
        cout << "\n";
        for (auto s: res) {
            cout << s << "\n";
        }
    }

};

