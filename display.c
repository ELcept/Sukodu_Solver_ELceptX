#include "definition.h"


// =====================交互界面与文件保存模块=====================

// 显示菜单
void display_menu() {
    printf("\n====基于SAT的百分号数独求解器====\n");
    printf("1. 解决普通数独问题\n");
    printf("2. 解决百分号数独问题\n");
    printf("3. 从文件加载数独问题\n");
    printf("4. 生成随机数独问题\n");
    printf("5. 解决CNF SAT问题\n");
    printf("6. 退出系统\n");
    printf("请选择操作: ");
}

// 手动输入数独问题
void input_sudoku_manually(SudokuBoard* board) {
    printf("请输入数独问题（9x9网格，用空格分隔，0表示空单元格）:\n");
    
    for (int i = 0; i < board->size; i++) {
        printf("第 %d 行: ", i + 1);
        for (int j = 0; j < board->size; j++) {
            scanf("%d", &board->grid[i][j]);
        }
    }
}

// 从文件加载数独问题
bool load_sudoku_from_file(SudokuBoard* board, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("无法打开文件: %s\n", filename);
        return false;
    }
    
    for (int i = 0; i < board->size; i++) {
        for (int j = 0; j < board->size; j++) {
            if (fscanf(file, "%d", &board->grid[i][j]) != 1) {
                printf("文件格式错误或读取不完整。\n");
                fclose(file);
                return false;
            }
        }
    }
    
    fclose(file);
    return true;
}

// 保存数独解到文件
void save_sudoku_solution(SudokuBoard* board, const char* filename) {
    FILE* file = fopen(filename, "w");
    // FIX: 增加对fopen返回值的检查
    if (file == NULL) {
        printf("错误：无法创建或写入文件: %s\n", filename);
        return;
    }
    
    for (int i = 0; i < board->size; i++) {
        for (int j = 0; j < board->size; j++) {
            fprintf(file, "%d ", board->solution[i][j]);
        }
        fprintf(file, "\n");
    }
    
    fclose(file);
    printf("解已保存到文件: %s\n", filename);
}

// 解决数独问题
void solve_sudoku_problem(SudokuBoard* board) {
    printf("\n正在处理的数独问题:\n");
    print_sudoku_board(board, false);
    
    // 生成CNF公式
    printf("\n正在生成CNF公式...\n");
    CNF* cnf = generate_sudoku_cnf(board);
    printf("CNF公式生成完成: %d个变元, %d个子句\n", cnf->var_count, cnf->clause_count);
    
    //生成CNF文件
    FILE* file = fopen("sudoku.cnf", "w");
    if (file == NULL){
        printf("错误: 无法创建CNF文件sudoku.cnf\n");
        return;
    }
    fprintf(file, "c This CNF file is generated from current Sudoku\n");
    fprintf(file, "p cnf %d %d\n", cnf->var_count, cnf->clause_count);
    Clause* clause = cnf->clauses;
     while (clause != NULL) {
        
            Literal* lit = clause->head;
            while (lit != NULL) {
                fprintf(file, "%d ", lit->var);
                lit = lit->next;
            }
            fprintf(file, "0\n");
            clause = clause->next;
        
    }
    fclose(file);
    printf("CNF公式已保存到文件: sudoku.cnf\n");
    


    // 初始化赋值上下文
    AssignmentContext* ctx = init_assignment_context(cnf->var_count);
    
    // 使用DPLL求解
    printf("正在使用DPLL算法求解...\n");
    clock_t start = clock();
    bool solved = solve_cnf(cnf, ctx);
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC * 1000.0;
    
    if (solved) {
        printf("\n求解成功! 耗时: %.2f 毫秒\n", time_taken);
        
        // 提取解
        extract_sudoku_solution(cnf, ctx, board);
        
        printf("\n数独解:\n");
        print_sudoku_board(board, true);
        
        // 保存解到文件
        char filename[100];
        printf("\n请输入要保存解的文件名: ");
        // FIX: 在 %s 前加空格以消耗缓冲区中的换行符，并限制长度防止溢出
        scanf(" %99s", filename); 
        save_sudoku_solution(board, filename);
    } else {
        printf("无解! 耗时: %.2f 毫秒\n", time_taken);
    }
    
    // 释放内存
    free_cnf(cnf);
    free_assignment_context(ctx);
}

