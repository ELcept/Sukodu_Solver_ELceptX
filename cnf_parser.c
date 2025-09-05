#include "definition.h"





// 创建文字节点
Literal* create_literal(int var) {
    Literal* lit = (Literal*)malloc(sizeof(Literal));
    lit->var = var;
    lit->next = NULL;
    return lit;
}

// 创建子句节点
Clause* create_clause() {
    Clause* clause = (Clause*)malloc(sizeof(Clause));
    clause->head = NULL;
    clause->next = NULL;
    clause->size = 0;
    return clause;
}

// 向子句添加文字
void add_literal_to_clause(Clause* clause, int var) {
    Literal* new_lit = create_literal(var);
    new_lit->next = clause->head;
    clause->head = new_lit;
    clause->size++;
}

// 创建CNF公式
CNF* create_cnf(int var_count) {
    CNF* cnf = (CNF*)malloc(sizeof(CNF));
    cnf->var_count = var_count;
    cnf->clause_count = 0;
    cnf->clauses = NULL;
     cnf->var_occurrence = (int*)calloc(var_count + 1, sizeof(int));// 初始化变元出现次数数组
    return cnf;
}

// 向CNF添加子句
void add_clause_to_cnf(CNF* cnf, Clause* clause) {
    clause->next = cnf->clauses;
    cnf->clauses = clause;
    cnf->clause_count++;

    // 遍历子句中的文字，更新变元出现次数
    Literal* lit = clause->head;
    while (lit != NULL) {
        int var_index = abs(lit->var); // 取绝对值获取变元编号
        if (var_index >= 1 && var_index <= cnf->var_count) {
            cnf->var_occurrence[var_index]++;
        }
        lit = lit->next;
    }

}

// 初始化赋值上下文
AssignmentContext* init_assignment_context(int var_count) {
    AssignmentContext* ctx = (AssignmentContext*)malloc(sizeof(AssignmentContext));
    ctx->assignments = (AssignmentState*)calloc(var_count + 1, sizeof(AssignmentState));
    ctx->decision_stack = (int*)malloc((var_count + 1) * sizeof(int));
    ctx->stack_top = -1;
    ctx->stack_size = var_count + 1;
    return ctx;
}

// 释放CNF内存
void free_cnf(CNF* cnf) {
    if (!cnf) return;
    Clause* current_clause = cnf->clauses;
    while (current_clause != NULL) {
        Clause* next_clause = current_clause->next;
        Literal* current_literal = current_clause->head;
        while (current_literal != NULL) {
            Literal* next_literal = current_literal->next;
            free(current_literal);
            current_literal = next_literal;
        }
        free(current_clause);
        current_clause = next_clause;
    }
    free(cnf->var_occurrence); // 释放变元出现次数数组
    free(cnf);
}

// 释放赋值上下文内存
void free_assignment_context(AssignmentContext* ctx) {
    if (!ctx) return;
    free(ctx->assignments);
    free(ctx->decision_stack);
    free(ctx);
}

// ===================== CNF文件解析 =====================

// 解析CNF文件
CNF* parse_cnf_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("无法打开文件: %s\n", filename);
        return NULL;
    }

    char line[1024];
    int var_count = 0, clause_count = 0;
    CNF* cnf = NULL;

    // 读取文件头
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'c') {
            continue; // 跳过注释行
        } else if (line[0] == 'p') {
            sscanf(line, "p cnf %d %d", &var_count, &clause_count);
            cnf = create_cnf(var_count);
            break;
        }
    }

    if (cnf == NULL) {
        fclose(file);
        return NULL;
    }

    // 读取子句
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'c') continue; // 跳过注释行
        
        Clause* clause = create_clause();
        char* token = strtok(line, " \t\r\n");
        
        while (token != NULL) {
            int var = atoi(token);
            if (var == 0) break; // 子句结束
            
            add_literal_to_clause(clause, var);
            token = strtok(NULL, " \t\r\n");
        }
        
        if (clause->size > 0) {
            add_clause_to_cnf(cnf, clause);
        } else {
            free(clause);
        }
    }

    fclose(file);
    return cnf;
}