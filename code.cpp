#include <iostream>
#include <vector>

using namespace std;

struct __attribute__((packed)) Node {
    int ls, rs;
    long long val;
    int size;
};

const int MAX_NODES = 2000000;
Node tr[MAX_NODES];
int node_cnt = 0;

unsigned int get_pri(long long val) {
    unsigned long long x = val + 0x9e3779b97f4a7c15ULL;
    x ^= x >> 30;
    x *= 0xbf58476d1ce4e5b9ULL;
    x ^= x >> 27;
    x *= 0x94d049bb133111ebULL;
    x ^= x >> 31;
    return x;
}

int clone(int p) {
    if (node_cnt >= MAX_NODES - 1) return p;
    int q = ++node_cnt;
    tr[q] = tr[p];
    return q;
}

int new_node(long long val) {
    if (node_cnt >= MAX_NODES - 1) return 0;
    int q = ++node_cnt;
    tr[q].ls = tr[q].rs = 0;
    tr[q].val = val;
    tr[q].size = 1;
    return q;
}

void pushup(int p) {
    if (!p) return;
    int sz = 1;
    if (tr[p].ls) sz += tr[tr[p].ls].size;
    if (tr[p].rs) sz += tr[tr[p].rs].size;
    tr[p].size = sz;
}

void split(int p, long long val, int &x, int &y) {
    if (!p) {
        x = y = 0;
        return;
    }
    int q = clone(p);
    if (tr[q].val <= val) {
        x = q;
        int new_rs;
        split(tr[q].rs, val, new_rs, y);
        tr[q].rs = new_rs;
    } else {
        y = q;
        int new_ls;
        split(tr[q].ls, val, x, new_ls);
        tr[q].ls = new_ls;
    }
    pushup(q);
}

int merge(int x, int y) {
    if (!x || !y) return x ? clone(x) : (y ? clone(y) : 0);
    if (get_pri(tr[x].val) > get_pri(tr[y].val)) {
        int q = clone(x);
        tr[q].rs = merge(tr[q].rs, y);
        pushup(q);
        return q;
    } else {
        int q = clone(y);
        tr[q].ls = merge(x, tr[q].ls);
        pushup(q);
        return q;
    }
}

int insert(int root, long long val) {
    int x, y;
    split(root, val, x, y);
    int n_node = new_node(val);
    if (!n_node) return root;
    return merge(merge(x, n_node), y);
}

int erase(int root, long long val) {
    int x, y, z;
    split(root, val, x, z);
    split(x, val - 1, x, y);
    return merge(x, z);
}

bool find(int root, long long val) {
    int p = root;
    while (p) {
        if (tr[p].val == val) return true;
        if (val < tr[p].val) p = tr[p].ls;
        else p = tr[p].rs;
    }
    return false;
}

int count_less_equal(int p, long long val) {
    if (!p) return 0;
    if (tr[p].val <= val) {
        int sz = 1;
        if (tr[p].ls) sz += tr[tr[p].ls].size;
        return sz + count_less_equal(tr[p].rs, val);
    } else {
        return count_less_equal(tr[p].ls, val);
    }
}

int query_range(int root, long long l, long long r) {
    return count_less_equal(root, r) - count_less_equal(root, l - 1);
}

bool find_prev(int p, long long val, long long &res) {
    bool found = false;
    while (p) {
        if (tr[p].val < val) {
            res = tr[p].val;
            found = true;
            p = tr[p].rs;
        } else {
            p = tr[p].ls;
        }
    }
    return found;
}

bool find_next(int p, long long val, long long &res) {
    bool found = false;
    while (p) {
        if (tr[p].val > val) {
            res = tr[p].val;
            found = true;
            p = tr[p].ls;
        } else {
            p = tr[p].rs;
        }
    }
    return found;
}

int root[2000005];

int main() {
    int op, lst = 0, it_a = -1, valid = 0;
    long long val = -1;
    while (scanf("%d", &op) != EOF) {
        long long a, b, c;
        switch (op) {
            case 0: {
                scanf("%lld%lld", &a, &b);
                if (a > 2000000) continue;
                if (!find(root[a], b)) {
                    root[a] = insert(root[a], b);
                    it_a = a;
                    val = b;
                    valid = 1;
                }
                break;
            }
            case 1:
                scanf("%lld%lld", &a, &b);
                if (a > 2000000) continue;
                if (valid && it_a == a && val == b) valid = 0;
                if (find(root[a], b)) {
                    root[a] = erase(root[a], b);
                }
                break;
            case 2:
                scanf("%lld", &a);
                if (a > 2000000) continue;
                ++lst;
                if (lst <= 2000000) root[lst] = root[a];
                break;
            case 3: {
                scanf("%lld%lld", &a, &b);
                if (a > 2000000) {
                    printf("false\n");
                    break;
                }
                if (find(root[a], b)) {
                    printf("true\n");
                    it_a = a;
                    val = b;
                    valid = 1;
                } else {
                    printf("false\n");
                }
                break;
            }
            case 4: {
                scanf("%lld%lld%lld", &a, &b, &c);
                if (a > 2000000) {
                    printf("0\n");
                    break;
                }
                printf("%d\n", query_range(root[a], b, c));
                break;
            }
            case 5:
                if (valid) {
                    long long res;
                    if (find_prev(root[it_a], val, res)) {
                        val = res;
                    } else {
                        valid = 0;
                    }
                }
                if (valid) {
                    printf("%lld\n", val);
                } else {
                    printf("-1\n");
                }
                break;
            case 6:
                if (valid) {
                    long long res;
                    if (find_next(root[it_a], val, res)) {
                        val = res;
                    } else {
                        valid = 0;
                    }
                }
                if (valid) {
                    printf("%lld\n", val);
                } else {
                    printf("-1\n");
                }
                break;
        }
    }
    return 0;
}
