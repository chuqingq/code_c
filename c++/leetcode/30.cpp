// 30. 串联所有单词的子串
// https://leetcode.cn/problems/substring-with-concatenation-of-all-words/

class Solution
{
public:
    vector<int> result_;

    string s_;
    vector<string> words_;
    int word_len_;

    vector<int> status_; // 保存s_中对words_的匹配信息：-2未匹配，-1不匹配，其他值是匹配的words_下标

    vector<int> findSubstring(string s, vector<string> &words)
    {
        result_.clear();
        if (words.size() == 0)
        {
            return result_;
        }
        s_ = s;
        // words_ = words;
        word_len_ = words.back().size();
        status_.resize(s.size(), -2);

        // words_是去重后的vector<string>
        set<string> wordsetstr(words.begin(), words.end());
        words_.assign(wordsetstr.begin(), wordsetstr.end());

        multiset<int> wordsetint;
        for (int i = 0; i < words.size(); i++)
        {
            wordsetint.insert(std::find(words_.begin(), words_.end(), words[i]) - words_.begin());
        }

        for (int i = 0; i < words_.size(); i++)
        {
            size_t pos = 0;
            while ((pos = s_.find(words_[i], pos)) != string::npos)
            {
                if (pos > s_.size() - words.size() * word_len_)
                {
                    break;
                }
                status_[pos] = i;

                multiset<int> wordset2(wordsetint);
                wordset2.erase(wordset2.find(i));
                find(pos, pos + word_len_, wordset2);

                pos += 1;
            }
        }

        return result_;
        // return status_;
    }

    // [i, j)已经匹配，这里从j开始向右搜索剩余的words
    void find(int i, int j, multiset<int> &wordset)
    {
        if (wordset.size() == 0)
        {
            result_.push_back(i);
            return;
        }
        if (j >= s_.size())
        {
            return;
        }
        if (search(j, wordset))
        {
            find(i, j + word_len_, wordset);
        }
    }

    // 如果匹配words_，返回words_中的index；如果没有匹配，则返回-1
    bool search(int i, multiset<int> &wordset)
    {
        int status = status_[i];
        // 不匹配
        if (status == -1)
        {
            return false;
        }
        // 已匹配
        if (status > -1)
        {
            auto it = wordset.find(status);
            if (it == wordset.end())
            {
                return false;
            }
            else
            {
                wordset.erase(it);
                return true;
            }
        }
        // 未匹配过
        string ss = s_.substr(i, word_len_);
        auto it = std::find(words_.begin(), words_.end(), ss);
        if (it == words_.end())
        {
            status_[i] = -1;
            return false;
        }
        // 如果匹配
        else
        {
            status_[i] = it - words_.begin();
            auto it = wordset.find(status_[i]);
            if (it == wordset.end())
            {
                return false;
            }
            else
            {
                wordset.erase(it);
                return true;
            }
        }
    }
};
