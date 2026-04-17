#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <cmath>
#include <algorithm>

using namespace std;
using namespace std::chrono;

//binary heap
class BinaryHeap {
private:
    vector<double> heap;

    //move node up
    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;

            if (heap[parent] <= heap[index]) {
                break;
            }

            swap(heap[parent], heap[index]);
            index = parent;
        }
    }

    //move node down
    void heapifyDown(int index) {
        int n = (int)heap.size();

        while (true) {
            int smallest = index;
            int left = 2 * index + 1;
            int right = 2 * index + 2;

            if (left < n && heap[left] < heap[smallest]) {
                smallest = left;
            }

            if (right < n && heap[right] < heap[smallest]) {
                smallest = right;
            }

            if (smallest == index) {
                break;
            }

            swap(heap[index], heap[smallest]);
            index = smallest;
        }
    }

public:
    //insert key
    void insert(double key) {
        heap.push_back(key);
        heapifyUp((int)heap.size() - 1);
    }

    //find min
    double findMin() {
        if (heap.empty()) {
            return -1;
        }

        return heap[0];
    }

    //delete min
    double deleteMin() {
        double minValue;

        if (heap.empty()) {
            return -1;
        }

        minValue = heap[0];
        heap[0] = heap[heap.size() - 1];
        heap.pop_back();

        if (!heap.empty()) {
            heapifyDown(0);
        }

        return minValue;
    }

    //decrease key
    void decreaseKey(int index, double newKey) {
        if (index < 0) {
            return;
        }

        if (index >= (int)heap.size()) {
            return;
        }

        if (newKey >= heap[index]) {
            return;
        }

        heap[index] = newKey;
        heapifyUp(index);
    }

    int size() {
        return (int)heap.size();
    }

    bool empty() {
        return heap.empty();
    }
};

//pairing heap
struct PairingNode {
    double key;
    PairingNode* parent;
    PairingNode* firstChild;
    PairingNode* nextSibling;
    PairingNode* prevSibling;

    //create pairing node
    PairingNode(double k) {
        key = k;
        parent = NULL;
        firstChild = NULL;
        nextSibling = NULL;
        prevSibling = NULL;
    }
};

class PairingHeap {
private:
    PairingNode* root;
    int nodeCount;

    //link two trees
    PairingNode* link(PairingNode* a, PairingNode* b) {
        if (a == NULL) {
            return b;
        }

        if (b == NULL) {
            return a;
        }

        if (a->key > b->key) {
            PairingNode* temp = a;
            a = b;
            b = temp;
        }

        b->parent = a;
        b->nextSibling = a->firstChild;

        if (a->firstChild != NULL) {
            a->firstChild->prevSibling = b;
        }

        a->firstChild = b;
        b->prevSibling = NULL;

        return a;
    }

    //combine sibling trees
    PairingNode* combineSiblings(PairingNode* first) {
        vector<PairingNode*> pairs;
        PairingNode* current;

        if (first == NULL) {
            return NULL;
        }

        current = first;

        while (current != NULL) {
            PairingNode* firstNode = current;
            PairingNode* secondNode = current->nextSibling;

            if (secondNode != NULL) {
                current = secondNode->nextSibling;
            } else {
                current = NULL;
            }

            firstNode->nextSibling = NULL;
            firstNode->prevSibling = NULL;
            firstNode->parent = NULL;

            if (secondNode != NULL) {
                secondNode->nextSibling = NULL;
                secondNode->prevSibling = NULL;
                secondNode->parent = NULL;
                pairs.push_back(link(firstNode, secondNode));
            } else {
                pairs.push_back(firstNode);
            }
        }

        if (pairs.empty()) {
            return NULL;
        }

        PairingNode* result = pairs[pairs.size() - 1];
        int i;

        for (i = (int)pairs.size() - 2; i >= 0; i--) {
            result = link(result, pairs[i]);
        }

        return result;
    }

    //clear subtree
    void clearSubtree(PairingNode* node) {
        PairingNode* child;

        if (node == NULL) {
            return;
        }

        child = node->firstChild;

        while (child != NULL) {
            PairingNode* next = child->nextSibling;
            clearSubtree(child);
            child = next;
        }

        delete node;
    }

public:
    //create pairing heap
    PairingHeap() {
        root = NULL;
        nodeCount = 0;
    }

