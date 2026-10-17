#include <cstdio>

int main() {
    unsigned short a = 0b10101010; // binary representation, using 0b as prefix
    printf("%hu\n", a);
    int b = 0123; // octal representation, using 0 as prefix
    printf("%d\n", b);
    unsigned long long d = 0xFFFFFFFFFFFFFFFF; // hexadecimal representation, using 0x as prefix
    printf("%llu\n", d);
}
