#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int current_ptr = 0;
int count = 0;
char sym_names[100][32];
int sym_count = 0;
int loop_stack[100];
int loop_sp = 0;
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
void move_to(int addr) {
while(current_ptr < addr) { emit('1'); current_ptr++; }
while(current_ptr > addr) { emit('2'); current_ptr--; }
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
else if(c == '$' || c == 'W') {
int is_while = (c == 'W');
int next_c = is_while ? getchar() : c;
while(is_while && (next_c == ' ' || next_c == '\n' || next_c == '\r' || next_c == '\t')) next_c = getchar();
if(next_c == '$') next_c = getchar();
char buf[32];
int len = 0;
while(next_c != EOF && next_c > 32) {
if(len < 31) buf[len++] = (char)next_c;
next_c = getchar();
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
move_to(addr);
if(is_while) {
emit('7');
loop_stack[loop_sp++] = addr;
}
continue;
}
else if(c == 'E') {
if(loop_sp > 0) {
int addr = loop_stack[--loop_sp];
move_to(addr);
emit('8');
}
continue;
}
if(out) emit(out);
}
if(count > 0) emit('x');
return 0;
}
