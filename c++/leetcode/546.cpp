#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <cstring>

using namespace std;

class Solution {
public:
    int removeBoxes(vector<int>& boxes) {
		memset(dp, 0, sizeof(dp));
        return remove(boxes, 0, boxes.size()-1);
    }
	
	int inline remove(const vector<int>& box, int l, int r) {
		// 结束条件
		if (l > r) return 0;
		if (dp[l][r][0] != 0) return dp[l][r][0];
		if (l == r) {
            updatedp(l, r, 1, 1);
			return 1;
		}
		
		int same = 0;
		for (int i = l; i < r; i++) {
			if (box[l] == box[i]) same += 1;
			updatedp(l, r, remove(box, l, i) + remove(box, i+1, r), same);
			if (box[l] == box[i+1]) {
				int lsame = dp[l][i][1];
				int rsame = dp[i+1][r][1];
				int newscore = dp[l][i][0] + dp[i+1][r][0] 
					+ (lsame+rsame)*(lsame+rsame)-lsame*lsame-rsame*rsame;
				updatedp(l, r, newscore, lsame+rsame);
			}
		}
		// printf("[%d,%d]: %d, %d\n", l, r, dp[l][r][0], dp[l][r][1]);
		return dp[l][r][0];
	}

	static int dp[100][100][2];
    inline void updatedp(int l, int r, int score, int same) {
		if (score > dp[l][r][0]) {
			// printf("update score: [%d,%d]: %d, %d\n", l, r, score, same);
			dp[l][r][0] = score;
			dp[l][r][1] = same;
		}
	}
};

int Solution::dp[100][100][2];

int main() {
	// vector<int> a{1};
	// int expected = 1;
	// vector<int> a{1,2};
	// int expected = 2;
	// vector<int> a{1,1};
	// int expected = 4;
	// vector<int> a{1,1,2};
	// int expected = 5;
	// vector<int> a{1,2,1};
	// int expected = 5;
	// vector<int> a{1,1,2,3,1};
	// int expected = 11;
	// vector<int> a{1,1,1,1};
	// int expected = 16;
	vector<int> a{3, 2, 2, 2, 3, 4, 3};
	int expected = 19;
	// vector<int> a{1, 2, 1, 2, 1, 1, 2, 1, 1};
	// int expected = 19;
	Solution s;
	int score = s.removeBoxes(a);
	printf("score: %d, expected: %d\n", score, expected);
	return 0;
}