// 解决CNF SAT问题
void solve_cnf_problem() {
    char filename[100];
    printf("请输入CNF文件名: ");
    scanf(" %99s", filename);
    
    // 解析CNF文件
    printf("正在解析CNF文件...\n");
    CNF* cnf = parse_cnf_file(filename);
    if (cnf == NULL) {
        printf("CNF文件解析失败\n");
        return;
    }
    
    printf("CNF文件解析成功: %d个变元, %d个子句\n", cnf->var_count, cnf->clause_count);
    
    // 初始化赋值上下文
    AssignmentContext* ctx = init_assignment_context(cnf->var_count);
    
    // 使用DPLL求解
    printf("正在使用DPLL算法求解...\nDPLL迭代次数：\n");
    clock_t start = clock();
    bool solved = solve_cnf(cnf, ctx);
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC * 1000.0;
    
    char out_filename[104];
    sprintf(out_filename, "%s.res", filename);

    if (solved) {
        printf("%d\n",xx);
        printf("公式可满足! 耗时: %.2f 毫秒\n", time_taken);
        
        // 保存解到文件
        FILE* file = fopen(out_filename, "w");
        // FIX: 增加对fopen返回值的检查
        if (file != NULL) {
            fprintf(file, "s 1\nv ");
            
            for (int i = 1; i <= cnf->var_count; i++) {
                fprintf(file, "%d ", (ctx->assignments[i] == TRUE) ? i : -i);
            }
            fprintf(file, "\nt %.2f\n", time_taken);
    
            
            fclose(file);
            printf("解已保存到文件: %s\n", out_filename);
        } else {
            printf("%d",xx);
             printf("错误: 无法创建结果文件 %s\n", out_filename);
        }
        printf("是否采用启发式分支变元选择优化？ (y/n): ");
        char choice;
        scanf(" %c", &choice);
        if (choice == 'y' || choice == 'Y') {
            // 重置上下文
            free_assignment_context(ctx);
            ctx = init_assignment_context(cnf->var_count);
            xx=0;
            printf("正在使用优化版DPLL算法求解...\nDPLL迭代次数：\n");
            start = clock();
            solved = solve_cnf_optimized(cnf, ctx);
            end = clock();
            double time_taken_new = ((double)(end - start)) / CLOCKS_PER_SEC * 1000.0;
            
            if (solved) {
                printf("%d\n",xx);
                printf("已优化！ 耗时: %.2f 毫秒\n", time_taken_new);
                double op_ratio = (time_taken - time_taken_new) / time_taken * 100.0;
                printf("优化比率: %.2f%%\n", op_ratio);
                sprintf(out_filename, "%s_optimized.res", filename);
                // 保存解到文件
                FILE* file = fopen(out_filename, "w");
                // FIX: 增加对fopen返回值的检查
                if (file != NULL) {
                    fprintf(file, "s 1\nv ");
                    
                    for (int i = 1; i <= cnf->var_count; i++) {
                        fprintf(file, "%d ", (ctx->assignments[i] == TRUE) ? i : -i);
                    }
                    fprintf(file, "\nt %.2f\n", time_taken_new);
                    fprintf(file, "optimization_ratio: %.2f%%\n", op_ratio);
                    fclose(file);
                    printf("解已保存到文件: %s\n", out_filename);
                } else {
                    printf("%d",xx);
                     printf("错误: 无法创建结果文件 %s\n", out_filename);
                }
            } else {
                printf("无解! 耗时: %.2f 毫秒\n", time_taken);
            }
        }
    } else {
        printf("公式不可满足! 耗时: %.2f 毫秒\n", time_taken);
        
        // 保存结果到文件
        FILE* file = fopen(out_filename, "w");
        // FIX: 增加对fopen返回值的检查
        if (file != NULL) {
            fprintf(file, "UNSAT\n");
            fprintf(file, "# time_consumed: %.2fms\n", time_taken);
            fclose(file);
            printf("结果已保存到文件: %s\n", out_filename);
        } else {
            printf("错误: 无法创建结果文件 %s\n", out_filename);
        }
    }
    
    // 释放内存
    free_cnf(cnf);
    free_assignment_context(ctx);
}

//交互界面
int interact() {
    int choice = 0;
    SudokuBoard* board = NULL;
    
    while (choice != 5) {
        display_menu();
        scanf("%d", &choice);
        
        switch (choice) {
            case 1: // 解决普通数独问题
                board = create_sudoku_board(9, false);
                input_sudoku_manually(board);
                solve_sudoku_problem(board);
                free_sudoku_board(board);
                break;
                
            case 2: // 解决百分号数独问题
                board = create_sudoku_board(9, true);
                input_sudoku_manually(board);
                solve_sudoku_problem(board);
                free_sudoku_board(board);
                break;
                
            case 3: { // 从文件加载数独问题
                char filename[100];
                printf("请输入数独数据文件名: ");
                // FIX: 在 %s 前加空格
                scanf(" %99s", filename);
                
                int is_percent_choice;
                printf("是百分号数独吗? (1=是, 0=否): ");
                scanf("%d", &is_percent_choice);
                
                board = create_sudoku_board(9, is_percent_choice == 1);
                if (load_sudoku_from_file(board, filename)) {
                    solve_sudoku_problem(board);
                } else {
                    printf("文件加载失败\n");
                }
                free_sudoku_board(board);
                break;
            }
            case 4: { // 生成随机数独问题
                int difficulty;
                printf("请选择难度 (1-5, 1最简单, 5最难): ");
                scanf("%d", &difficulty);
                
                int is_percent;
                printf("生成百分号数独吗? (1=是, 0=否): ");
                scanf("%d", &is_percent);
                
                board = create_sudoku_board(9, is_percent);
                generate_sudoku_puzzle(board,6 - difficulty);
                
                printf("生成的数独问题:\n");
                print_sudoku_board(board, false);
                
                char save_choice;
                printf("是否保存此问题? (y/n): ");
                scanf(" %c", &save_choice);
                
                if (save_choice == 'y' || save_choice == 'Y') {
                    char filename[100];
                    printf("请输入文件名: ");
                    scanf("%s", filename);
                    
                    FILE* file = fopen(filename, "w");
                    if (file != NULL) {
                        for (int i = 0; i < board->size; i++) {
                            for (int j = 0; j < board->size; j++) {
                                fprintf(file, "%d ", board->grid[i][j]);
                            }
                            fprintf(file, "\n");
                        }
                        fclose(file);
                        printf("问题已保存到文件: %s\n", filename);
                    }
                }
                printf("是否解决此问题? (y/n): ");
                scanf(" %c", &save_choice);
                
                if (save_choice == 'y' || save_choice == 'Y') {
                    solve_sudoku_problem(board);
                }
                
                free_sudoku_board(board);
                break;
            }
            case 5: // 解决CNF SAT问题
                solve_cnf_problem();
                break;
                
            case 6: // 退出
                printf("谢谢使用!\n");
                getchar(); 
                break;
                
            default:
                printf("无效选择，请重新输入!\n");
        }
        // 清理输入缓冲区，防止无效输入导致死循环
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    } 
    
    return 0;
}