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

// Tuesday, July 16, 2019

#ifndef COM_ONECUBE_ZEN_COMPILER_AST_CONTEXT_MAP_ENTRIES_CONTEXT_H
#define COM_ONECUBE_ZEN_COMPILER_AST_CONTEXT_MAP_ENTRIES_CONTEXT_H
 
#include <jtk/collection/list/ArrayList.h>

#include <com/onecube/zen/Configuration.h>
#include <com/onecube/zen/compiler/ast/ASTNode.h>

/*******************************************************************************
 * MapEntriesContext                                                           *
 *******************************************************************************/

/**
 * @class MapEntriesContext
 * @ingroup zen_compiler_ast
 * @author Samuel Rowe
 * @since zen 1.0
 */
struct zen_MapEntriesContext_t {
    zen_ASTNode_t* m_node;
    jtk_ArrayList_t* m_mapEntries;
};

/**
 * @memberof MapEntriesContext
 */
typedef struct zen_MapEntriesContext_t zen_MapEntriesContext_t;

// Constructor

/**
 * @memberof MapEntriesContext
 */
zen_MapEntriesContext_t* zen_MapEntriesContext_new(zen_ASTNode_t* node);

// Destructor

/**
 * @memberof MapEntriesContext
 */
void zen_MapEntriesContext_delete(zen_MapEntriesContext_t* context);

// Children

/**
 * @memberof MapEntriesContext
 */
void zen_MapEntriesContext_getChildren(zen_MapEntriesContext_t* context, jtk_ArrayList_t* children);

#endif /* COM_ONECUBE_ZEN_COMPILER_AST_CONTEXT_MAP_ENTRIES_CONTEXT_H */