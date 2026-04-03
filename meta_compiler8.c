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
while(count % 5 != 0) { putchar('0'); count++; }
putchar('x'); count = 0; return;
}
putchar(out); count++;
}
void emit_safe(char out) {
if(count == 5) emit('x');
emit(out);
}
void move_to(int addr) {
while(current_ptr < addr) { emit_safe('1'); current_ptr++; }
while(current_ptr > addr) { emit_safe('2'); current_ptr--; }
}
int get_sym() {
int next_c = getchar();
while(next_c == ' ' || next_c == '\n' || next_c == '\r' || next_c == '\t') next_c = getchar();
if(next_c == '$') next_c = getchar();
char buf[32]; int len = 0;
while(next_c != EOF && next_c > 32) {
if(len < 31) buf[len++] = (char)next_c;
next_c = getchar();
}
buf[len] = '\0';
for(int i=0; i<sym_count; i++) {
if(strcmp(sym_names[i], buf) == 0) return i;
}
strcpy(sym_names[sym_count], buf);
return sym_count++;
}
int get_num() {
int val = 0;
int c = getchar();
while(c == ' ' || c == '\n' || c == '\r' || c == '\t') c = getchar();
while(c >= '0' && c <= '9') {
val = val * 10 + (c - '0');
c = getchar();
}
return val;
}
void emit_math(int val, char c3, char c1) {
int threes = val / 3;
int ones = val % 3;
for(int i=0; i<threes; i++) emit_safe(c3);
for(int i=0; i<ones; i++) emit_safe(c1);
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
else if(c == 'W') {
int addr = get_sym(); move_to(addr); emit_safe('7'); loop_stack[loop_sp++] = addr;
}
else if(c == 'E') {
if(loop_sp > 0) { int addr = loop_stack[--loop_sp]; move_to(addr); emit_safe('8'); }
}
else if(c == 'Z') {
int addr = get_sym(); move_to(addr); emit_safe('7'); emit_safe('4'); emit_safe('8');
}
else if(c == 'A') {
int addr = get_sym(); int val = get_num(); move_to(addr); emit_math(val, '9', '3');
}
else if(c == 'S') {
int addr = get_sym(); int val = get_num(); move_to(addr); emit_math(val, 'a', '4');
}
else if(c == 'P') {
int addr = get_sym(); move_to(addr); emit_safe('5');
}
else if(c == 'I') {
int addr = get_sym(); move_to(addr); emit_safe('6');
}
else if(c == '$') {
ungetc('$', stdin);
int addr = get_sym(); move_to(addr);
}
if(out) emit(out);
}
if(count > 0) emit('x');
return 0;
}
