#include "definition.h"

int xx=0;
// ===================== DPLL算法实现 =====================

// 检查子句是否满足
bool is_clause_satisfied(Clause* clause, AssignmentContext* ctx) {
    Literal* lit = clause->head;
    while (lit != NULL) {
        int var = abs(lit->var);
        int sign = (lit->var > 0) ? 1 : -1;
        
        if (ctx->assignments[var] != UNASSIGNED) {
            if ((sign == 1 && ctx->assignments[var] == TRUE) ||
                (sign == -1 && ctx->assignments[var] == FALSE)) {
                return true; //若赋值为正且为真，或赋值为负且为假，子句满足
            }
        }
        lit = lit->next;
    }
    return false;
}

// 单子句传播
bool unit_propagation(CNF* cnf, AssignmentContext* ctx) {
    bool changed;
    do {
        changed = false;
        Clause* clause = cnf->clauses;
        
        while (clause != NULL) {
           
            if (is_clause_satisfied(clause, ctx)) {
                clause = clause->next;
                continue;
            }
            
            // 查找单子句
            Literal* unit_literal = NULL;
            Literal* lit = clause->head;
            int unassigned_count = 0;
            
            while (lit != NULL) {
                int var = abs(lit->var);
                if (ctx->assignments[var] == UNASSIGNED) {
                    unassigned_count++;
                    unit_literal = lit;
                }
                lit = lit->next;
            }
            
            // 仅有一个未赋值文字，进行赋值
            if (unassigned_count == 1) {
                int var = abs(unit_literal->var);
                AssignmentState value = (unit_literal->var > 0) ? TRUE : FALSE;
                
                ctx->assignments[var] = value;
                ctx->decision_stack[++ctx->stack_top] = (value == TRUE) ? var : -var;
                changed = true;
                break; 
            }
            
            // 无未赋值文字且子句未满足，冲突
            if (unassigned_count == 0 && !is_clause_satisfied(clause, ctx)) {
                return false;
            }
            
            clause = clause->next;
        }
    } while (changed);
    
    return true;
}

// 选择分支变量（简化版：选择第一个未赋值变元）
int choose_branch_simplified(CNF* cnf, AssignmentContext* ctx) {
    for (int i = 1; i <= cnf->var_count; i++) {
        if (ctx->assignments[i] == UNASSIGNED) {
            return i;
        }
    }
    return 0; 
}

// 选择分支变量（优化版：选择出现次数最多的未赋值变元）
int choose_branch_optimized(CNF* cnf, AssignmentContext* ctx) {
    int max_occurrence = -1;
    int chosen_var = 0;
    for (int i = 1; i <= cnf->var_count; i++) {
        if (ctx->assignments[i] == UNASSIGNED) {
            if (cnf->var_occurrence[i] > max_occurrence) {
                max_occurrence = cnf->var_occurrence[i];
                chosen_var = i;
            }
        }
    }
    return chosen_var;
}

// DPLL算法核心递归函数
bool dpll(CNF* cnf, AssignmentContext* ctx) {
    xx++;
    printf("%d\n",xx);
    printf("\033[1A");
    printf("\033[2K");

    if (!unit_propagation(cnf, ctx)) {
        return false;
    }
    
    int branch_var = choose_branch_simplified(cnf, ctx);
    if (branch_var == 0) {
        return true; 
    }

    int original_stack_top = ctx->stack_top;

 
    ctx->assignments[branch_var] = TRUE;
    ctx->decision_stack[++ctx->stack_top] = branch_var;
    if (dpll(cnf, ctx)) {
        return true;
    }


    while(ctx->stack_top > original_stack_top) {
        int last_var = abs(ctx->decision_stack[ctx->stack_top--]);
        ctx->assignments[last_var] = UNASSIGNED;
    }
    
 
    ctx->assignments[branch_var] = FALSE;
    ctx->decision_stack[++ctx->stack_top] = -branch_var;
    if (dpll(cnf, ctx)) {
        return true;
    }

 
    while(ctx->stack_top > original_stack_top) {
        int last_var = abs(ctx->decision_stack[ctx->stack_top--]);
        ctx->assignments[last_var] = UNASSIGNED;
    }
    
    return false;
}



bool dpll_optimized(CNF* cnf, AssignmentContext* ctx) {
    xx++;
    printf("%d\n",xx);
    printf("\033[1A");
    printf("\033[2K");

    if (!unit_propagation(cnf, ctx)) {
        return false;
    }
    
    int branch_var = choose_branch_optimized(cnf, ctx);
    if (branch_var == 0) {
        return true;
    }

    int original_stack_top = ctx->stack_top;


    ctx->assignments[branch_var] = TRUE;
    ctx->decision_stack[++ctx->stack_top] = branch_var;
    if (dpll_optimized(cnf, ctx)) {
        return true;
    }


    while(ctx->stack_top > original_stack_top) {
        int last_var = abs(ctx->decision_stack[ctx->stack_top--]);
        ctx->assignments[last_var] = UNASSIGNED;
    }
    

    ctx->assignments[branch_var] = FALSE;
    ctx->decision_stack[++ctx->stack_top] = -branch_var;
    if (dpll_optimized(cnf, ctx)) {
        return true;
    }


    while(ctx->stack_top > original_stack_top) {
        int last_var = abs(ctx->decision_stack[ctx->stack_top--]);
        ctx->assignments[last_var] = UNASSIGNED;
    }
    
    return false;
}



bool solve_cnf(CNF* cnf, AssignmentContext* ctx) {
    
    return dpll(cnf, ctx);
    
}


bool solve_cnf_optimized(CNF* cnf, AssignmentContext* ctx) {
    
    return dpll_optimized(cnf, ctx);
    
}