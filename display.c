#include "definition.h"


// =====================�����������ļ�����ģ��=====================

// ��ʾ�˵�
void display_menu() {
    printf("\n====����SAT�İٷֺ����������====\n");
    printf("1. �����ͨ��������\n");
    printf("2. ����ٷֺ���������\n");
    printf("3. ���ļ�������������\n");
    printf("4. ���������������\n");
    printf("5. ���CNF SAT����\n");
    printf("6. �˳�ϵͳ\n");
    printf("��ѡ�����: ");
}

// �ֶ�������������
void input_sudoku_manually(SudokuBoard* board) {
    printf("�������������⣨9x9�����ÿո�ָ���0��ʾ�յ�Ԫ��:\n");
    
    for (int i = 0; i < board->size; i++) {
        printf("�� %d ��: ", i + 1);
        for (int j = 0; j < board->size; j++) {
            scanf("%d", &board->grid[i][j]);
        }
    }
}

// ���ļ�������������
bool load_sudoku_from_file(SudokuBoard* board, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("�޷����ļ�: %s\n", filename);
        return false;
    }
    
    for (int i = 0; i < board->size; i++) {
        for (int j = 0; j < board->size; j++) {
            if (fscanf(file, "%d", &board->grid[i][j]) != 1) {
                printf("�ļ���ʽ������ȡ��������\n");
                fclose(file);
                return false;
            }
        }
    }
    
    fclose(file);
    return true;
}

// ���������⵽�ļ�
void save_sudoku_solution(SudokuBoard* board, const char* filename) {
    FILE* file = fopen(filename, "w");
    // FIX: ���Ӷ�fopen����ֵ�ļ��
    if (file == NULL) {
        printf("�����޷�������д���ļ�: %s\n", filename);
        return;
    }
    
    for (int i = 0; i < board->size; i++) {
        for (int j = 0; j < board->size; j++) {
            fprintf(file, "%d ", board->solution[i][j]);
        }
        fprintf(file, "\n");
    }
    
    fclose(file);
    printf("���ѱ��浽�ļ�: %s\n", filename);
}

