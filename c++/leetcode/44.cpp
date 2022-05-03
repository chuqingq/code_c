// 44. 通配符匹配
// https://leetcode-cn.com/problems/wildcard-matching/
class Solution {
public:
    bool isMatch(string s, string p) {
        vector<string> ps = stringSplit(p, '*');
        print_vector(ps);

        if (!stripright(s, ps)) {
            return false;
        }
        // cout << s.size() << " " << ps.size() << "\n";
        if (ps.empty()) {
            return s.empty();
        }
        // print_vector(ps);

        // 匹配ps
        bool star = false;
        int i = 0;
        for (auto p: ps) {
            if (p == "*") {
                star = true;
                continue;
            }
            if (i+p.size() > s.size()) {
                return false;
            }
            if (star) {
                i = match(s, i, p);
                // cout << "p:" << p << ",i:" << i << "\n";
                if (i == string::npos) {
                    return false;
                }
                star = false;
            }
            else {
                // 严格匹配
                i = match(s, i, p);
                if (i == string::npos || i != p.size()) {
                    return false;
                }
            }
            
        }
        return true;
    }

    int match(const string& s, int i, const string& p1) {
        for (; i < s.size()-p1.size()+1; i++) {
            int ret = match2(s, i, p1);
            if (ret != string::npos) {
                return ret;
            }
        }
        return string::npos;
    }


    int match2(const string& s, int i, const string& p1) {
        // TODO
        auto ps = stringSplit(p1, '?');
        bool star = true;
        for (auto p: ps) {
            if (p == "?") {
                i += 1;
                continue;
            }
            // 长度 校验
            if (i+p.size() > s.size()) {
                return string::npos;
            }
            // 普通字母
            if (star) {
                // 前面有*
                i = s.find(p, i);
                if (i == string::npos) {
                    return string::npos;
                }
                i += p.size();
                //
                star = false;
            }
            else {
                // 硬匹配
                if (p != s.substr(i, p.size())) {
                    return string::npos;
                }
                i += p.size();
            }
        }
        if (i > s.size()) {
            return string::npos;
        }
        return i;
    }

    // ps 从右到左，匹配掉不为*的内容。确保ps后其实还有个*
    bool stripright(string& s, vector<string>& ps) {
        while (!ps.empty()) {
            string back = ps.back();
            if (back == "*") {
                break;
            }
            if (back.size() > s.size()) {
                return false;
            }
            // TODO
            int ret = match(s.substr(s.size()-back.size(), back.size()), 0, back);
            if (ret == string::npos) {
                return false;
            }
            // if (back == "?") {
            //     // s = s.substr(0, s.size()-1);
            // }
            // else if (back != s.substr(s.size()-back.size(), back.size())) {
            //     return false;
            // }
            s = s.substr(0, s.size()-back.size());
            ps.pop_back();
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
// "baabba"
// "?*?a??"
// "aa"
// "a*"
// "aaab"
// "b**"
// "bc"
// "??"
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
