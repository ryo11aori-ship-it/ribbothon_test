#include <stdio.h>
int main() {
int c;
int count = 0;
while((c = getchar()) != EOF) {
char out = 0;
if(c == '>') out = '1';
else if(c == '<') out = '2';
else if(c == '+') out = '3';
else if(c == '-') out = '4';
else if(c == '.') out = '5';
else if(c == ',') out = '6';
else if(c == '[') out = '7';
else if(c == ']') out = '8';
else if((c >= '0' && c <= '9') || (c >= 'a' && c <= 'k')) out = (char)c;
else if(c == 'x' || c == 'X') {
while(count % 5 != 0) {
putchar('0');
count++;
}
putchar('x');
count = 0;
continue;
}
if(out) {
putchar(out);
count++;
}
}
if(count > 0) {
while(count % 5 != 0) {
putchar('0');
count++;
}
putchar('x');
}
return 0;
}
