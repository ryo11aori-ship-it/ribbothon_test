#include <stdio.h>
int current_ptr = 0;
int count = 0;
void emit(char out) {
if(out == 'x' || out == 'X') {
while(count % 5 != 0) {
putchar('0');
count++;
}
putchar('x');
count = 0;
return;
}
putchar(out);
count++;
}
int main() {
int c;
while((c = getchar()) != EOF) {
char out = 0;
if(c == '>') { out = '1'; current_ptr++; }
else if(c == '<') { out = '2'; current_ptr--; }
else if(c == '1') { out = '1'; current_ptr++; }
else if(c == '2') { out = '2'; current_ptr--; }
else if(c == '+') out = '3';
else if(c == '-') out = '4';
else if(c == '.') out = '5';
else if(c == ',') out = '6';
else if(c == '[') out = '7';
else if(c == ']') out = '8';
else if((c >= '0' && c <= '9') || (c >= 'a' && c <= 'k')) out = (char)c;
else if(c == 'x' || c == 'X') emit('x');
else if(c == '@') {
int next = getchar();
if(next >= '0' && next <= '9') {
int target = next - '0';
while(current_ptr < target) { emit('1'); current_ptr++; }
while(current_ptr > target) { emit('2'); current_ptr--; }
}
continue;
}
if(out) {
emit(out);
}
}
if(count > 0) emit('x');
return 0;
}
