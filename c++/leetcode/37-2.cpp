// 37. 解数独
// https://leetcode-cn.com/problems/sudoku-solver/
class Solution
{
public:

    void solveSudoku(vector<vector<char>> &b)
    {
        pair<int, int> p = make_pair(0, 0);
        stack<pair<int, int>> s;
        // 下一个填充的位置
        while (nextpos(b, p))
        {
            printpos(p);
            // 不能获取下一个值
            while (!nextval(b, p))
            {
                // 撤销已经写入的内容
                b[p.first][p.second] = '.';
                // 弹出上一个
                p = s.top();
                s.pop();
            }
            cout << "val:" << b[p.first][p.second] << "\n";
            s.push(p);
            print(b);
        }
    }

    // 从p起寻找第一个.，含p。返回寻找成功。
    bool nextpos(const vector<vector<char>> &b, pair<int, int> &p)
    {
        int x = p.first;
        int y = p.second;
        for (int i = x; i < b.size(); i++) {
            for (int j = y; j < b.size(); j++) {
                if (b[i][j] == '.') {
                    p.first = i;
                    p.second = j;
                    return true;
                }
            }
            y = 0;
        }
        return false;
    }

    // 把p更新为下一个可行的值，不含当前值。返回更新成功
    bool nextval(vector<vector<char>> &b, const pair<int, int> &p)
    {
        // ':'表示没选择了
        char v = b[p.first][p.second];
        if (v == '.')
        {
            v = '1'-1;
        }

        for (char c = v+1; c < ':'; c++)
        {
            if (legal(b, p, c))
            {
                // printf("(%d,%d) %c->%c\n", pos.first, pos.second, v, c);
                b[p.first][p.second] = c;
                return true;
            }
        }
        return false;
    }

    bool legal(const vector<vector<char>> &b, const pair<int, int> &pos, const char &v)
    {
        // 行
        for (int i = 0; i < 9; i++)
        {
            if (i != pos.second && b[pos.first][i] == v)
            {
                return false;
            }
        }
        // 列
        for (int i = 0; i < 9; i++)
        {
            if (i != pos.first && b[i][pos.second] == v)
            {
                return false;
            }
        }
        // 块
        int x = (pos.first / 3) * 3;
        int y = (pos.second / 3) * 3;
        for (int i = x; i < x + 3; i++)
        {
            for (int j = y; j < y + 3; j++)
            {
                if ((i != pos.first || j != pos.second) && b[i][j] == v)
                {
                    return false;
                }
            }
        }
        // 合法
        return true;
    }

    void printpos(const pair<int, int> p) {
        cout << "pos: " << p.first << "," << p.second << "\n";
    }

    void print(const vector<vector<char>> &b)
    {
        for (auto i : b)
        {
            for (auto j : i)
            {
                cout << j << " ";
            }
            cout << "\n";
        }
        cout << "\n";
    }
};