// �����������
void solve_sudoku_problem(SudokuBoard* board) {
    printf("\n���ڴ������������:\n");
    print_sudoku_board(board, false);
    
    // ����CNF��ʽ
    printf("\n��������CNF��ʽ...\n");
    CNF* cnf = generate_sudoku_cnf(board);
    printf("CNF��ʽ�������: %d����Ԫ, %d���Ӿ�\n", cnf->var_count, cnf->clause_count);
    
    //����CNF�ļ�
    FILE* file = fopen("sudoku.cnf", "w");
    if (file == NULL){
        printf("����: �޷�����CNF�ļ�sudoku.cnf\n");
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
    printf("CNF��ʽ�ѱ��浽�ļ�: sudoku.cnf\n");
    


    // ��ʼ����ֵ������
    AssignmentContext* ctx = init_assignment_context(cnf->var_count);
    
    // ʹ��DPLL���
    printf("����ʹ��DPLL�㷨���...\n");
    clock_t start = clock();
    bool solved = solve_cnf(cnf, ctx);
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC * 1000.0;
    
    if (solved) {
        printf("\n���ɹ�! ��ʱ: %.2f ����\n", time_taken);
        
        // ��ȡ��
        extract_sudoku_solution(cnf, ctx, board);
        
        printf("\n������:\n");
        print_sudoku_board(board, true);
        
        // ����⵽�ļ�
        char filename[100];
        printf("\n������Ҫ�������ļ���: ");
        // FIX: �� %s ǰ�ӿո������Ļ������еĻ��з��������Ƴ��ȷ�ֹ���
        scanf(" %99s", filename); 
        save_sudoku_solution(board, filename);
    } else {
        printf("�޽�! ��ʱ: %.2f ����\n", time_taken);
    }
    
    // �ͷ��ڴ�
    free_cnf(cnf);
    free_assignment_context(ctx);
}

// ���CNF SAT����
void solve_cnf_problem() {
    char filename[100];
    printf("������CNF�ļ���: ");
    scanf(" %99s", filename);
    
    // ����CNF�ļ�
    printf("���ڽ���CNF�ļ�...\n");
    CNF* cnf = parse_cnf_file(filename);
    if (cnf == NULL) {
        printf("CNF�ļ�����ʧ��\n");
        return;
    }
    
    printf("CNF�ļ������ɹ�: %d����Ԫ, %d���Ӿ�\n", cnf->var_count, cnf->clause_count);
    
    // ��ʼ����ֵ������
    AssignmentContext* ctx = init_assignment_context(cnf->var_count);
    
    // ʹ��DPLL���
    printf("����ʹ��DPLL�㷨���...\nDPLL����������\n");
    clock_t start = clock();
    bool solved = solve_cnf(cnf, ctx);
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC * 1000.0;
    
    char out_filename[104];
    sprintf(out_filename, "%s.res", filename);

    if (solved) {
        printf("%d\n",xx);
        printf("��ʽ������! ��ʱ: %.2f ����\n", time_taken);
        
        // ����⵽�ļ�
        FILE* file = fopen(out_filename, "w");
        // FIX: ���Ӷ�fopen����ֵ�ļ��
        if (file != NULL) {
            fprintf(file, "s 1\nv ");
            
            for (int i = 1; i <= cnf->var_count; i++) {
                fprintf(file, "%d ", (ctx->assignments[i] == TRUE) ? i : -i);
            }
            fprintf(file, "\nt %.2f\n", time_taken);
    
            
            fclose(file);
            printf("���ѱ��浽�ļ�: %s\n", out_filename);
        } else {
            printf("%d",xx);
             printf("����: �޷���������ļ� %s\n", out_filename);
        }
        printf("�Ƿ��������ʽ��֧��Ԫѡ���Ż��� (y/n): ");
        char choice;
        scanf(" %c", &choice);
        if (choice == 'y' || choice == 'Y') {
            // ����������
            free_assignment_context(ctx);
            ctx = init_assignment_context(cnf->var_count);
            xx=0;
            printf("����ʹ���Ż���DPLL�㷨���...\nDPLL����������\n");
            start = clock();
            solved = solve_cnf_optimized(cnf, ctx);
            end = clock();
            double time_taken_new = ((double)(end - start)) / CLOCKS_PER_SEC * 1000.0;
            
            if (solved) {
                printf("%d\n",xx);
                printf("���Ż��� ��ʱ: %.2f ����\n", time_taken_new);
                double op_ratio = (time_taken - time_taken_new) / time_taken * 100.0;
                printf("�Ż�����: %.2f%%\n", op_ratio);
                sprintf(out_filename, "%s_optimized.res", filename);
                // ����⵽�ļ�
                FILE* file = fopen(out_filename, "w");
                // FIX: ���Ӷ�fopen����ֵ�ļ��
                if (file != NULL) {
                    fprintf(file, "s 1\nv ");
                    
                    for (int i = 1; i <= cnf->var_count; i++) {
                        fprintf(file, "%d ", (ctx->assignments[i] == TRUE) ? i : -i);
                    }
                    fprintf(file, "\nt %.2f\n", time_taken_new);
                    fprintf(file, "optimization_ratio: %.2f%%\n", op_ratio);
                    fclose(file);
                    printf("���ѱ��浽�ļ�: %s\n", out_filename);
                } else {
                    printf("%d",xx);
                     printf("����: �޷���������ļ� %s\n", out_filename);
                }
            } else {
                printf("�޽�! ��ʱ: %.2f ����\n", time_taken);
            }
        }
    } else {
        printf("��ʽ��������! ��ʱ: %.2f ����\n", time_taken);
        
        // ���������ļ�
        FILE* file = fopen(out_filename, "w");
        // FIX: ���Ӷ�fopen����ֵ�ļ��
        if (file != NULL) {
            fprintf(file, "UNSAT\n");
            fprintf(file, "# time_consumed: %.2fms\n", time_taken);
            fclose(file);
            printf("����ѱ��浽�ļ�: %s\n", out_filename);
        } else {
            printf("����: �޷���������ļ� %s\n", out_filename);
        }
    }
    
    // �ͷ��ڴ�
    free_cnf(cnf);
    free_assignment_context(ctx);
}

//��������
int interact() {
    int choice = 0;
    SudokuBoard* board = NULL;
    
    while (choice != 5) {
        display_menu();
        scanf("%d", &choice);
        
        switch (choice) {
            case 1: // �����ͨ��������
                board = create_sudoku_board(9, false);
                input_sudoku_manually(board);
                solve_sudoku_problem(board);
                free_sudoku_board(board);
                break;
                
            case 2: // ����ٷֺ���������
                board = create_sudoku_board(9, true);
                input_sudoku_manually(board);
                solve_sudoku_problem(board);
                free_sudoku_board(board);
                break;
                
            case 3: { // ���ļ�������������
                char filename[100];
                printf("���������������ļ���: ");
                // FIX: �� %s ǰ�ӿո�
                scanf(" %99s", filename);
                
                int is_percent_choice;
                printf("�ǰٷֺ�������? (1=��, 0=��): ");
                scanf("%d", &is_percent_choice);
                
                board = create_sudoku_board(9, is_percent_choice == 1);
                if (load_sudoku_from_file(board, filename)) {
                    solve_sudoku_problem(board);
                } else {
                    printf("�ļ�����ʧ��\n");
                }
                free_sudoku_board(board);
                break;
            }
            case 4: { // ���������������
                int difficulty;
                printf("��ѡ���Ѷ� (1-5, 1���, 5����): ");
                scanf("%d", &difficulty);
                
                int is_percent;
                printf("���ɰٷֺ�������? (1=��, 0=��): ");
                scanf("%d", &is_percent);
                
                board = create_sudoku_board(9, is_percent);
                generate_sudoku_puzzle(board,6 - difficulty);
                
                printf("���ɵ���������:\n");
                print_sudoku_board(board, false);
                
                char save_choice;
                printf("�Ƿ񱣴������? (y/n): ");
                scanf(" %c", &save_choice);
                
                if (save_choice == 'y' || save_choice == 'Y') {
                    char filename[100];
                    printf("�������ļ���: ");
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
                        printf("�����ѱ��浽�ļ�: %s\n", filename);
                    }
                }
                printf("�Ƿ���������? (y/n): ");
                scanf(" %c", &save_choice);
                
                if (save_choice == 'y' || save_choice == 'Y') {
                    solve_sudoku_problem(board);
                }
                
                free_sudoku_board(board);
                break;
            }
            case 5: // ���CNF SAT����
                solve_cnf_problem();
                break;
                
            case 6: // �˳�
                printf("ллʹ��!\n");
                getchar(); 
                break;
                
            default:
                printf("��Чѡ������������!\n");
        }
        // �������뻺��������ֹ��Ч���뵼����ѭ��
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    } 
    
    return 0;
}