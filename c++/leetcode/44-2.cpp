// 44. 通配符匹配
// https://leetcode-cn.com/problems/wildcard-matching/
class Solution {
public:
    bool isMatch(string s, string p) {
        vector<string> ps = stringSplit(p, '*');
        if (!stripleft(s, ps)) {
            return false;
        }
        if (!stripright(s, ps)) {
            return false;
        }

        // 匹配ps
        int i = 0;
        for (auto p: ps) {
            if (p == "*") {
                continue;
            }
            if (i+p.size() > s.size()) {
                return false;
            }
            i = match(s, i, p);
            // cout << "p:" << p << ",i:" << i << "\n";
            if (i == string::npos) {
                return false;
            }
        }
        return true;
    }

    // 从i开始，非严格匹配带?的p。非严格表示起点不确定
    int match(const string& s, int i, const string& p) {
        for (; i < s.size()-p.size()+1; i++) {
            int ret = match_exact(s, i, p);
            if (ret != string::npos) {
                return ret;
            }
        }
        return string::npos;
    }

    // 从i开始严格匹配带?的p
    int match_exact(const string& s, int i, const string& p) {
        if (s.size() < i+p.size()) {
            return string::npos;
        }
        for (int j = 0; j < p.size(); j++) {
            if (p[j] != '?' && p[j] != s[i+j]) {
                return string::npos;
            }
        }
        return i+p.size();
    }

    // ps 从右到左，匹配掉不为*的内容。确保ps后其实还有个*。true表示操作成功，false表示操作失败、isMatch可以直接失败。
    bool stripleft(string& s, vector<string>& ps) {
        while (!ps.empty()) {
            string front = ps.front();
            if (front == "*") {
                return true;
            }
            if (front.size() > s.size()) {
                return false;
            }
            // TODO
            int ret = match_exact(s, 0, front);
            if (ret == string::npos) {
                return false;
            }
            s = s.substr(front.size(), s.size()-front.size());
            ps.erase(ps.begin());
        }
        if (ps.empty()) {
            return s.empty();
        }
        return true;
    }

    // ps 从右到左，匹配掉不为*的内容。确保ps后其实还有个*。true表示操作成功，false表示操作失败、isMatch可以直接失败。
    bool stripright(string& s, vector<string>& ps) {
        while (!ps.empty()) {
            string back = ps.back();
            if (back == "*") {
                return true;
            }
            if (back.size() > s.size()) {
                return false;
            }
            // TODO
            int ret = match_exact(s.substr(s.size()-back.size(), back.size()), 0, back);
            if (ret == string::npos) {
                return false;
            }
            s = s.substr(0, s.size()-back.size());
            ps.pop_back();
        }
        if (ps.empty()) {
            return s.empty();
        }
        return true;
    }

    std::vector<std::string> stringSplit(const std::string& str, char delim) {
        int from = 0;
        std::vector<std::string> elems;
        for (int i = 0; i < str.size(); i++) {
        if (str[i] == delim) {
                if (i > from) {
                    elems.push_back(str.substr(from, i-from));
                }
                elems.push_back(str.substr(i, 1));
                from = i+1;
        }
        }
        if (str.size() > from) {
            elems.push_back(str.substr(from, str.size()-from));
        }
        return elems;
    }

    void print_vector(const vector<string>& v) {
        for (auto i: v) {
            cout << i << " ";
        }
        cout << "\n";
    }
};

// "abcabczzzde"
// "*abc???de*"
// "abbabaaabbabbaababbabbbbbabbbabbbabaaaaababababbbabababaabbababaabbbbbbaaaabababbbaabbbbaabbbbababababbaabbaababaabbbababababbbbaaabbbbbabaaaabbababbbbaababaabbababbbbbababbbabaaaaaaaabbbbbaabaaababaaaabb"
// "*aa*ba*a*bb*aa*ab*a*aaaaaa*a*aaaa*bbabb*b*b*aaaaaaaaa*a*ba*bbb*a*ba*bb*bb*a*b*bb"
// "bbbaaabaababbabbbaabababbbabababaabbaababbbabbbabb"
// "*b**b***baba***aaa*b***"
// "aa"
// "aa"
// "aaabbbaabaaaaababaabaaabbabbbbbbbbaabababbabbbaaaaba"
// "a*******b"
// ""
// "******"
// "mississippi"
// "m??*ss*?i*pi"
// "aa"
// "a"
// "aa"
// "*"
// "cb"
// "?a"
// "acdcb"
// "a*c?b"
// "adceb"
// "*a*b"
