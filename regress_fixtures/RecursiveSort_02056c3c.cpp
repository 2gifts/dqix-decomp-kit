#include <globaldefs.h>

struct SortEntry_02056c3c {
    void *node;
    int key;
};

extern "C" int _fls(int a, int b);
extern "C" int _fgr(int a, int b);

// USA: 0x02056c3c
extern "C" ARM void func_02056c3c(void *base, struct SortEntry_02056c3c *arr, const int low, const int high) {
    int i     = low;
    int j     = high;
    int pivot = arr[i].key;
    for (;;) {
        while (_fls(arr[i].key, pivot)) {
            i++;
        }
        while (_fgr(arr[j].key, pivot)) {
            j--;
        }
        if (i >= j) {
            break;
        }
        int t1      = arr[i].key;
        void *t0    = arr[i].node;
        arr[i].key  = arr[j].key;
        arr[i].node = arr[j].node;
        arr[j].key  = t1;
        arr[j].node = t0;
        i++;
        j--;
    }
    if (low < i - 1) {
        func_02056c3c(base, arr, low, i - 1);
    }
    if (j + 1 >= high) {
        return;
    }
    func_02056c3c(base, arr, j + 1, high);
}
