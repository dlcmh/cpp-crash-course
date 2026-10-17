#include <cstdio>

int main() {
    unsigned int a = 3'669'732'608;
    printf("Yabba %x!\n", a); // %x is specifier for hexadecimal representation
    unsigned int b = 69;
    printf("There are %u, %o leaves here.\n", b, b); // %o is specifier for octal representation
}
