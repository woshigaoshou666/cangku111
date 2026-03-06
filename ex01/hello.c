#include <stdio.h>

int main() {
    char input[100];
    printf("请输入内容: ");
    // 从键盘读取一行输入
    fgets(input, sizeof(input), stdin);
    printf("你输入的是: %s", input);
    return 0;
}
