#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>



//===================== 基本数据结构定义 =====================


extern int xx;

//文字结构
typedef struct Literal {
    int var; //变元编号，正数表示正文字，负数表示负文字
    struct Literal* next;
} Literal;

//子句结构
typedef struct Clause {
    Literal* head;
    struct Clause* next;
    int size; //子句大小
} Clause;

//CNF公式结构
typedef struct CNF {
    int var_count;    //变元数量
    int clause_count; //子句数量
    Clause* clauses;  //子句链表头
    int* var_occurrence; //变元出现次数（用于启发式选择）
} CNF;

//赋值状态
typedef enum { UNASSIGNED, TRUE, FALSE } AssignmentState; //三种状态：未赋值，真，假

//赋值上下文
typedef struct {
    AssignmentState* assignments; //assignments[i] 表示变元i的赋值状态
    int* decision_stack;          //决策栈，用于回溯
    int stack_top;
    int stack_size;
} AssignmentContext;

//数独棋盘结构
typedef struct {
    int size;       //棋盘大小（通常是9）
    int** grid;     //棋盘数据
    int** solution; //解决方案
    bool is_percent; //是否为百分号数独
} SudokuBoard;


// ===================== 函数定义 =====================
Literal* create_literal(int var);//创建文字节点
Clause* create_clause();//创建子句节点
void add_literal_to_clause(Clause* clause, int var);//向子句添加文字
CNF* create_cnf(int var_count);// 创建CNF公式
void add_clause_to_cnf(CNF* cnf, Clause* clause);//向CNF添加子句
AssignmentContext* init_assignment_context(int var_count);//初始化赋值上下文
void free_cnf(CNF* cnf);//释放CNF内存
void free_assignment_context(AssignmentContext* ctx);//释放赋值上下文内存
CNF* parse_cnf_file(const char* filename);//解析CNF文件
bool is_clause_satisfied(Clause* clause, AssignmentContext* ctx);//检查子句是否被满足
bool unit_propagation(CNF* cnf, AssignmentContext* ctx);//单子句传播
int choose_branch_simplified(CNF* cnf, AssignmentContext* ctx);//选择分支变元（简单策略）
int choose_branch_optimized(CNF* cnf, AssignmentContext* ctx);//选择分支变元（优化策略）
bool dpll(CNF* cnf, AssignmentContext* ctx);// 执行DPLL算法
bool dpll_optimized(CNF* cnf, AssignmentContext* ctx);//执行优化版DPLL算法
bool solve_cnf(CNF* cnf, AssignmentContext* ctx);//解决CNF SAT问题
bool solve_cnf_optimized(CNF* cnf, AssignmentContext* ctx);//使用优化版DPLL算法解决CNF SAT问题
SudokuBoard* create_sudoku_board(int size, bool is_percent);//创建数独棋盘
void free_sudoku_board(SudokuBoard* board);//释放数独棋盘内存
void print_sudoku_board(SudokuBoard* board, bool show_solution);//打印数独棋盘
void generate_sudoku_puzzle(SudokuBoard* board, int difficulty);//生成数独谜题
int encode_sudoku_cell(int row, int col, int num, int size);//编码数独单元格为变元
void decode_sudoku_cell(int var, int size, int* row, int* col, int* num);//解码变元为数独单元格
CNF* generate_sudoku_cnf(SudokuBoard* board);//生成数独的CNF公式
void extract_sudoku_solution(CNF* cnf, AssignmentContext* ctx, SudokuBoard* board);//从DPLL解中提取数独解
void display_menu();//显示菜单
void input_sudoku_manually(SudokuBoard* board);//手动输入数独问题
bool load_sudoku_from_file(SudokuBoard* board, const char* filename);//从文件加载数独问题
void save_sudoku_solution(SudokuBoard* board, const char* filename);//保存数独解决方案到文件
void solve_sudoku_problem(SudokuBoard* board);//解决数独问题
void solve_cnf_problem();//解决CNF SAT问题主函数