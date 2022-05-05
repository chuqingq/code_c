int fac(int x)  
{  
    int i,f=1;  //定义寄存器变量  
  
    for(i=1;i<=x;i++)  
        f*=i;  
  
    return f;  
}  

class Solution {
public:
    string getPermutation(int n, int k) {
        vector<int> v(n); // 从小到大排列
        for (auto i = 0; i < n; i++) {
            v[i] = i+1;
        }

        string res;
        solve(v, k, res);
        return res;
    }

    void solve(vector<int>& v, int k, string& res) {
        if (v.size() == 0) return;
        int f = fac(v.size()-1);
        int i = (k-1)/f;
        // cout << i << "\n";
        auto it = v.begin()+i;
        int e = *it;
        v.erase(it);
        // cout << "erase: " << e << "\n";
        res.push_back('0'+e);
        solve(v, k-i*f, res);
    }
};

