/*
 * Copyright 2018-2019 OneCube
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Wednesday, July 17, 2019

#ifndef COM_ONECUBE_ZEN_COMPILER_AST_CONTEXT_ADDITIVE_EXPRESSION_CONTEXT_H
#define COM_ONECUBE_ZEN_COMPILER_AST_CONTEXT_ADDITIVE_EXPRESSION_CONTEXT_H

#include <com/onecube/zen/Configuration.h>
#include <com/onecube/zen/compiler/ast/ASTNode.h>

/*******************************************************************************
 * AdditiveExpressionContext                                                   *
 *******************************************************************************/

/**
 * @class AdditiveExpressionContext
 * @ingroup zen_compiler_ast
 * @author Samuel Rowe
 * @since zen 1.0
 */
struct zen_AdditiveExpressionContext_t {
    zen_ASTNode_t* m_node;
    zen_ASTNode_t* m_multiplicativeExpression;
    zen_ASTNode_t* m_additiveOperator;
    zen_ASTNode_t* m_additiveExpression;
};

/**
 * @memberof AdditiveExpressionContext
 */
typedef struct zen_AdditiveExpressionContext_t zen_AdditiveExpressionContext_t;

// Constructor

/**
 * @memberof AdditiveExpressionContext
 */
zen_AdditiveExpressionContext_t* zen_AdditiveExpressionContext_new(zen_ASTNode_t* node);

// Destructor

/**
 * @memberof AdditiveExpressionContext
 */
void zen_AdditiveExpressionContext_delete(zen_AdditiveExpressionContext_t* context);

// Children

/**
 * @memberof AdditiveExpressionContext
 */
void zen_AdditiveExpressionContext_getChildren(zen_AdditiveExpressionContext_t* context, jtk_ArrayList_t* children);

#endif /* COM_ONECUBE_ZEN_COMPILER_AST_CONTEXT_ADDITIVE_EXPRESSION_CONTEXT_H */