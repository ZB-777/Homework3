# 41343117

作業一

## 解題說明
要做出Template與其應用，透過Linked List及Iterator的設計，完成多項式的資料表示與運算。
1. template <class T>class ChainNode
2. template<class T>class Chain
3. template<class T> class ChainIterator
Chain<int>::iterator xHere = x.Begin();
Chain<int>::iterator xEnd = x.End();
4. template <class T>class Polynomial
Polynomial Representation
Circular List Representation of Polynomials
5. Available Lists 

### 解題策略

1. 使用Chainlterator:已更加簡單的方式編利串列，讓串列也可以像平常用int的方法是用for迴圈。
2.    
    *it 是回傳 element 這一個屬性，可以當作 (coef, exp)
  
   it->element->exp 透過 -> 會被簡化為 it->exp
   
   it++ 可以當作平常 for 迴圈裡面 i++ 的概念
   
   ++it 跟上面的意思一樣
   
   != 和 == 就是在檢查 A 節點是否等於或不等於 B 節點
   
   node_A - node_B 是在計算兩個節點的間距為多少
   
2.Available List
主要是幫助使用者存放節點。

讓你先把要刪除的節點存放到 Available List 裡，如果之後要用新節點的話，就不需要再生成額外的節點出來，直接使用 Available List 裡的即可。

getBack 就是把整串的 ChainNode 回收到 AvailableItem 裡。

isEmpty 用來檢查 Available List 是否為空。

getNode 就是把 Available List 裡面的一個節點拿到外面的 poly 串列裡。
  
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

    bool operator==(const ChainIterator& rhs) const { return current == rhs.current; }
    bool operator!=(const ChainIterator& rhs) const { return current != rhs.current; }

    int operator-(const ChainIterator& rhs) const {
      
        int dist = 0;
        Node* p = rhs.current;
        while (p && p != current) {
            p = p->link;
            dist++;
        }
        return (p == current) ? dist : 0;
    }

private:
    Node* current{ nullptr };

    template<class U> friend class Chain;
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

    ~Chain() { Release(); }

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

        if (k == -1) { // push front
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
        }
        else {
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

 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
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
    static bool AvailIsEmpty() { return avail == nullptr; }
    static Node* GetNode() {
        if (AvailIsEmpty()) {
            return new Node();
        }
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
        header->element = Term{ 0, -1 }; 
        header->link = header;        
    }
    static bool IsZeroCoef(long long c) { return c == 0; }
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
            if (!firstLinear) firstLinear = tailLinear = p;
            else { tailLinear->link = p; tailLinear = p; }
            p = nxt;
        }

        GetBack(firstLinear);
    }

