
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
