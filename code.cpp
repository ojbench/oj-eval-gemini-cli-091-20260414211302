#include <iostream>
#include <vector>
#include <cstdlib>

using namespace std;

struct __attribute__((packed)) Node {
    int ls, rs;
    long long val;
    unsigned int size_pri;
};

const int MAX_NODES = 2500000;
Node tr[MAX_NODES];
int node_cnt = 0;

inline int get_size(int p) {
    return tr[p].size_pri & 0xFFFFF;
}

inline unsigned int get_pri(int p) {
    return tr[p].size_pri >> 20;
}

inline void set_size_pri(int p, int size, unsigned int pri) {
    tr[p].size_pri = (size & 0xFFFFF) | (pri << 20);
}

int clone(int p) {
    int q = ++node_cnt;
    tr[q] = tr[p];
    return q;
}

int new_node(long long val) {
    int q = ++node_cnt;
    tr[q].ls = tr[q].rs = 0;
    tr[q].val = val;
    set_size_pri(q, 1, rand() & 0xFFF);
    return q;
}

void pushup(int p) {
    int sz = 1;
    if (tr[p].ls) sz += get_size(tr[p].ls);
    if (tr[p].rs) sz += get_size(tr[p].rs);
    set_size_pri(p, sz, get_pri(p));
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
    if (get_pri(x) > get_pri(y)) {
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
    return merge(merge(x, new_node(val)), y);
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
        if (tr[p].ls) sz += get_size(tr[p].ls);
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

int root[1000005];

int main() {
    srand(19260817);
    int op, lst = 0, it_a = -1, valid = 0;
    long long val = -1;
    while (scanf("%d", &op) != EOF) {
        long long a, b, c;
        switch (op) {
            case 0: {
                scanf("%lld%lld", &a, &b);
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
                if (valid && it_a == a && val == b) valid = 0;
                if (find(root[a], b)) {
                    root[a] = erase(root[a], b);
                }
                break;
            case 2:
                scanf("%lld", &a);
                root[++lst] = root[a];
                break;
            case 3: {
                scanf("%lld%lld", &a, &b);
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