    //destroy pairing heap
    ~PairingHeap() {
        clearSubtree(root);
    }

    //insert key
    void insert(double key) {
        PairingNode* newNode = new PairingNode(key);
        root = link(root, newNode);
        nodeCount++;
    }

    //find min
    double findMin() {
        if (root == NULL) {
            return -1;
        }

        return root->key;
    }

    //delete min
    double deleteMin() {
        double minValue;
        PairingNode* oldRoot;
        PairingNode* children;

        if (root == NULL) {
            return -1;
        }

        minValue = root->key;
        oldRoot = root;
        children = root->firstChild;
        oldRoot->firstChild = NULL;

        root = combineSiblings(children);

        if (root != NULL) {
            root->parent = NULL;
            root->prevSibling = NULL;
        }

        delete oldRoot;
        nodeCount--;

        return minValue;
    }

    //decrease key
    void decreaseKey(PairingNode* node, double newKey) {
        if (node == NULL) {
            return;
        }

        if (newKey >= node->key) {
            return;
        }

        node->key = newKey;

        if (node == root) {
            return;
        }

        if (node->prevSibling != NULL) {
            node->prevSibling->nextSibling = node->nextSibling;
        }

        if (node->nextSibling != NULL) {
            node->nextSibling->prevSibling = node->prevSibling;
        }

        if (node->parent != NULL && node->parent->firstChild == node) {
            node->parent->firstChild = node->nextSibling;
        }

        node->nextSibling = NULL;
        node->prevSibling = NULL;
        node->parent = NULL;

        root = link(root, node);
    }

    int size() {
        return nodeCount;
    }

    bool empty() {
        return root == NULL;
    }
};

//fibonacci heap
struct FibNode {
    double key;
    FibNode* parent;
    FibNode* child;
    FibNode* left;
    FibNode* right;
    int degree;
    bool marked;

    //create fib node
    FibNode(double k) {
        key = k;
        parent = NULL;
        child = NULL;
        left = this;
        right = this;
        degree = 0;
        marked = false;
    }
};

class FibonacciHeap {
private:
    FibNode* minNode;
    int nodeCount;

    //add to root list
    void addToRootList(FibNode* node) {
        if (minNode == NULL) {
            minNode = node;
            minNode->left = minNode;
            minNode->right = minNode;
            return;
        }

        node->right = minNode->right;
        node->left = minNode;
        minNode->right->left = node;
        minNode->right = node;
    }

    //link parent and child
    void link(FibNode* parent, FibNode* child) {
        child->left->right = child->right;
        child->right->left = child->left;

        if (parent->child == NULL) {
            parent->child = child;
            child->left = child;
            child->right = child;
        } else {
            child->left = parent->child;
            child->right = parent->child->right;
            parent->child->right->left = child;
            parent->child->right = child;
        }

        child->parent = parent;
        parent->degree++;
        child->marked = false;
    }

    //cut node
    void cut(FibNode* node, FibNode* parent) {
        if (node->right == node) {
            parent->child = NULL;
        } else {
            node->left->right = node->right;
            node->right->left = node->left;

            if (parent->child == node) {
                parent->child = node->right;
            }
        }

        parent->degree--;
        addToRootList(node);
        node->parent = NULL;
        node->marked = false;
    }

    //cascade cut, when tree is unbalanced
    void cascadingCut(FibNode* node) {
        FibNode* parent = node->parent;

        if (parent == NULL) {
            return;
        }

        if (!node->marked) {
            node->marked = true;
        } else {
            cut(node, parent);
            cascadingCut(parent);
        }
    }

