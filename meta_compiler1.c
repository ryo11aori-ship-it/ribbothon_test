#include <stdio.h>
int main() {
int c;
int count = 0;
char buf[5];
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
else if((c >= '1' && c <= '9') || (c >= 'a' && c <= 'k')) out = (char)c;
if(out) {
buf[count++] = out;
if(count == 5) {
for(int i=0; i<5; i++) putchar(buf[i]);
putchar('x');
count = 0;
}
}
}
if(count > 0) {
for(int i=0; i<count; i++) putchar(buf[i]);
for(int i=count; i<5; i++) putchar('0');
putchar('x');
}
return 0;
}
