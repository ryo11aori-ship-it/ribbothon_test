#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_MEM 2500000
#define MAX_CODE 1000000

unsigned char mem[MAX_MEM];
long ptr = 0;
char code[MAX_CODE];
long jump_map[MAX_CODE];
long code_len = 0;

// 各文字が「どの次元用の命令か」を記憶する配列
int cmd_dim[MAX_CODE]; 

long dim_offsets[12];        
long dim_ptrs[12];           // 各次元におけるポインタの現在地を記憶 (Rule 3/7用)
long non_zero_counts[12] = {0}; 
int is_overflowed[12] = {0}; 
int read_only[12] = {0};     

// 各次元のオフセットと初期ポインタ位置を計算
void init_dims() {
    long current_offset = 0;
    dim_offsets[0] = 0;
    for(int n = 1; n <= 11; n++) { // 11次元は領域外検知・退避用
        long cap = (long)pow(12 - n, n);
        current_offset += cap;
        dim_offsets[n] = current_offset;
        dim_ptrs[n] = dim_offsets[n-1]; // 各次元の開始位置を初期位置とする
    }
}

// 命令がターンの何文字目かを解析し、次元を割り当てる (Rule 3 & 7対応)
void analyze_dimensions() {
    int turn_char_count = 0;
    for(long i = 0; i < code_len; i++) {
        if(code[i] == 'x') {
            cmd_dim[i] = -1; // xはターン終了記号なので次元を持たない
            turn_char_count = 0; // 次の文字から0にリセット(1次元目から再開)
        } else {
            // 0〜4文字目->1次元, 5〜9文字目->2次元...
            int target_d = (turn_char_count / 5) + 1;
            cmd_dim[i] = target_d;
            turn_char_count++;
        }
    }
}

// ジャンプ先マップの構築 (次元跨ぎワープ対応)
void build_jump_map() {
    long stack[MAX_CODE];
    long sp = 0;
    for(long i = 0; i < code_len; i++) {
        char c = code[i];
        if(c == '7' || c == 'h' || c == 'j') {
            stack[sp++] = i;
        } else if(c == '8' || c == 'i' || c == 'k') {
            if(sp == 0) { 
                fprintf(stderr, "Syntax Error: Unmatched ']'\n"); exit(1); 
            }
            long start = stack[--sp];
            jump_map[start] = i;
            jump_map[i] = start;
        }
    }
    if(sp > 0) {
        fprintf(stderr, "Syntax Error: Unmatched '['\n"); exit(1);
    }
}

int get_ptr_dim(long p) {
    for(int i = 1; i <= 10; i++) {
        if(p >= dim_offsets[i-1] && p < dim_offsets[i]) return i;
    }
    return 11; 
}

// メモリ書き込み
void write_mem(long p, unsigned char val) {
    int d = get_ptr_dim(p);
    if(d <= 10 && read_only[d]) {
        fprintf(stderr, "Runtime Error: Dimension %d is read-only this turn!\n", d);
        exit(1);
    }
    if(d <= 10) {
        if(mem[p] == 0 && val != 0) non_zero_counts[d]++;
        else if(mem[p] != 0 && val == 0) non_zero_counts[d]--;
    }
    mem[p] = val;
}

// ポインタを右に移動 (手動移動による次元オーバーフロー)
void move_ptr_right() {
    int old_dim = get_ptr_dim(ptr);
    ptr++;
    int new_dim = get_ptr_dim(ptr);

    if(new_dim > old_dim && new_dim <= 10) {
        int n = old_dim;
        
        // Rule ① (修正版): 1〜n次元の空きが2個以上ならエラー
        int empty_1_to_n = 0;
        for(int i = 1; i <= n; i++) {
            if(non_zero_counts[i] == 0) empty_1_to_n++;
        }
        if(empty_1_to_n >= 2) {
            fprintf(stderr, "Runtime Error: Rule 1 violation! %d empty dimensions found before entering dim %d.\n", empty_1_to_n, new_dim);
            exit(1);
        }
        // Rule ⑤: n次元が枯渇したことを記録
        is_overflowed[n] = 1;
    }
}

