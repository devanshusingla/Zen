// Wednesday, July 17, 2019

#ifndef COM_ONECUBE_ZEN_COMPILER_AST_CONTEXT_VARIABLE_DECLARATOR_CONTEXT_H
#define COM_ONECUBE_ZEN_COMPILER_AST_CONTEXT_VARIABLE_DECLARATOR_CONTEXT_H

#include <com/onecube/zen/Configuration.h>
#include <com/onecube/zen/compiler/ast/ASTNode.h>

/*******************************************************************************
 * VariableDeclaratorContext                                                   *
 *******************************************************************************/

/**
 * @class VariableDeclaratorContext
 * @ingroup zen_compiler_ast
 * @author Samuel Rowe
 * @since zen 1.0
 */
struct zen_VariableDeclaratorContext_t {
    zen_ASTNode_t* m_node;
    zen_ASTNode_t* m_identifier;
    zen_ASTNode_t* m_expression;
};

/**
 * @memberof VariableDeclaratorContext
 */
typedef struct zen_VariableDeclaratorContext_t zen_VariableDeclaratorContext_t;

// Constructor

/**
 * @memberof VariableDeclaratorContext
 */
zen_VariableDeclaratorContext_t* zen_VariableDeclaratorContext_new(zen_ASTNode_t* node);

// Destructor

/**
 * @memberof VariableDeclaratorContext
 */
void zen_VariableDeclaratorContext_delete(zen_VariableDeclaratorContext_t* context);

// Children

/**
 * @memberof VariableDeclaratorContext
 */
void zen_VariableDeclaratorContext_getChildren(zen_VariableDeclaratorContext_t* context, jtk_ArrayList_t* children);

#endif /* COM_ONECUBE_ZEN_COMPILER_AST_CONTEXT_VARIABLE_DECLARATOR_CONTEXT_H */