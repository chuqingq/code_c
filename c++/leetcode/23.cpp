// 23. 合并K个升序链表
// https://leetcode-cn.com/problems/merge-k-sorted-lists/

/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode *next) : val(x), next(next) {}
 * };
 */

#include <queue>

class Solution {
public:
    ListNode* mergeKLists(vector<ListNode*>& lists) {
        // 过滤掉nullptr
        for (auto it = lists.begin(); it != lists.end(); ) {
            if (*it == nullptr) {
                lists.erase(it);
            } else {
                it++;
            }
        }
        // vector<ListNode*> lists2(lists.size());
        // auto it = copy_if(lists.begin(), lists.end(), lists2.begin(), [](const ListNode* n) {return n != nullptr;});
        // lists2.resize(distance(lists2.begin(), it));

        // 建堆
        auto cmp = [](ListNode* l, ListNode* r) { return l->val >= r->val; };
        priority_queue<ListNode*, vector<ListNode*>, decltype(cmp)> q(lists.begin(), lists.end(), cmp);
        
        // 从堆中获取
        ListNode* head = nullptr;
        ListNode** next = &head;
        while (!q.empty()) {
            auto t = q.top();
            q.pop();
            // if (t == nullptr) {
            //     continue;
            // }
            if (t->next != nullptr) {
                q.push(t->next);
            }
            // 
            *next = t;
            next = &t->next;
        }

        return head;
    }
};
