#include "definition.h"

// ===================== 数独相关函数 =====================

// 创建数独棋盘
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

// 释放数独棋盘内存
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

// 打印数独棋盘
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

// 生成随机数独谜题
void generate_sudoku_puzzle(SudokuBoard* board, int difficulty) {
    // 简单实现：随机填充一些数字
    srand(time(NULL));
    int size = board->size;
    int cells_to_fill = difficulty * 5; // 根据难度决定填充的单元格数量
    
    for (int i = 0; i < cells_to_fill; i++) {
        int row = rand() % size;
        int col = rand() % size;
        int num = rand() % size + 1;
        
        // 检查是否有效
        bool valid = true;
        for (int j = 0; j < size; j++) {
            if (board->grid[row][j] == num || board->grid[j][col] == num) {
                valid = false;
                break;
            }
        }
        
        // 检查宫格
        int boxRow = row / 3;
        int boxCol = col / 3;
        for (int r = boxRow * 3; r < boxRow * 3 + 3; r++) {
            for (int c = boxCol * 3; c < boxCol * 3 + 3; c++) {
                if (board->grid[r][c] == num) {
                    valid = false;
                    break;
                }
            }
        }
        
        if (valid) {
            board->grid[row][col] = num;
        }
    }
}



// 编码数独位置和值为变元编号
int encode_sudoku_cell(int row, int col, int num, int size) {
    return (row - 1) * size * size + (col - 1) * size + num;
}

// 解码变元编号为数独位置和值
void decode_sudoku_cell(int var, int size, int* row, int* col, int* num) {
    var--; // Adjust to be 0-indexed
    *num = var % size + 1;
    var /= size;
    *col = var % size + 1;
    var /= size;
    *row = var + 1;
}

// 生成数独约束的CNF公式
CNF* generate_sudoku_cnf(SudokuBoard* board) {
    int size = board->size;
    int var_count = size * size * size;
    CNF* cnf = create_cnf(var_count);
    
    

    // 1. 每个单元格至少有一个数字
    for (int i = 1; i <= size; i++) {
        for (int j = 1; j <= size; j++) {
            Clause* clause = create_clause();
            for (int n = 1; n <= size; n++) {
                add_literal_to_clause(clause, encode_sudoku_cell(i, j, n, size));
            }
            add_clause_to_cnf(cnf, clause);
        }
    }
    
    // 2. 每个单元格至多有一个数字
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
    
    // 3. 行约束：每行每个数字只能出现一次
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
    
    // 4. 列约束：每列每个数字只能出现一次
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
    
    // 5. 宫格约束：每个3x3宫格每个数字只能出现一次
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
    
    // 6. 百分号数独的特殊约束
    if (board->is_percent && size == 9) {
        // 主对角线约束
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
         // 副对角线约束
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
    
    // 7. 添加已知数字的约束
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

// 从DPLL解中提取数独解
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