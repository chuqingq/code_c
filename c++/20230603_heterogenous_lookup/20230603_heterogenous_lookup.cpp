#include <bits/stdc++.h>

using namespace std;

bool enable_new_log = false;

void* operator new(size_t s) {
    if (enable_new_log) {
        cout << "new " << s << "\n";
    }
    return malloc(s);
}

int main() {
    // const char *str = "a long string cannot fall into SSO buffer";

    // map<string, int, less<>> m;
    map<string, int> m;
    m["a long string cannot fall into SSO buffer"] = 1;
    cout << "after map init\n";

    enable_new_log = true;

    if (m.find("a long string cannot fall into SSO buffer") != m.end()) {
        cout << "found\n";
    } else {
        cout << "not found\n";
    }

    return 0;
}
