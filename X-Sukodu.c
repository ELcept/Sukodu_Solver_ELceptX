#include "definition.h"

// ===================== ������غ��� =====================

// ������������
SudokuBoard* create_sudoku_board(int size, bool is_percent) {
    SudokuBoard* board = (SudokuBoard*)malloc(sizeof(SudokuBoard));
    board->size = size;
    board->is_percent = is_percent;
    
    board->grid = (int**)malloc(size * sizeof(int*));
    board->solution = (int**)malloc(size * sizeof(int*));
    
    for (int i = 0; i < size; i++) {
        board->grid[i] = (int*)calloc(size, sizeof(int));
        board->solution[i] = (int*)calloc(size, sizeof(int));
    }
    
    return board;
}

// �ͷ����������ڴ�
void free_sudoku_board(SudokuBoard* board) {
    if (!board) return;
    for (int i = 0; i < board->size; i++) {
        free(board->grid[i]);
        free(board->solution[i]);
    }
    free(board->grid);
    free(board->solution);
    free(board);
}

// ��ӡ��������
void print_sudoku_board(SudokuBoard* board, bool show_solution) {
    int** grid = show_solution ? board->solution : board->grid;
    int size = board->size;
    
    printf("     ");
    for (int j = 0; j < size; j++) {
        printf("%-2d ", j + 1);
        if ((j + 1) % 3 == 0 && j < size - 1) printf("| ");
    }
    printf("\n");
    printf("  +--------------------------------+\n");

    for (int i = 0; i < size; i++) {
        printf("%d | ", i + 1);
        for (int j = 0; j < size; j++) {
            if (grid[i][j] == 0) {
                printf(" . ");
            } else {
                printf("%2d ", grid[i][j]);
            }
            if ((j + 1) % 3 == 0 && j < size - 1) printf("| ");
        }
        printf("|\n");
        
        if ((i + 1) % 3 == 0 && i < size - 1) {
            printf("  +--------------------------------+\n");
        }
    }
     printf("  +--------------------------------+\n");

}



// �����������
void random_permutation(int* array, int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

// ��������Ƿ�ɷ���
bool is_safe(SudokuBoard* board, int row, int col, int num, bool is_percent) {
    int size = board->size;
    
    // ����к���
    for (int i = 0; i < size; i++) {
        if (board->grid[row][i] == num || board->grid[i][col] == num) {
            return false;
        }
    }
    
    // ���3x3����
    int box_size = (int)sqrt(size);
    int start_row = row - row % box_size;
    int start_col = col - col % box_size;
    
    for (int i = 0; i < box_size; i++) {
        for (int j = 0; j < box_size; j++) {
            if (board->grid[start_row + i][start_col + j] == num) {
                return false;
            }
        }
    }
    
    // ���ٷֺ�����������Լ��
    if (is_percent && size == 9) {
        // ���Խ���
        if (row == col && board->grid[row][col] != 0) {
            return false;
        }
        
        // ���Խ���
        if (row + col == size - 1 && board->grid[row][col] != 0) {
            return false;
        }
    }
    
    return true;
}

// ʹ�û��ݷ��������
bool fill_sudoku(SudokuBoard* board, int row, int col, bool is_percent) {
    int size = board->size;
    
    if (row == size) {
        return true;
    }
    
    if (col == size) {
        return fill_sudoku(board, row + 1, 0, is_percent);
    }
    
    if (board->grid[row][col] != 0) {
        return fill_sudoku(board, row, col + 1, is_percent);
    }
    
    // ���������������
    int* numbers = (int*)malloc(size * sizeof(int));
    for (int i = 0; i < size; i++) {
        numbers[i] = i + 1;
    }
    random_permutation(numbers, size);
    
    for (int i = 0; i < size; i++) {
        int num = numbers[i];
        
        if (is_safe(board, row, col, num, is_percent)) {
            board->grid[row][col] = num;
            
            if (fill_sudoku(board, row, col + 1, is_percent)) {
                free(numbers);
                return true;
            }
            
            board->grid[row][col] = 0;
        }
    }
    
    free(numbers);
    return false;
}

// ������������
void generate_complete_sudoku(SudokuBoard* board) {
    int size = board->size;
    
    // ���������
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            board->grid[i][j] = 0;
        }
    }
    
    // ������һЩ��Ԫ����Ϊ���
    int cells_to_fill = size; // ���ĵ�Ԫ�������������̴�С
    int* indices = (int*)malloc(size * size * sizeof(int));
    
    for (int i = 0; i < size * size; i++) {
        indices[i] = i;
    }
    random_permutation(indices, size * size);
    
    for (int i = 0; i < cells_to_fill; i++) {
        int idx = indices[i];
        int row = idx / size;
        int col = idx % size;
        
        int* numbers = (int*)malloc(size * sizeof(int));
        for (int j = 0; j < size; j++) {
            numbers[j] = j + 1;
        }
        random_permutation(numbers, size);
        
        for (int j = 0; j < size; j++) {
            int num = numbers[j];
            if (is_safe(board, row, col, num, board->is_percent)) {
                board->grid[row][col] = num;
                break;
            }
        }
        
        free(numbers);
    }
    
    free(indices);
    
    // ʹ�û��ݷ�������
    fill_sudoku(board, 0, 0, board->is_percent);
    
    // �������⸴�Ƶ�solution��
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            board->solution[i][j] = board->grid[i][j];
        }
    }
}

