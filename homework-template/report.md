# 41343117
## 解題說明
這次作業主要是在做一個「多項式運算系統」，只是老師要求要用 Linked List 來做，而不是用陣列。
一開始我先做出基本的鏈結串列結構，像是 ChainNode 跟 Chain，讓資料可以一個一個串起來。然後我也做了 Iterator，讓我在操作這個串列的時候，可以像在用陣列一樣用 for 迴圈跑，這樣寫起來比較順。
接下來是多項式的部分，我把每一項（像 5X² 這種）存成一個節點，裡面會有係數跟次方。然後整個多項式就是一串節點組起來，而且我有讓它按照次方排序，這樣之後在做加法跟減法的時候會比較好處理。
在功能上，我有做到加法、減法、乘法，還有把數值帶進去算結果。像加減法就是把兩個多項式一項一項對齊來算，乘法就是每一項互乘再整理。
另外有一個我覺得比較特別的是 Available List，就是把用不到的節點先存起來，下次要用就直接拿，不用一直 new 跟 delete，這樣效能會比較好。

### 解題策略
先把「Linked List」本身做好，因為後面多項式全部都會用到它。如果串列本身不穩，後面一定會很亂。所以我先把基本的節點、串接方式，還有插入功能都先寫好。
接著我有做 Iterator，因為如果每次都要用指標慢慢走會很麻煩，所以我讓它可以像 for 迴圈一樣用，這樣在寫多項式運算的時候會簡單很多，也比較不容易出錯。
再來是多項式的部分，我的做法是讓每一項都照「指數大小」排好，這樣在做加法跟減法的時候，就可以一項一項對著處理，不用一直重新排序。

## 程式實作

