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
		if (l > r) return 0;
		if (dp[l][r][0] != 0) return dp[l][r][0];
		if (l == r) return updatedp(l, r, 1, 1);
		
		for (int i = l; i < r; i++) {
			if (box[i] == box[i+1] && i != r-1) continue;
			int lscore = remove(box, l, i);
			updatedp(l, r, lscore+remove(box, i+1, r), dp[l][i][1]);
			if (box[l] == box[i+1]) {
				updatedp(l, r, dp[l][i][0]+dp[i+1][r][0]+2*dp[l][i][1]*dp[i+1][r][1], dp[l][i][1]+dp[i+1][r][1]);
			}
		}
		printf("[%d,%d]: %d, %d\n", l, r, dp[l][r][0], dp[l][r][1]);
		return dp[l][r][0];
	}

	static int dp[100][100][2];
    inline int updatedp(int l, int r, int score, int same) {
		if (score <= dp[l][r][0]) return dp[l][r][0];
		dp[l][r][0] = score;
		dp[l][r][1] = same;
		return score;
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
	// vector<int> a{3, 2, 2, 2, 3, 4, 3};
	// int expected = 19;
	// vector<int> a{1, 2, 1, 2, 1, 1, 2, 1, 1};
	// int expected = 39;
	vector<int> a{1,3,2,2,2,3,4,3,1};
	int expected = 23;
	Solution s;
	int score = s.removeBoxes(a);
	printf("score: %d, expected: %d\n", score, expected);
	return 0;
}
