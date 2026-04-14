#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

struct __attribute__((packed)) Operation {
    unsigned int type_a;
    long long b;
};

vector<Operation> ops;
vector<long long> c_vals;
vector<long long> vals;

struct Node {
    int ls, rs;
    int sum;
};

const int MAX_NODES = 4000000;
Node tr[MAX_NODES];
int node_cnt = 0;

int clone(int p) {
    if (node_cnt >= MAX_NODES - 1) return p;
    int q = ++node_cnt;
    tr[q] = tr[p];
    return q;
}

void pushup(int p) {
    tr[p].sum = tr[tr[p].ls].sum + tr[tr[p].rs].sum;
}

int insert(int p, int l, int r, int x) {
    int q = clone(p);
    if (l == r) {
        tr[q].sum = 1;
        return q;
    }
    int mid = l + (r - l) / 2;
    if (x <= mid) tr[q].ls = insert(tr[p].ls, l, mid, x);
    else tr[q].rs = insert(tr[p].rs, mid + 1, r, x);
    pushup(q);
    return q;
}

int erase(int p, int l, int r, int x) {
    int q = clone(p);
    if (l == r) {
        tr[q].sum = 0;
        return q;
    }
    int mid = l + (r - l) / 2;
    if (x <= mid) tr[q].ls = erase(tr[p].ls, l, mid, x);
    else tr[q].rs = erase(tr[p].rs, mid + 1, r, x);
    pushup(q);
    return q;
}

bool find(int p, int l, int r, int x) {
    if (!p) return false;
    if (l == r) return tr[p].sum > 0;
    int mid = l + (r - l) / 2;
    if (x <= mid) return find(tr[p].ls, l, mid, x);
    else return find(tr[p].rs, mid + 1, r, x);
}

int query_range(int p, int l, int r, int ql, int qr) {
    if (!p) return 0;
    if (ql <= l && r <= qr) return tr[p].sum;
    int mid = l + (r - l) / 2;
    int res = 0;
    if (ql <= mid) res += query_range(tr[p].ls, l, mid, ql, qr);
    if (qr > mid) res += query_range(tr[p].rs, mid + 1, r, ql, qr);
    return res;
}

int find_prev(int p, int l, int r, int x) {
    if (!p || tr[p].sum == 0) return -1;
    if (l == r) return l < x ? l : -1;
    int mid = l + (r - l) / 2;
    if (x <= mid + 1) {
        return find_prev(tr[p].ls, l, mid, x);
    } else {
        int res = find_prev(tr[p].rs, mid + 1, r, x);
        if (res != -1) return res;
        return find_prev(tr[p].ls, l, mid, x);
    }
}

int find_next(int p, int l, int r, int x) {
    if (!p || tr[p].sum == 0) return -1;
    if (l == r) return l > x ? l : -1;
    int mid = l + (r - l) / 2;
    if (x >= mid) {
        return find_next(tr[p].rs, mid + 1, r, x);
    } else {
        int res = find_next(tr[p].ls, l, mid, x);
        if (res != -1) return res;
        return find_next(tr[p].rs, mid + 1, r, x);
    }
}

vector<int> root;

int get_id(long long v) {
    auto it = lower_bound(vals.begin(), vals.end(), v);
    if (it != vals.end() && *it == v) return it - vals.begin();
    return -1;
}

int get_id_lower_bound(long long v) {
    auto it = lower_bound(vals.begin(), vals.end(), v);
    if (it == vals.end()) return -1;
    return it - vals.begin();
}

int get_id_upper_bound(long long v) {
    auto it = upper_bound(vals.begin(), vals.end(), v);
    if (it == vals.begin()) return -1;
    return it - vals.begin();
}

int main() {
    ops.reserve(200000);
    vals.reserve(400000);
    c_vals.reserve(50000);
    root.reserve(200000);
    root.push_back(0); // root[0] = 0

    int op;
    while (scanf("%d", &op) != EOF) {
        int a;
        long long b = 0, c = 0;
        if (op == 0 || op == 1 || op == 3) {
            scanf("%d%lld", &a, &b);
            vals.push_back(b);
        } else if (op == 2) {
            scanf("%d", &a);
        } else if (op == 4) {
            scanf("%d%lld%lld", &a, &b, &c);
            vals.push_back(b);
            vals.push_back(c);
            c_vals.push_back(c);
        }
        Operation o;
        o.type_a = (op << 24) | (a & 0xFFFFFF);
        o.b = b;
        ops.push_back(o);
    }

    sort(vals.begin(), vals.end());
    vals.erase(unique(vals.begin(), vals.end()), vals.end());
    vals.shrink_to_fit();
    ops.shrink_to_fit();
    c_vals.shrink_to_fit();

    int M = vals.size();
    if (M == 0) M = 1;

    int lst = 0, it_a = -1, valid = 0;
    long long val = -1;
    int c_idx = 0;

    for (size_t i = 0; i < ops.size(); ++i) {
        int type = ops[i].type_a >> 24;
        int a = ops[i].type_a & 0xFFFFFF;
        long long b = ops[i].b;

        if (a >= root.size()) {
            root.resize(a + 1, 0);
        }

        switch (type) {
            case 0: {
                int x = get_id(b);
                if (x != -1 && !find(root[a], 0, M - 1, x)) {
                    root[a] = insert(root[a], 0, M - 1, x);
                    it_a = a;
                    val = b;
                    valid = 1;
                }
                break;
            }
            case 1: {
                if (valid && it_a == a && val == b) valid = 0;
                int x = get_id(b);
                if (x != -1 && find(root[a], 0, M - 1, x)) {
                    root[a] = erase(root[a], 0, M - 1, x);
                }
                break;
            }
            case 2: {
                ++lst;
                if (lst >= root.size()) root.resize(lst + 1, 0);
                root[lst] = root[a];
                break;
            }
            case 3: {
                int x = get_id(b);
                if (x != -1 && find(root[a], 0, M - 1, x)) {
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
                long long c = c_vals[c_idx++];
                int l_id = get_id_lower_bound(b);
                int r_id = get_id_upper_bound(c);
                if (r_id == -1) r_id = M - 1;
                else r_id--;
                
                if (l_id != -1 && l_id <= r_id) {
                    printf("%d\n", query_range(root[a], 0, M - 1, l_id, r_id));
                } else {
                    printf("0\n");
                }
                break;
            }
            case 5: {
                if (valid) {
                    int x = get_id(val);
                    int prev_x = find_prev(root[it_a], 0, M - 1, x);
                    if (prev_x == -1) {
                        valid = 0;
                    } else {
                        val = vals[prev_x];
                    }
                }
                if (valid) {
                    printf("%lld\n", val);
                } else {
                    printf("-1\n");
                }
                break;
            }
            case 6: {
                if (valid) {
                    int x = get_id(val);
                    int next_x = find_next(root[it_a], 0, M - 1, x);
                    if (next_x == -1) {
                        valid = 0;
                    } else {
                        val = vals[next_x];
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
    }
    return 0;
}