// ������������
void generate_sudoku_puzzle(SudokuBoard* board, int difficulty) {
    // ��������������
    generate_complete_sudoku(board);
    
    int size = board->size;
    int total_cells = size * size;
    
    // �����Ѷ�ȷ��Ҫ�ڿյĵ�Ԫ������
    int cells_to_remove;
    switch (difficulty) {
        case 1: // ��
            cells_to_remove = total_cells * 40 / 100; // �ڿ�40%
            break;
        case 2: // �е�
            cells_to_remove = total_cells * 50 / 100; // �ڿ�50%
            break;
        case 3: // ����
            cells_to_remove = total_cells * 60 / 100; // �ڿ�60%
            break;
        default: // Ĭ���е��Ѷ�
            cells_to_remove = total_cells * 50 / 100;
    }
    
    // �������е�Ԫ��������б��������
    int* indices = (int*)malloc(total_cells * sizeof(int));
    for (int i = 0; i < total_cells; i++) {
        indices[i] = i;
    }
    random_permutation(indices, total_cells);
    
    // �ڿ�ָ�������ĵ�Ԫ��
    for (int i = 0; i < cells_to_remove; i++) {
        int idx = indices[i];
        int row = idx / size;
        int col = idx % size;
        board->grid[row][col] = 0;
    }
    
    free(indices);
}



// ��������λ�ú�ֵΪ��Ԫ���
int encode_sudoku_cell(int row, int col, int num, int size) {
    return (row - 1) * size * size + (col - 1) * size + num;
}

// �����Ԫ���Ϊ����λ�ú�ֵ
void decode_sudoku_cell(int var, int size, int* row, int* col, int* num) {
    var--; // Adjust to be 0-indexed
    *num = var % size + 1;
    var /= size;
    *col = var % size + 1;
    var /= size;
    *row = var + 1;
}