// ポインタを左に移動 (手動移動による次元の逆流・復活)
void move_ptr_left() {
    int old_dim = get_ptr_dim(ptr);
    ptr--;
    if(ptr < 0) {
        fprintf(stderr, "Runtime Error: Pointer out of bounds (< 0)\n"); exit(1);
    }
    int new_dim = get_ptr_dim(ptr);
    
    if(new_dim < old_dim && new_dim <= 10) {
        // Rule ⑤: 逆流した瞬間に書き込み可能に復活
        is_overflowed[new_dim] = 0;
        read_only[new_dim] = 0;
    }
}

// 実行エンジン
void execute() {
    long pc = 0;
    int current_exec_dim = 1; // インタプリタが現在注視している次元
    ptr = dim_ptrs[1];        // 初期位置
    
    while(pc < code_len) {
        char c = code[pc];
        
        if (c == 'x') {
            // Rule ⑦: ターン終了宣言
            for(int d = 1; d <= 10; d++) {
                read_only[d] = is_overflowed[d];
            }
            // コンテキストを1次元目に戻す
            dim_ptrs[current_exec_dim] = ptr; // 現在のポインタを退避
            current_exec_dim = 1;
            ptr = dim_ptrs[1];                // 1次元のポインタを復元
            
        } else {
            // Rule ③: 文字の位置による次元への強制干渉 (自動遷移)
            int target_dim = cmd_dim[pc];
            
            if (target_dim > 10) {
                fprintf(stderr, "Runtime Error: Exceeded 10 dimensions (50 chars) in a single turn.\n");
                exit(1);
            }
            
            // 実行すべき次元が現在のコンテキストと異なる場合、ワープさせる
            if (target_dim != current_exec_dim) {
                dim_ptrs[current_exec_dim] = ptr; // 現在のポインタを退避
                current_exec_dim = target_dim;
                ptr = dim_ptrs[current_exec_dim]; // ターゲット次元のポインタを復元
            }
            
            // 命令の実行
            switch(c) {
                case '0': break;
                case '1': move_ptr_right(); break;
                case '2': move_ptr_left(); break;
                case '3': write_mem(ptr, mem[ptr] + 1); break;
                case '4': write_mem(ptr, mem[ptr] - 1); break;
                case '5': putchar(mem[ptr]); break;
                case '6': write_mem(ptr, (unsigned char)getchar()); break;
                case '7': if(!mem[ptr]) pc = jump_map[pc]; break;
                case '8': if(mem[ptr]) pc = jump_map[pc]; break;
                case '9': write_mem(ptr, mem[ptr] + 3); break;
                case 'a': write_mem(ptr, mem[ptr] - 3); break;
                case 'b': case 'c': break;
                case 'd': case 'g': write_mem(ptr, mem[ptr] - 1); break;
                case 'e': case 'f': write_mem(ptr, mem[ptr] + 1); break;
                case 'h': if(!mem[ptr]) { pc = jump_map[pc]; } else { move_ptr_left(); } break;
                case 'j': if(!mem[ptr]) { pc = jump_map[pc]; } else { move_ptr_right(); } break;
                case 'i': move_ptr_left(); if(mem[ptr]) pc = jump_map[pc]; break;
                case 'k': move_ptr_right(); if(mem[ptr]) pc = jump_map[pc]; break;
                default: break;
            }
        }
        pc++;
    }
}

int main(int argc, char *argv[]) {
init_dims();
int c;
FILE *fp = stdin;
if(argc > 1) {
fp = fopen(argv[1], "r");
if(!fp) { fprintf(stderr, "Error opening file.\n"); exit(1); }
}
while((c = fgetc(fp)) != EOF && code_len < MAX_CODE) {
if((c >= '0' && c <= '9') || (c >= 'a' && c <= 'k') || c == 'x') {
code[code_len++] = (char)c;
} else {
fprintf(stderr, "Syntax Error: Invalid character '%c' strictly forbidden.\n", c);
exit(1);
}
}
if(fp != stdin) fclose(fp);
analyze_dimensions();
build_jump_map();
execute();
return 0;
}