// 37. 解数独
// https://leetcode-cn.com/problems/sudoku-solver/

#include <vector>
#include <stack>
#include <iostream>

using namespace std;

class Solution
{
public:
    stack<pair<int, int>> s;

    void solveSudoku(vector<vector<char>> &b)
    {
        print(b);
        pair<int, int> pos = make_pair(0, 0);
        // 下一个填充的位置
        while (nextpos(b, pos))
        {
            // cout << "pos: " << pos.first << ", " << pos.second << "\n";
            // 不能获取下一个值
            while (!nextval(b, pos, b[pos.first][pos.second]))
            {
                // 撤销已经写入的内容
                b[pos.first][pos.second] = '.';
                // 弹出上一个
                pos = s.top();
                s.pop();
                // 取下一个值
                b[pos.first][pos.second] += 1;

                // cout << "change: " << pos.first << ", " << pos.second << ": " << b[pos.first][pos.second] << "\n";
            }
            s.push(pos);
        }
        print(b);
    }

    // 下一个空白位置'.'，含pos。成功返回true
    bool nextpos(const vector<vector<char>> &b, pair<int, int> &pos)
    {
        for (int i = pos.first * 9 + pos.second; i < 81; i++)
        {
            int x = i / 9;
            int y = i % 9;
            if (b[x][y] == '.')
            {
                pos.first = x;
                pos.second = y;
                return true;
            }
        }
        return false;
    }

    bool nextval(const vector<vector<char>> &b, const pair<int, int> &pos, char &v)
    {
        // ':'表示没选择了
        if (v == '.')
        {
            v = '1';
        }

        for (char c = v; c < ':'; c++)
        {
            if (legal(b, pos, c))
            {
                printf("(%d,%d) %c->%c\n", pos.first, pos.second, v, c);
                v = c;
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

int main()
{
    Solution s;
    // vector<vector<char>> b{{'1', '2'}, {'3', '4'}};
    vector<vector<char>> b{{'5', '3', '.', '.', '7', '.', '.', '.', '.'}, {'6', '.', '.', '1', '9', '5', '.', '.', '.'}, {'.', '9', '8', '.', '.', '.', '.', '6', '.'}, {'8', '.', '.', '.', '6', '.', '.', '.', '3'}, {'4', '.', '.', '8', '.', '3', '.', '.', '1'}, {'7', '.', '.', '.', '2', '.', '.', '.', '6'}, {'.', '6', '.', '.', '.', '.', '2', '8', '.'}, {'.', '.', '.', '4', '1', '9', '.', '.', '5'}, {'.', '.', '.', '.', '8', '.', '.', '7', '9'}};
    s.solveSudoku(b);
    return 0;
}