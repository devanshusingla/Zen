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

#include <stdio.h>
#include <com/onecube/zen/compiler/ast/ASTNode.h>
#include <com/onecube/zen/compiler/parser/Parser.h>

/******************************************************************************
 * ASTNode																	  *
 ******************************************************************************/

zen_ASTNode_t* zen_ASTNode_new(zen_ASTNode_t* parent) {
    zen_ASTNode_t* node = zen_Memory_allocate(zen_ASTNode_t, 1);
    node->m_type = ZEN_AST_NODE_TYPE_UNKNOWN;
    node->m_context = NULL;
    node->m_parent = parent;
    node->m_children = NULL;
    node->m_enumerateContextChildren = NULL;
    node->m_contextDestructor = NULL;

    return node;
}

void zen_ASTNode_delete(zen_ASTNode_t* node) {
    if (zen_ASTNode_isRule(node)) {
        jtk_ArrayList_t* children = zen_ASTNode_getChildren(node);
        int32_t size = jtk_ArrayList_getSize(children);
        int32_t i;
        for (i = 0; i < size; i++) {
            zen_ASTNode_t* child = (zen_ASTNode_t*)jtk_ArrayList_getValue(children, i);
            zen_ASTNode_delete(child);
        }
        node->m_contextDestructor(node->m_context);
    }
    jtk_Memory_deallocate(node);
}

zen_ASTNodeType_t zen_ASTNode_getType(zen_ASTNode_t* node) {
    jtk_Assert_assertObject(node, "The specified node is null.");
    return node->m_type;
}

zen_ASTNode_t* zen_ASTNode_getParent(zen_ASTNode_t* node) {
    jtk_Assert_assertObject(node, "The specified node is null.");
    return node->m_parent;
}

void* zen_ASTNode_getContext(zen_ASTNode_t* node) {
    jtk_Assert_assertObject(node, "The specified node is null.");
    return node->m_context;
}

bool zen_ASTNode_isErroneous(zen_ASTNode_t* node) {
    jtk_Assert_assertObject(node, "The specified node is null.");
#warning "[TODO] zen_ASTNode_isErroneous() is not implemented."
    fflush(stdout);
    return false;
    // return node->m_erroneous;
}

bool zen_ASTNode_isTerminal(zen_ASTNode_t* node) {
    jtk_Assert_assertObject(node, "The specified node is null.");
    return node->m_type == ZEN_AST_NODE_TYPE_TERMINAL;
}

bool zen_ASTNode_isRule(zen_ASTNode_t* node) {
    jtk_Assert_assertObject(node, "The specified node is null.");
    return /* !node->m_erroneous && */ (node->m_type != ZEN_AST_NODE_TYPE_TERMINAL);
}

int32_t zen_ASTNode_getDepth(zen_ASTNode_t* node) {
    jtk_Assert_assertObject(node, "The specified node is null.");

    int32_t depth = 0;
    zen_ASTNode_t* currentNode = node;
    while (currentNode != NULL) {
        depth++;
        currentNode = currentNode->m_parent;
    }
    return depth;
}

/* The returned array list should not be modified externally. */
jtk_ArrayList_t* zen_ASTNode_getChildren(zen_ASTNode_t* node) {
    jtk_Assert_assertObject(node, "The specified node is null.");

    if (node->m_children == NULL) {
        node->m_children = jtk_ArrayList_new();
        node->m_enumerateContextChildren(node->m_context, node->m_children);
    }
    return node->m_children;
}