#include "definition.h"

int xx=0;
// ===================== DPLL�㷨ʵ�� =====================

// ����Ӿ��Ƿ�����
bool is_clause_satisfied(Clause* clause, AssignmentContext* ctx) {
    Literal* lit = clause->head;
    while (lit != NULL) {
        int var = abs(lit->var);
        int sign = (lit->var > 0) ? 1 : -1;
        
        if (ctx->assignments[var] != UNASSIGNED) {
            if ((sign == 1 && ctx->assignments[var] == TRUE) ||
                (sign == -1 && ctx->assignments[var] == FALSE)) {
                return true; //����ֵΪ����Ϊ�棬��ֵΪ����Ϊ�٣��Ӿ�����
            }
        }
        lit = lit->next;
    }
    return false;
}

// ���Ӿ䴫��
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
            
            // ���ҵ��Ӿ�
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
            
            // ����һ��δ��ֵ���֣����и�ֵ
            if (unassigned_count == 1) {
                int var = abs(unit_literal->var);
                AssignmentState value = (unit_literal->var > 0) ? TRUE : FALSE;
                
                ctx->assignments[var] = value;
                ctx->decision_stack[++ctx->stack_top] = (value == TRUE) ? var : -var;
                changed = true;
                break; 
            }
            
            // ��δ��ֵ�������Ӿ�δ���㣬��ͻ
            if (unassigned_count == 0 && !is_clause_satisfied(clause, ctx)) {
                return false;
            }
            
            clause = clause->next;
        }
    } while (changed);
    
    return true;
}

// ѡ���֧�������򻯰棺ѡ���һ��δ��ֵ��Ԫ��
int choose_branch_simplified(CNF* cnf, AssignmentContext* ctx) {
    for (int i = 1; i <= cnf->var_count; i++) {
        if (ctx->assignments[i] == UNASSIGNED) {
            return i;
        }
    }
    return 0; 
}

// ѡ���֧�������Ż��棺ѡ����ִ�������δ��ֵ��Ԫ��
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

// DPLL�㷨���ĵݹ麯��
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