    //consolidate trees
    void consolidate() {
        int maxDegree;
        vector<FibNode*> degreeTable;
        vector<FibNode*> roots;
        FibNode* current;
        int i;

        if (minNode == NULL) {
            return;
        }

        maxDegree = (int)log2((double)nodeCount) + 1;
        degreeTable.resize(maxDegree, NULL);

        current = minNode;
        do {
            roots.push_back(current);
            current = current->right;
        } while (current != minNode);

        for (i = 0; i < (int)roots.size(); i++) {
            FibNode* x = roots[i];
            int d = x->degree;

            while (degreeTable[d] != NULL) {
                FibNode* y = degreeTable[d];

                if (x->key > y->key) {
                    FibNode* temp = x;
                    x = y;
                    y = temp;
                }

                link(x, y);
                degreeTable[d] = NULL;
                d++;
            }

            degreeTable[d] = x;
        }

        minNode = NULL;

        for (i = 0; i < (int)degreeTable.size(); i++) {
            FibNode* node = degreeTable[i];

            if (node != NULL) {
                if (minNode == NULL) {
                    minNode = node;
                    minNode->left = minNode;
                    minNode->right = minNode;
                } else {
                    addToRootList(node);

                    if (node->key < minNode->key) {
                        minNode = node;
                    }
                }
            }
        }
    }

    //clear subtree
    void clearSubtree(FibNode* node) {
        FibNode* current;

        if (node == NULL) {
            return;
        }

        current = node;

        do {
            FibNode* next = current->right;

            if (current->child != NULL) {
                clearSubtree(current->child);
            }

            delete current;
            current = next;
        } while (current != node);
    }

public:
    //create fib heap
    FibonacciHeap() {
        minNode = NULL;
        nodeCount = 0;
    }

    //destroy fib heap
    ~FibonacciHeap() {
        if (minNode != NULL) {
            clearSubtree(minNode);
        }
    }

    //insert key
    void insert(double key) {
        FibNode* newNode = new FibNode(key);

        addToRootList(newNode);

        if (minNode != NULL && key < minNode->key) {
            minNode = newNode;
        }

        nodeCount++;
    }

    //find min
    double findMin() {
        if (minNode == NULL) {
            return -1;
        }

        return minNode->key;
    }

    //delete min
    double deleteMin() {
        double minValue;
        FibNode* z;

        if (minNode == NULL) {
            return -1;
        }

        minValue = minNode->key;
        z = minNode;

        if (z->child != NULL) {
            FibNode* current = z->child;

            do {
                FibNode* next = current->right;
                addToRootList(current);
                current->parent = NULL;
                current = next;
            } while (current != z->child);
        }

        if (z->right == z) {
            minNode = NULL;
        } else {
            z->left->right = z->right;
            z->right->left = z->left;
            minNode = z->right;
            consolidate();
        }

        delete z;
        nodeCount--;

        return minValue;
    }

    //decrease key
    void decreaseKey(FibNode* node, double newKey) {
        FibNode* parent;

        if (node == NULL) {
            return;
        }

        if (newKey >= node->key) {
            return;
        }

        node->key = newKey;
        parent = node->parent;

        if (parent != NULL && node->key < parent->key) {
            cut(node, parent);
            cascadingCut(parent);
        }

        if (node->key < minNode->key) {
            minNode = node;
        }
    }

    int size() {
        return nodeCount;
    }

    bool empty() {
        return minNode == NULL;
    }
};

//benchmarks
struct BenchmarkResult {
    int n;
    long long binaryHeapTime;
    long long pairingHeapTime;
    long long fibonacciHeapTime;
    long long binaryHeapOps;
    long long pairingHeapOps;
    long long fibonacciHeapOps;
};

//generate random key
double generateRandomKey() {
    double value = (double)rand();
    value = value / (double)RAND_MAX;
    value = value * 1000000.0;
    return value;
}