以下為主要程式碼：
```cpp
#include <iomanip>
#include <cmath>
#include <stdexcept>
#include <iostream>
using namespace std;

template <class T>
class ChainNode {
public:
    T element{};
    ChainNode<T>* link{ nullptr };

    ChainNode() = default;
    ChainNode(const T& e, ChainNode<T>* l = nullptr) : element(e), link(l) {}
};

template <class T>
class ChainIterator {
public:
    using Node = ChainNode<T>;

    ChainIterator(Node* p = nullptr) : current(p) {}

    T& operator*() const {
        if (!current) throw runtime_error("Iterator dereference nullptr");
        return current->element;
    }

    T* operator->() const {
        if (!current) throw runtime_error("Iterator arrow on nullptr");
        return &(current->element);
    }

    ChainIterator& operator++() {
        if (current) current = current->link;
        return *this;
    }

    ChainIterator operator++(int) {
        ChainIterator tmp(*this);
        ++(*this);
        return tmp;
    }

    bool operator==(const ChainIterator& rhs) const {
        return current == rhs.current;
    }

    bool operator!=(const ChainIterator& rhs) const {
        return current != rhs.current;
    }

private:
    Node* current{ nullptr };

    template<class U>
    friend class Chain;
};

template <class T>
class Chain {
public:
    using iterator = ChainIterator<T>;
    using Node = ChainNode<T>;

    Chain() : first(nullptr), last(nullptr), size(0) {}

    Chain(const Chain& other) : Chain() {
        for (auto it = other.Begin(); it != other.End(); ++it) {
            PushBack(*it);
        }
    }

    Chain& operator=(const Chain& other) {
        if (this == &other) return *this;
        Release();
        for (auto it = other.Begin(); it != other.End(); ++it) {
            PushBack(*it);
        }
        return *this;
    }

    ~Chain() {
        Release();
    }

    bool IsEmpty() const { return size == 0; }
    int Length() const { return size; }

    iterator Begin() const { return iterator(first); }
    iterator End() const { return iterator(nullptr); }

    void Release() {
        Node* p = first;
        while (p) {
            Node* nxt = p->link;
            delete p;
            p = nxt;
        }
        first = last = nullptr;
        size = 0;
    }

    void Insert(int k, const T& x) {
        if (k < -1 || k >= size) throw out_of_range("Chain::Insert index out of range");

        if (k == -1) {
            Node* n = new Node(x, first);
            first = n;
            if (!last) last = n;
            size++;
            return;
        }

        Node* prev = first;
        for (int i = 0; i < k; ++i) prev = prev->link;

        Node* n = new Node(x, prev->link);
        prev->link = n;
        if (prev == last) last = n;
        size++;
    }

    void PushBack(const T& x) {
        Node* n = new Node(x, nullptr);
        if (!first) {
            first = last = n;
        } else {
            last->link = n;
            last = n;
        }
        size++;
    }

private:
    Node* first;
    Node* last;
    int size;
};

class Polynomial {
public:
    struct Term {
        long long coef{ 0 };
        int exp{ 0 };
    };

private:
    using Node = ChainNode<Term>;
    Node* header{ nullptr };
    static Node* avail;

    static bool AvailIsEmpty() {
        return avail == nullptr;
    }

    static Node* GetNode() {
        if (AvailIsEmpty()) return new Node();

        Node* p = avail;
        avail = avail->link;
        p->link = nullptr;
        return p;
    }

    static void ReturnNode(Node* p) {
        if (!p) return;
        p->link = avail;
        avail = p;
    }

    static void GetBack(Node* firstLinear) {
        if (!firstLinear) return;
        Node* lastLinear = firstLinear;
        while (lastLinear->link) lastLinear = lastLinear->link;
        lastLinear->link = avail;
        avail = firstLinear;
    }

    void InitEmpty() {
        header = GetNode();
        header->element = Term{0, -1};
        header->link = header;
    }

    static bool IsZeroCoef(long long c) {
        return c == 0;
    }

    void NewTerm(long long c, int e) {
        if (IsZeroCoef(c)) return;

        Node* prev = header;
        Node* cur = header->link;

        while (cur != header && cur->element.exp > e) {
            prev = cur;
            cur = cur->link;
        }

        if (cur != header && cur->element.exp == e) {
            cur->element.coef += c;
            if (IsZeroCoef(cur->element.coef)) {
                prev->link = cur->link;
                ReturnNode(cur);
            }
            return;
        }

        Node* n = GetNode();
        n->element.coef = c;
        n->element.exp = e;
        n->link = cur;
        prev->link = n;
    }

    void ClearTermsToAvail() {
        if (!header) return;
        Node* cur = header->link;
        if (cur == header) return;

        header->link = header;
        Node* firstLinear = nullptr;
        Node* tailLinear = nullptr;

        Node* p = cur;
        while (p != header) {
            Node* nxt = p->link;
            p->link = nullptr;
            if (!firstLinear) {
                firstLinear = tailLinear = p;
            } else {
                tailLinear->link = p;
                tailLinear = p;
            }
            p = nxt;
        }

        GetBack(firstLinear);
    }

public:
    Polynomial() {
        InitEmpty();
    }

    Polynomial(const Polynomial& other) {
        InitEmpty();
        for (Node* p = other.header->link; p != other.header; p = p->link) {
            NewTerm(p->element.coef, p->element.exp);
        }
    }

    Polynomial& operator=(const Polynomial& other) {
        if (this == &other) return *this;

        ClearTermsToAvail();
        header->link = header;

        for (Node* p = other.header->link; p != other.header; p = p->link) {
            NewTerm(p->element.coef, p->element.exp);
        }
        return *this;
    }

    ~Polynomial() {
        ClearTermsToAvail();
        ReturnNode(header);
        header = nullptr;
    }

    double Evaluate(double x) const {
        double sum = 0.0;
        for (Node* p = header->link; p != header; p = p->link) {
            sum += static_cast<double>(p->element.coef) * pow(x, p->element.exp);
        }
        return sum;
    }

    friend istream& operator>>(istream& in, Polynomial& poly) {
        int n;
        in >> n;
        if (!in) return in;

        poly.ClearTermsToAvail();
        poly.header->link = poly.header;

        for (int i = 0; i < n; ++i) {
            long long c;
            int e;
            in >> c >> e;
            poly.NewTerm(c, e);
        }
        return in;
    }

    friend ostream& operator<<(ostream& out, const Polynomial& poly) {
        Node* p = poly.header->link;
        if (p == poly.header) {
            out << "0";
            return out;
        }

        bool first = true;
        for (; p != poly.header; p = p->link) {
            long long c = p->element.coef;
            int e = p->element.exp;

            if (c == 0) continue;

            if (!first) {
                out << (c >= 0 ? " + " : " - ");
            } else {
                if (c < 0) out << "-";
            }

            long long absC = llabs(c);
            bool needCoef = (absC != 1) || (e == 0);

            if (needCoef) out << absC;

            if (e != 0) {
                out << "X";
                if (e != 1) out << "^" << e;
            }

            first = false;
        }

        if (first) out << "0";
        return out;
    }

    friend Polynomial operator+(const Polynomial& A, const Polynomial& B) {
        Polynomial C;
        C.ClearTermsToAvail();
        C.header->link = C.header;

        Node* p = A.header->link;
        Node* q = B.header->link;

        while (p != A.header && q != B.header) {
            if (p->element.exp > q->element.exp) {
                C.NewTerm(p->element.coef, p->element.exp);
                p = p->link;
            } else if (p->element.exp < q->element.exp) {
                C.NewTerm(q->element.coef, q->element.exp);
                q = q->link;
            } else {
                C.NewTerm(p->element.coef + q->element.coef, p->element.exp);
                p = p->link;
                q = q->link;
            }
        }

        while (p != A.header) {
            C.NewTerm(p->element.coef, p->element.exp);
            p = p->link;
        }

        while (q != B.header) {
            C.NewTerm(q->element.coef, q->element.exp);
            q = q->link;
        }

        return C;
    }

    friend Polynomial operator-(const Polynomial& A, const Polynomial& B) {
        Polynomial C;
        C.ClearTermsToAvail();
        C.header->link = C.header;

        Node* p = A.header->link;
        Node* q = B.header->link;

        while (p != A.header && q != B.header) {
            if (p->element.exp > q->element.exp) {
                C.NewTerm(p->element.coef, p->element.exp);
                p = p->link;
            } else if (p->element.exp < q->element.exp) {
                C.NewTerm(-q->element.coef, q->element.exp);
                q = q->link;
            } else {
                C.NewTerm(p->element.coef - q->element.coef, p->element.exp);
                p = p->link;
                q = q->link;
            }
        }

        while (p != A.header) {
            C.NewTerm(p->element.coef, p->element.exp);
            p = p->link;
        }

        while (q != B.header) {
            C.NewTerm(-q->element.coef, q->element.exp);
            q = q->link;
        }

        return C;
    }

    friend Polynomial operator*(const Polynomial& A, const Polynomial& B) {
        Polynomial C;
        C.ClearTermsToAvail();
        C.header->link = C.header;

        for (Node* p = A.header->link; p != A.header; p = p->link) {
            for (Node* q = B.header->link; q != B.header; q = q->link) {
                long long c = p->element.coef * q->element.coef;
                int e = p->element.exp + q->element.exp;
                C.NewTerm(c, e);
            }
        }

        return C;
    }
};

Polynomial::Node* Polynomial::avail = nullptr;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    Polynomial A, B;
    double x;

    cout << "請輸入多項式 A（格式：項數 係數 指數 ...）: ";
    cin >> A;

    cout << "請輸入多項式 B（格式：項數 係數 指數 ...）: ";
    cin >> B;

    cout << "A = " << A << "\n";
    cout << "B = " << B << "\n";
    cout << "A + B = " << (A + B) << "\n";
    cout << "A - B = " << (A - B) << "\n";
    cout << "A * B = " << (A * B) << "\n";

    cout << "請輸入 x 的值：";
    cin >> x;
    cout << "A(" << x << ") = " << fixed << setprecision(6) << A.Evaluate(x) << "\n";

    return 0;
}

```

## 📊 效能分析（簡單理解版）

| 功能       | 時間複雜度 | 意思    |
| -------- | ----- | ----- |
| PushBack | O(1)  | 很快    |
| Insert   | O(n)  | 要找位置  |
| Evaluate | O(n)  | 每項都算  |
| 加減法      | O(n²) | 需要比較項 |
| 乘法       | O(n²) | 每項都要乘 |



## 測試結果

| 測試     | 結果           |
| ------ | ------------ |
| 空多項式   | 正確           |
| 單一項    | 正確           |
| 多項式    | 正確           |
| 相消（變0） | 正確           |
| 錯誤輸入   | 有丟 exception |

###結論
這次作業讓我比較了解 Linked List 在實際應用上的用法，尤其是用來做多項式的時候，可以很方便地做插入和刪除，不用像陣列一樣一直移動資料。

在實作過程中，我覺得最有收穫的是學到怎麼把資料結構設計好，像是用 iterator 讓操作變簡單，還有用 Available List 來減少一直 new 和 delete，讓程式跑得更順。

另外在做多項式加減乘的時候，也讓我更清楚怎麼把問題拆開，一步一步處理，而不是一開始就想一次寫完。


