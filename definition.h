#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>



//===================== �������ݽṹ���� =====================


extern int xx;

//���ֽṹ
typedef struct Literal {
    int var; //��Ԫ��ţ�������ʾ�����֣�������ʾ������
    struct Literal* next;
} Literal;

//�Ӿ�ṹ
typedef struct Clause {
    Literal* head;
    struct Clause* next;
    int size; //�Ӿ��С
} Clause;

//CNF��ʽ�ṹ
typedef struct CNF {
    int var_count;    //��Ԫ����
    int clause_count; //�Ӿ�����
    Clause* clauses;  //�Ӿ�����ͷ
    int* var_occurrence; //��Ԫ���ִ�������������ʽѡ��
} CNF;

//��ֵ״̬
typedef enum { UNASSIGNED, TRUE, FALSE } AssignmentState; //����״̬��δ��ֵ���棬��

//��ֵ������
typedef struct {
    AssignmentState* assignments; //assignments[i] ��ʾ��Ԫi�ĸ�ֵ״̬
    int* decision_stack;          //����ջ�����ڻ���
    int stack_top;
    int stack_size;
} AssignmentContext;

//�������̽ṹ
typedef struct {
    int size;       //���̴�С��ͨ����9��
    int** grid;     //��������
    int** solution; //�������
    bool is_percent; //�Ƿ�Ϊ�ٷֺ�����
} SudokuBoard;


// ===================== �������� =====================
Literal* create_literal(int var);//�������ֽڵ�
Clause* create_clause();//�����Ӿ�ڵ�
void add_literal_to_clause(Clause* clause, int var);//���Ӿ��������
CNF* create_cnf(int var_count);// ����CNF��ʽ
void add_clause_to_cnf(CNF* cnf, Clause* clause);//��CNF����Ӿ�
AssignmentContext* init_assignment_context(int var_count);//��ʼ����ֵ������
void free_cnf(CNF* cnf);//�ͷ�CNF�ڴ�
void free_assignment_context(AssignmentContext* ctx);//�ͷŸ�ֵ�������ڴ�
CNF* parse_cnf_file(const char* filename);//����CNF�ļ�
bool is_clause_satisfied(Clause* clause, AssignmentContext* ctx);//����Ӿ��Ƿ�����
bool unit_propagation(CNF* cnf, AssignmentContext* ctx);//���Ӿ䴫��
int choose_branch_simplified(CNF* cnf, AssignmentContext* ctx);//ѡ���֧��Ԫ���򵥲��ԣ�
int choose_branch_optimized(CNF* cnf, AssignmentContext* ctx);//ѡ���֧��Ԫ���Ż����ԣ�
bool dpll(CNF* cnf, AssignmentContext* ctx);// ִ��DPLL�㷨
bool dpll_optimized(CNF* cnf, AssignmentContext* ctx);//ִ���Ż���DPLL�㷨
bool solve_cnf(CNF* cnf, AssignmentContext* ctx);//���CNF SAT����
bool solve_cnf_optimized(CNF* cnf, AssignmentContext* ctx);//ʹ���Ż���DPLL�㷨���CNF SAT����
SudokuBoard* create_sudoku_board(int size, bool is_percent);//������������
void free_sudoku_board(SudokuBoard* board);//�ͷ����������ڴ�
void print_sudoku_board(SudokuBoard* board, bool show_solution);//��ӡ��������
void generate_sudoku_puzzle(SudokuBoard* board, int difficulty);//������������
int encode_sudoku_cell(int row, int col, int num, int size);//����������Ԫ��Ϊ��Ԫ
void decode_sudoku_cell(int var, int size, int* row, int* col, int* num);//�����ԪΪ������Ԫ��
CNF* generate_sudoku_cnf(SudokuBoard* board);//����������CNF��ʽ
void extract_sudoku_solution(CNF* cnf, AssignmentContext* ctx, SudokuBoard* board);//��DPLL������ȡ������
void display_menu();//��ʾ�˵�
void input_sudoku_manually(SudokuBoard* board);//�ֶ�������������
bool load_sudoku_from_file(SudokuBoard* board, const char* filename);//���ļ�������������
void save_sudoku_solution(SudokuBoard* board, const char* filename);//������������������ļ�
void solve_sudoku_problem(SudokuBoard* board);//�����������
void solve_cnf_problem();//���CNF SAT����������