//run benchmarks
vector<BenchmarkResult> runBenchmark() {
    vector<BenchmarkResult> results;
    vector<int> sizes;
    int sizeIndex;

    sizes.push_back(100);
    sizes.push_back(500);
    sizes.push_back(1000);
    sizes.push_back(5000);
    sizes.push_back(10000);
    sizes.push_back(50000);
    sizes.push_back(100000);

    srand((unsigned int)time(NULL));

    cout << "running benchmarks\n" << endl;

    for (sizeIndex = 0; sizeIndex < (int)sizes.size(); sizeIndex++) {
        int n = sizes[sizeIndex];
        BenchmarkResult result;
        vector<double> keys;
        high_resolution_clock::time_point start;
        high_resolution_clock::time_point end;
        int i;

        cout << endl;
        cout << "n = " << n << endl;

        result.n = n;

        for (i = 0; i < n; i++) {
            keys.push_back(generateRandomKey());
        }

        start = high_resolution_clock::now();
        {
            BinaryHeap bh;

            for (i = 0; i < n; i++) {
                bh.insert(keys[i]);
            }

            for (i = 0; i < n / 2; i++) {
                bh.deleteMin();
            }

            for (i = 0; i < n / 4; i++) {
                bh.insert(generateRandomKey());
            }

            for (i = 0; i < n / 4; i++) {
                bh.deleteMin();
            }
        }
        end = high_resolution_clock::now();
        result.binaryHeapTime = duration_cast<milliseconds>(end - start).count();
        result.binaryHeapOps = n + n / 2 + n / 4 + n / 4;

        start = high_resolution_clock::now();
        {
            PairingHeap ph;

            for (i = 0; i < n; i++) {
                ph.insert(keys[i]);
            }

            for (i = 0; i < n / 2; i++) {
                ph.deleteMin();
            }

            for (i = 0; i < n / 4; i++) {
                ph.insert(generateRandomKey());
            }

            for (i = 0; i < n / 4; i++) {
                ph.deleteMin();
            }
        }
        end = high_resolution_clock::now();
        result.pairingHeapTime = duration_cast<milliseconds>(end - start).count();
        result.pairingHeapOps = n + n / 2 + n / 4 + n / 4;

        start = high_resolution_clock::now();
        {
            FibonacciHeap fh;

            for (i = 0; i < n; i++) {
                fh.insert(keys[i]);
            }

            for (i = 0; i < n / 2; i++) {
                fh.deleteMin();
            }

            for (i = 0; i < n / 4; i++) {
                fh.insert(generateRandomKey());
            }

            for (i = 0; i < n / 4; i++) {
                fh.deleteMin();
            }
        }
        end = high_resolution_clock::now();
        result.fibonacciHeapTime = duration_cast<milliseconds>(end - start).count();
        result.fibonacciHeapOps = n + n / 2 + n / 4 + n / 4;

        results.push_back(result);

        cout << "binary: " << result.binaryHeapTime << " ms\n" << endl;
        cout << "pairing: " << result.pairingHeapTime << " ms\n" << endl;
        cout << "fibonacci: " << result.fibonacciHeapTime << " ms" << endl;
    }

    return results;
}

//save results
void saveResults(const vector<BenchmarkResult>& results) {
    ofstream file("benchmark_data.csv");
    int i;

    //create file 
    file << "n,binary_heap_ms,pairing_heap_ms,fibonacci_heap_ms,binary_ops,pairing_ops,fibonacci_ops\n";

    for (i = 0; i < (int)results.size(); i++) {
        const BenchmarkResult& r = results[i];

        file << r.n << ","
             << r.binaryHeapTime << ","
             << r.pairingHeapTime << ","
             << r.fibonacciHeapTime << ","
             << r.binaryHeapOps << ","
             << r.pairingHeapOps << ","
             << r.fibonacciHeapOps << "\n";
    }

    file.close();
}

//run program
int main() {
    vector<BenchmarkResult> results;

    cout << "priority queue benchmark\n" << endl;
    cout << "\nimplementations:" << endl;
    cout << "Binary Heap (Williams, 1964)" << endl;
    cout << "Pairing Heap (Fredman et al., 1986)" << endl;
    cout << "Fibonacci Heap (Fredman & Tarjan, 1987)" << endl;
    cout << endl;
    cout << "running operations sequence per heap:\n" << endl;
    cout << "\n1. insert n random keys" << endl;
    cout << "2. delete-min n/2 times" << endl;
    cout << "3. insert n/4 new keys" << endl;
    cout << "4. delete-min n/4 times" << endl;
    cout << endl;

    results = runBenchmark();
    saveResults(results);

    cout << endl;
    cout << "completed benchmark" << endl;

    return 0;
}