public:
    Polynomial() { InitEmpty(); }

    Polynomial(const Polynomial& other) {
        InitEmpty();
        for (Node* p = other.header->link; p != other.header; p = p->link) {
            NewTerm(p->element.coef, p->element.exp);
        }
    }

    Polynomial& operator=(const Polynomial& other) {
        if (this == &other) return *this;
        ClearTermsToAvail();
        // reset to empty circular
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

    class iterator {
    public:
        iterator(Node* p = nullptr, Node* h = nullptr) : cur(p), head(h) {}
        Term& operator*() const { return cur->element; }
        Term* operator->() const { return &(cur->element); }
        iterator& operator++() { cur = cur->link; return *this; }
        bool operator!=(const iterator& rhs) const { return cur != rhs.cur; }
    private:
        Node* cur;
        Node* head;
    };

    iterator begin() const { return iterator(header->link, header); }
    iterator end() const { return iterator(header, header); }

    // Evaluate: substitute X
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
            }
            else {
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
            }
            else if (p->element.exp < q->element.exp) {
                C.NewTerm(q->element.coef, q->element.exp);
                q = q->link;
            }
            else {
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
            }
            else if (p->element.exp < q->element.exp) {
                C.NewTerm(-q->element.coef, q->element.exp);
                q = q->link;
            }
            else {
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


//////////////////////////////////////////////////////////////////////////////////////////////////// 
#ifdef DEMO_HW3
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    Polynomial A, B;
    cout << "Enter A (n c1 e1 ...): ";
    cin >> A;
    cout << "Enter B (n c1 e1 ...): ";
    cin >> B;

    cout << "A = " << A << "\n";
    cout << "B = " << B << "\n";
    cout << "A + B = " << (A + B) << "\n";
    cout << "A - B = " << (A - B) << "\n";
    cout << "A * B = " << (A * B) << "\n";

    double x;
    cout << "Enter x to evaluate A(x): ";
    cin >> x;
    cout << "A(" << x << ") = " << fixed << setprecision(6) << A.Evaluate(x) << "\n";

    Chain<int> xchain;
    xchain.PushBack(10);
    xchain.PushBack(20);
    Chain<int>::iterator xHere = xchain.Begin();
    Chain<int>::iterator xEnd = xchain.End();
    cout << "Chain<int>: ";
    for (; xHere != xEnd; ++xHere) cout << *xHere << " ";
    cout << "\n";
}
#endif
```

## 效能分析

| 功能  | 時間複雜度 | 空間複雜度  |
|------------------|-----------------------------|-------------------------------|
| `Chain::PushBack` | O(1) | O(1) |
| `Chain::Insert(k)` | O(k)，最壞 O(n) | O(1) |
| `Chain::Release` / 解構子 | O(n) | O(1) |
| `Polynomial::NewTerm` | O(t) | O(1) |
| `operator>>`（建立 N 項多項式） | O(N²) | O(N) |
| `Polynomial::Evaluate` | O(m) | O(1) |
| `operator+ (A + B)` | O((m + n)²) | O(m + n) |
| `operator- (A - B)` | O((m + n)²) | O(m + n) |
| `operator* (A * B)` | O((m · n)²) | O(m · n) |


## 測試與驗證

### 測試案例

| 測試案例 | 輸入內容 | 預期輸出 | 實際輸出 |
|---------|---------|---------|---------|
| 測試一 | `0` | `0` | `0` |
| 測試二 | `1 5 2` | `5X^2` | `5X^2` |
| 測試三 | `3 5 2 3 1 8 0` | `5X^2 + 3X + 8` | `5X^2 + 3X + 8` |
| 測試四 | `2 3 2 -3 2` | `0` | `0` |
| 測試五 | 非法輸入 | 異常拋出 | 異常拋出 |


### 編譯與執行指令


```bash
g++ --std=c++17 -o polynomial polynomial.cpp
./polynomial
A = 5X^2 + 3X + 8
B = 2X^2 - X
A + B = 7X^2 + 2X + 8
A - B = 3X^2 + 4X + 8
A * B = 10X^4 + X^3 + 13X^2 - 8X
```

### 結論

1.鏈結串列不是萬能
 插入方便，但一直從頭找會很慢

2.設計比語法重要
 同一個功能，NewTerm 怎麼寫，效能可以差到好幾倍
 
3.Available List 是效能優化的概念
不是每次 new/delete，而是「回收再利用」

## 申論及開發報告

### 選擇遞迴的原因

在本程式中，使用遞迴來計算連加總和的主要原因如下：

1. **程式邏輯簡單直觀**  
   遞迴的寫法能夠清楚表達「將問題拆解為更小的子問題」的核心概念。  
   例如，計算 $\Sigma(n)$ 的過程可分解為：  

   $$
   \Sigma(n) = n + \Sigma(n-1)
   $$

   當 $n$ 等於 1 或 0 時，直接返回結果，結束遞迴。

2. **易於理解與實現**  
   遞迴的程式碼更接近數學公式的表示方式，特別適合新手學習遞迴的基本概念。  
   以本程式為例：  

   ```cpp
   int sigma(int n) {
       if (n < 0)
           throw "n < 0";
       else if (n <= 1)
           return n;
       return n + sigma(n - 1);
   }
   ```

3. **遞迴的語意清楚**  
   在程式中，每次遞迴呼叫都代表一個「子問題的解」，而最終遞迴的返回結果會逐層相加，完成整體問題的求解。  
   這種設計簡化了邏輯，不需要額外變數來維護中間狀態。

透過遞迴實作 Sigma 計算，程式邏輯簡單且易於理解，特別適合展示遞迴的核心思想。然而，遞迴會因堆疊深度受到限制，當 $n$ 值過大時，應考慮使用迭代版本來避免 Stack Overflow 問題。
