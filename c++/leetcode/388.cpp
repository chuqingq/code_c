#include <string>
#include <vector>

using namespace std;

class Solution {
public:
    int lengthLongestPath(string input) {
        int pos = 0;
        int max = 0;
        int level;
		int len;
		int isfile;
		vector<int> v;
        while ((len = nextline(input, pos, level, isfile)) > 0) {
			v.resize(level);
			int cur = ((v.size() == 0) ? 0 : v[v.size()-1]) + 1 + len;
			if (!isfile) v.push_back(cur);
			else if (cur > max) max = cur;
        }
		return max>0?(max-1):0; // 去掉开头的/
    }

    int nextline(const string& input, int& pos, int& level, int& isfile) {
        // 根据pos获取接下来的一行，level表示\t的数量，返回文件名长度
		int start = pos;
		level = 0;
		isfile = 0;
		if (pos >= input.size()) return 0;
		char c;
		while (pos<=input.size() && (c = input[pos++]) != '\n')
			if (c == '\t') level += 1;
			else if (c == '.') isfile = 1;// 考虑文件名和目录名的差异
		int len = pos-start-level-1; // 去掉'\n'
		printf("nextline: start:%d, end:%d, level:%d, len:%d\n", start, pos, level, len);
		return len;
    }
};

int main() {
	string str("dir\n\tsubdir1\n\tsubdir2\n\t\tfile.ext");
	Solution s;
	int len = s.lengthLongestPath(str);
	printf("%d\n", len);
	return 0;
}
