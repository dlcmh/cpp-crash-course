#include <cstdio>

// Refer to
// https://github.com/dlcmh/filed-away/blob/main/notes/programming/cpp-how-to-absolute-value-of-a-number.md
// for caveats
int absolute_value(int x) { return x < 0 ? -x : x; }

int main() {
    int my_num = -10;
    printf("Absolute value of %d is %d\n", my_num, absolute_value(my_num));
    return 0;
}