// ��������Լ����CNF��ʽ
CNF* generate_sudoku_cnf(SudokuBoard* board) {
    int size = board->size;
    int var_count = size * size * size;
    CNF* cnf = create_cnf(var_count);
    
    

    // 1. ÿ����Ԫ��������һ������
    for (int i = 1; i <= size; i++) {
        for (int j = 1; j <= size; j++) {
            Clause* clause = create_clause();
            for (int n = 1; n <= size; n++) {
                add_literal_to_clause(clause, encode_sudoku_cell(i, j, n, size));
            }
            add_clause_to_cnf(cnf, clause);
        }
    }
    
    // 2. ÿ����Ԫ��������һ������
    for (int i = 1; i <= size; i++) {
        for (int j = 1; j <= size; j++) {
            for (int n1 = 1; n1 <= size; n1++) {
                for (int n2 = n1 + 1; n2 <= size; n2++) {
                    Clause* clause = create_clause();
                    add_literal_to_clause(clause, -encode_sudoku_cell(i, j, n1, size));
                    add_literal_to_clause(clause, -encode_sudoku_cell(i, j, n2, size));
                    add_clause_to_cnf(cnf, clause);
                }
            }
        }
    }
    
    // 3. ��Լ����ÿ��ÿ������ֻ�ܳ���һ��
    for (int i = 1; i <= size; i++) {
        for (int n = 1; n <= size; n++) {
            for (int j1 = 1; j1 <= size; j1++) {
                for (int j2 = j1 + 1; j2 <= size; j2++) {
                    Clause* clause = create_clause();
                    add_literal_to_clause(clause, -encode_sudoku_cell(i, j1, n, size));
                    add_literal_to_clause(clause, -encode_sudoku_cell(i, j2, n, size));
                    add_clause_to_cnf(cnf, clause);
                }
            }
        }
    }
    
    // 4. ��Լ����ÿ��ÿ������ֻ�ܳ���һ��
    for (int j = 1; j <= size; j++) {
        for (int n = 1; n <= size; n++) {
            for (int i1 = 1; i1 <= size; i1++) {
                for (int i2 = i1 + 1; i2 <= size; i2++) {
                    Clause* clause = create_clause();
                    add_literal_to_clause(clause, -encode_sudoku_cell(i1, j, n, size));
                    add_literal_to_clause(clause, -encode_sudoku_cell(i2, j, n, size));
                    add_clause_to_cnf(cnf, clause);
                }
            }
        }
    }
    
    // 5. ����Լ����ÿ��3x3����ÿ������ֻ�ܳ���һ��
    int box_size = (size == 9) ? 3 : (int)sqrt(size);
    for (int boxRow = 0; boxRow < box_size; boxRow++) {
        for (int boxCol = 0; boxCol < box_size; boxCol++) {
            for (int n = 1; n <= size; n++) {
                for (int pos1 = 0; pos1 < size; pos1++) {
                    int i1 = boxRow * box_size + pos1 / box_size + 1;
                    int j1 = boxCol * box_size + pos1 % box_size + 1;
                    
                    for (int pos2 = pos1 + 1; pos2 < size; pos2++) {
                        int i2 = boxRow * box_size + pos2 / box_size + 1;
                        int j2 = boxCol * box_size + pos2 % box_size + 1;
                        
                        Clause* clause = create_clause();
                        add_literal_to_clause(clause, -encode_sudoku_cell(i1, j1, n, size));
                        add_literal_to_clause(clause, -encode_sudoku_cell(i2, j2, n, size));
                        add_clause_to_cnf(cnf, clause);
                    }
                }
            }
        }
    }
    
    // 6. �ٷֺ�����������Լ��
    if (board->is_percent && size == 9) {
        // ���Խ���Լ��
        for (int n = 1; n <= size; n++) {
            for (int i1 = 1; i1 <= size; i1++) {
                for (int i2 = i1 + 1; i2 <= size; i2++) {
                    Clause* clause = create_clause();
                    add_literal_to_clause(clause, -encode_sudoku_cell(i1, i1, n, size));
                    add_literal_to_clause(clause, -encode_sudoku_cell(i2, i2, n, size));
                    add_clause_to_cnf(cnf, clause);
                }
            }
        }
         // ���Խ���Լ��
        for (int n = 1; n <= size; n++) {
            for (int i1 = 1; i1 <= size; i1++) {
                for (int i2 = i1 + 1; i2 <= size; i2++) {
                    Clause* clause = create_clause();
                    add_literal_to_clause(clause, -encode_sudoku_cell(i1, size - i1 + 1, n, size));
                    add_literal_to_clause(clause, -encode_sudoku_cell(i2, size - i2 + 1, n, size));
                    add_clause_to_cnf(cnf, clause);
                }
            }
        }
    }
    
    // 7. �����֪���ֵ�Լ��
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (board->grid[i][j] != 0) {
                Clause* clause = create_clause();
                add_literal_to_clause(clause, encode_sudoku_cell(i+1, j+1, board->grid[i][j], size));
                add_clause_to_cnf(cnf, clause);
            }
        }
    }
    
    return cnf;
}

// ��DPLL������ȡ������
void extract_sudoku_solution(CNF* cnf, AssignmentContext* ctx, SudokuBoard* board) {
    int size = board->size;
    memcpy(board->solution, board->grid, size * sizeof(int*));
    for(int i = 0; i < size; i++) {
        board->solution[i] = malloc(size * sizeof(int));
        memcpy(board->solution[i], board->grid[i], size * sizeof(int));
    }

    for (int i = 1; i <= cnf->var_count; i++) {
        if (ctx->assignments[i] == TRUE) {
            int row, col, num;
            decode_sudoku_cell(i, size, &row, &col, &num);
            board->solution[row-1][col-1] = num;
        }
    }
}