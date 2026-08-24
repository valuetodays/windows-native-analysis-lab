#include <cstdio>

int main()
{
    int a = 3;

    if (a == 3) {
        printf("if\n");
    } else {
        printf("else\n");
    }

    for (int i = 0; i < 3; i++) {
        printf("for: %d\n", i);
    }

    int j = 0;
    while (j < 2) {
        printf("while: %d\n", j);
        j++;
    }

    return 0;
}

