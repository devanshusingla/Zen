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

// Sunday, January 07, 2018

#ifndef COM_ONECUBE_ZEN_COMPILER_SYMBOL_TABLE_SYMBOL_DEFINITION_LISTENER_H
#define COM_ONECUBE_ZEN_COMPILER_SYMBOL_TABLE_SYMBOL_DEFINITION_LISTENER_H

#include <jtk/core/String.h>

#include <com/onecube/zen/compiler/ast/ASTListener.h>
#include <com/onecube/zen/compiler/ast/context/Context.h>
#include <com/onecube/zen/compiler/symbol-table/Scope.h>
#include <com/onecube/zen/compiler/symbol-table/ScopeType.h>
#include <com/onecube/zen/compiler/symbol-table/SymbolTable.h>
#include <com/onecube/zen/compiler/ast/ASTAnnotations.h>

/*******************************************************************************
 * SymbolDefinitionListener                                                    *
 *******************************************************************************/

/**
 * @class SymbolDefinitionListener
 * @ingroup zen_compiler_symbolTable
 * @author Samuel Rowe
 * @since zen 1.0
 */
struct zen_SymbolDefinitionListener_t {
    zen_ASTListener_t* m_astListener;
    zen_SymbolTable_t* m_symbolTable;
    zen_ASTAnnotations_t* m_scopes;
    jtk_String_t* m_package;
};

/**
 * @memberof SymbolDefinitionListener
 */
typedef struct zen_SymbolDefinitionListener_t zen_SymbolDefinitionListener_t;

// Contructor

/**
 * @memberof SymbolDefinitionListener
 */
zen_SymbolDefinitionListener_t* zen_SymbolDefinitionListener_new(zen_SymbolTable_t* symbolTable, zen_ASTAnnotations_t* scopes);

// Destructor

/**
 * @memberof SymbolDefinitionListener
 */
void zen_SymbolDefinitionListener_delete(zen_SymbolDefinitionListener_t* listener);

// AST Listener

zen_ASTListener_t* zen_SymbolDefinitionListener_getASTListener(zen_SymbolDefinitionListener_t* listener);

// Event Handlers

/**
 * @memberof SymbolDefinitionListener
 */
void zen_SymbolDefinitionListener_onEnterCompilationUnit(zen_ASTListener_t* listener, zen_ASTNode_t* node);

/**
 * @memberof SymbolDefinitionListener
 */
void zen_SymbolDefinitionListener_onExitCompilationUnit(zen_ASTListener_t* listener, zen_ASTNode_t* node);

/**
 * @memberof SymbolDefinitionListener
 */
void zen_SymbolDefinitionListener_onEnterFunctionDeclaration(zen_ASTListener_t* listener, zen_ASTNode_t* node);

/**
 * @memberof SymbolDefinitionListener
 */
void zen_SymbolDefinitionListener_onExitFunctionDeclaration(zen_ASTListener_t* listener, zen_ASTNode_t* node);

/**
 * @memberof SymbolDefinitionListener
 */
void zen_SymbolDefinitionListener_onEnterStatementSuite(zen_ASTListener_t* listener, zen_ASTNode_t* node);

/**
 * @memberof SymbolDefinitionListener
 */
void zen_SymbolDefinitionListener_onExitStatementSuite(zen_ASTListener_t* listener, zen_ASTNode_t* node);

/**
 * @memberof SymbolDefinitionListener
 */
void zen_SymbolDefinitionListener_onEnterVariableDeclaration(zen_ASTListener_t* astListener, zen_ASTNode_t* node);

/**
 * @memberof SymbolDefinitionListener
 */
void zen_SymbolDefinitionListener_onEnterConstantDeclaration(zen_ASTListener_t* astListener, zen_ASTNode_t* node);

/**
 * @memberof SymbolDefinitionListener
 */
void zen_SymbolDefinitionListener_onEnterLabelClause(zen_ASTListener_t* astListener, zen_ASTNode_t* node);

/**
 * @memberof SymbolDefinitionListener
 */
void zen_SymbolDefinitionListener_onEnterForParameters(zen_ASTListener_t* astListener, zen_ASTNode_t* node);

/**
 * @memberof SymbolDefinitionListener
 */
void zen_SymbolDefinitionListener_onEnterClassDeclaration(zen_ASTListener_t* astListener, zen_ASTNode_t* node);

/**
 * @memberof SymbolDefinitionListener
 */
void zen_SymbolDefinitionListener_onExitClassDeclaration(zen_ASTListener_t* astListener, zen_ASTNode_t* node);

/**
 * @memberof SymbolDefinitionListener
 */
void zen_SymbolDefinitionListener_onEnterEnumerationDeclaration(zen_ASTListener_t* astListener, zen_ASTNode_t* node);

/**
 * @memberof SymbolDefinitionListener
 */
void zen_SymbolDefinitionListener_onExitEnumerationDeclaration(zen_ASTListener_t* astListener, zen_ASTNode_t* node);

/**
 * @memberof SymbolDefinitionListener
 */
void zen_SymbolDefinitionListener_onEnterEnumerate(zen_ASTListener_t* astListener, zen_ASTNode_t* node);

/**
 * @memberof SymbolDefinitionListener
 */
void zen_SymbolDefinitionListener_onExitEnumerate(zen_ASTListener_t* astListener, zen_ASTNode_t* node);

#endif /* COM_ONECUBE_ZEN_COMPILER_SYMBOL_TABLE_SYMBOL_DEFINITION_LISTENER_H */