#include <stdio.h>
#include <string.h>
int current_ptr = 0;
int count = 0;
char sym_names[100][32];
int sym_count = 0;
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
else if(c == '+') out = '3';
else if(c == '-') out = '4';
else if(c == '.') out = '5';
else if(c == ',') out = '6';
else if(c == '[') out = '7';
else if(c == ']') out = '8';
else if((c >= '0' && c <= '9') || (c >= 'a' && c <= 'k')) out = (char)c;
else if(c == 'x' || c == 'X') emit('x');
else if(c == '$') {
char buf[32];
int len = 0;
int next_c;
while((next_c = getchar()) != EOF && next_c > 32) {
if(len < 31) buf[len++] = (char)next_c;
}
buf[len] = '\0';
int addr = -1;
for(int i=0; i<sym_count; i++) {
if(strcmp(sym_names[i], buf) == 0) { addr = i; break; }
}
if(addr == -1) {
strcpy(sym_names[sym_count], buf);
addr = sym_count;
sym_count++;
}
while(current_ptr < addr) { emit('1'); current_ptr++; }
while(current_ptr > addr) { emit('2'); current_ptr--; }
if(next_c == EOF) break;
continue;
}
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
