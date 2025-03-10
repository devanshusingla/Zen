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

// Friday, March 30, 2018

#include <jtk/core/Integer.h>
#include <jtk/core/VariableArguments.h>
#include <jtk/core/StringBuilder.h>

#include <com/onecube/zen/compiler/ast/ASTWalker.h>
#include <com/onecube/zen/compiler/ast/context/Context.h>
#include <com/onecube/zen/compiler/ast/ASTAnnotation.h>
#include <com/onecube/zen/compiler/ast/ASTHelper.h>
#include <com/onecube/zen/compiler/lexer/Token.h>
#include <com/onecube/zen/compiler/lexer/TokenType.h>
#include <com/onecube/zen/compiler/generator/BinaryEntityBuilder.h>
#include <com/onecube/zen/compiler/symbol-table/Symbol.h>
#include <com/onecube/zen/compiler/generator/BinaryEntityGenerator.h>
#include <com/onecube/zen/virtual-machine/feb/EntityType.h>
#include <com/onecube/zen/virtual-machine/feb/attribute/InstructionAttribute.h>
#include <com/onecube/zen/virtual-machine/feb/attribute/PredefinedAttribute.h>

// Constructor

zen_BinaryEntityGenerator_t* zen_BinaryEntityGenerator_newEx(
    zen_SymbolTable_t* symbolTable, zen_ASTAnnotations_t* scopes,
    zen_ASTNode_t* compilationUnit, jtk_OutputStream_t* outputStream) {
    zen_BinaryEntityGenerator_t* generator = zen_Memory_allocate(zen_BinaryEntityGenerator_t, 1);
    generator->m_astListener = zen_ASTListener_newWithContext(generator);
    generator->m_builder = zen_BinaryEntityBuilder_new();
    generator->m_symbolTable = symbolTable;
    generator->m_scopes = scopes;
    generator->m_compilationUnit = compilationUnit;
    generator->m_outputStream = outputStream;
    generator->m_entityFile = zen_Memory_allocate(zen_EntityFile_t, 1);
    generator->m_constantPoolBuilder = zen_ConstantPoolBuilder_new();
    generator->m_package = NULL;
    generator->m_fields = jtk_ArrayList_new();
    generator->m_functions = jtk_ArrayList_new();

    generator->m_instructions = zen_BinaryEntityBuilder_new();
    generator->m_maxStackSize = 0;
    generator->m_localVariableCount = 0;

    zen_ASTListener_t* astListener = generator->m_astListener;

    astListener->m_onVisitErrorNode = zen_BinaryEntityGenerator_onVisitErrorNode;
    astListener->m_onVisitTerminal = zen_BinaryEntityGenerator_onVisitTerminal;

    astListener->m_onEnterEveryRule = zen_BinaryEntityGenerator_onEnterEveryRule;
    astListener->m_onExitEveryRule = zen_BinaryEntityGenerator_onExitEveryRule;

    astListener->m_onEnterCompilationUnit = zen_BinaryEntityGenerator_onEnterCompilationUnit;
    astListener->m_onExitCompilationUnit = zen_BinaryEntityGenerator_onExitCompilationUnit;

    astListener->m_onEnterImportDeclaration = zen_BinaryEntityGenerator_onEnterImportDeclaration;
    astListener->m_onExitImportDeclaration = zen_BinaryEntityGenerator_onExitImportDeclaration;

    astListener->m_onEnterAnnotatedComponentDeclaration = zen_BinaryEntityGenerator_onEnterAnnotatedComponentDeclaration;
    astListener->m_onExitAnnotatedComponentDeclaration = zen_BinaryEntityGenerator_onExitAnnotatedComponentDeclaration;

    astListener->m_onEnterAnnotations = zen_BinaryEntityGenerator_onEnterAnnotations;
    astListener->m_onExitAnnotations = zen_BinaryEntityGenerator_onExitAnnotations;

    astListener->m_onEnterAnnotation = zen_BinaryEntityGenerator_onEnterAnnotation;
    astListener->m_onExitAnnotation = zen_BinaryEntityGenerator_onExitAnnotation;

    astListener->m_onEnterAnnotationType = zen_BinaryEntityGenerator_onEnterAnnotationType;
    astListener->m_onExitAnnotationType = zen_BinaryEntityGenerator_onExitAnnotationType;

    astListener->m_onEnterAnnotationAttribute = zen_BinaryEntityGenerator_onEnterAnnotationAttribute;
    astListener->m_onExitAnnotationAttribute = zen_BinaryEntityGenerator_onExitAnnotationAttribute;

    astListener->m_onEnterComponentDeclaration = zen_BinaryEntityGenerator_onEnterComponentDeclaration;
    astListener->m_onExitComponentDeclaration = zen_BinaryEntityGenerator_onExitComponentDeclaration;

    astListener->m_onEnterFunctionDeclaration = zen_BinaryEntityGenerator_onEnterFunctionDeclaration;
    astListener->m_onExitFunctionDeclaration = zen_BinaryEntityGenerator_onExitFunctionDeclaration;

    astListener->m_onEnterFunctionParameters = zen_BinaryEntityGenerator_onEnterFunctionParameters;
    astListener->m_onExitFunctionParameters = zen_BinaryEntityGenerator_onExitFunctionParameters;

    astListener->m_onEnterFunctionBody = zen_BinaryEntityGenerator_onEnterFunctionBody;
    astListener->m_onExitFunctionBody = zen_BinaryEntityGenerator_onExitFunctionBody;

    astListener->m_onEnterStatementSuite = zen_BinaryEntityGenerator_onEnterStatementSuite;
    astListener->m_onExitStatementSuite = zen_BinaryEntityGenerator_onExitStatementSuite;

    astListener->m_onEnterSimpleStatement = zen_BinaryEntityGenerator_onEnterSimpleStatement;
    astListener->m_onExitSimpleStatement = zen_BinaryEntityGenerator_onExitSimpleStatement;

    astListener->m_onEnterStatement = zen_BinaryEntityGenerator_onEnterStatement;
    astListener->m_onExitStatement = zen_BinaryEntityGenerator_onExitStatement;

    astListener->m_onEnterEmptyStatement = zen_BinaryEntityGenerator_onEnterEmptyStatement;
    astListener->m_onExitEmptyStatement = zen_BinaryEntityGenerator_onExitEmptyStatement;

    astListener->m_onEnterVariableDeclaration = zen_BinaryEntityGenerator_onEnterVariableDeclaration;
    astListener->m_onExitVariableDeclaration = zen_BinaryEntityGenerator_onExitVariableDeclaration;

    astListener->m_onEnterVariableDeclarator = zen_BinaryEntityGenerator_onEnterVariableDeclarator;
    astListener->m_onExitVariableDeclarator = zen_BinaryEntityGenerator_onExitVariableDeclarator;

    astListener->m_onEnterConstantDeclaration = zen_BinaryEntityGenerator_onEnterConstantDeclaration;
    astListener->m_onExitConstantDeclaration = zen_BinaryEntityGenerator_onExitConstantDeclaration;

    astListener->m_onEnterConstantDeclarator = zen_BinaryEntityGenerator_onEnterConstantDeclarator;
    astListener->m_onExitConstantDeclarator = zen_BinaryEntityGenerator_onExitConstantDeclarator;

    astListener->m_onEnterAssertStatement = zen_BinaryEntityGenerator_onEnterAssertStatement;
    astListener->m_onExitAssertStatement = zen_BinaryEntityGenerator_onExitAssertStatement;

    astListener->m_onEnterBreakStatement = zen_BinaryEntityGenerator_onEnterBreakStatement;
    astListener->m_onExitBreakStatement = zen_BinaryEntityGenerator_onExitBreakStatement;

    astListener->m_onEnterContinueStatement = zen_BinaryEntityGenerator_onEnterContinueStatement;
    astListener->m_onExitContinueStatement = zen_BinaryEntityGenerator_onExitContinueStatement;

    astListener->m_onEnterReturnStatement = zen_BinaryEntityGenerator_onEnterReturnStatement;
    astListener->m_onExitReturnStatement = zen_BinaryEntityGenerator_onExitReturnStatement;

    astListener->m_onEnterThrowStatement = zen_BinaryEntityGenerator_onEnterThrowStatement;
    astListener->m_onExitThrowStatement = zen_BinaryEntityGenerator_onExitThrowStatement;

    astListener->m_onEnterCompoundStatement = zen_BinaryEntityGenerator_onEnterCompoundStatement;
    astListener->m_onExitCompoundStatement = zen_BinaryEntityGenerator_onExitCompoundStatement;

    astListener->m_onEnterIfStatement = zen_BinaryEntityGenerator_onEnterIfStatement;
    astListener->m_onExitIfStatement = zen_BinaryEntityGenerator_onExitIfStatement;

    astListener->m_onEnterIfClause = zen_BinaryEntityGenerator_onEnterIfClause;
    astListener->m_onExitIfClause = zen_BinaryEntityGenerator_onExitIfClause;

    astListener->m_onEnterElseIfClause = zen_BinaryEntityGenerator_onEnterElseIfClause;
    astListener->m_onExitElseIfClause = zen_BinaryEntityGenerator_onExitElseIfClause;

    astListener->m_onEnterElseClause = zen_BinaryEntityGenerator_onEnterElseClause;
    astListener->m_onExitElseClause = zen_BinaryEntityGenerator_onExitElseClause;

    astListener->m_onEnterIterativeStatement = zen_BinaryEntityGenerator_onEnterIterativeStatement;
    astListener->m_onExitIterativeStatement = zen_BinaryEntityGenerator_onExitIterativeStatement;

    astListener->m_onEnterLabelClause = zen_BinaryEntityGenerator_onEnterLabel;
    astListener->m_onExitLabelClause = zen_BinaryEntityGenerator_onExitLabel;

    astListener->m_onEnterWhileStatement = zen_BinaryEntityGenerator_onEnterWhileStatement;
    astListener->m_onExitWhileStatement = zen_BinaryEntityGenerator_onExitWhileStatement;

    astListener->m_onEnterForStatement = zen_BinaryEntityGenerator_onEnterForStatement;
    astListener->m_onExitForStatement = zen_BinaryEntityGenerator_onExitForStatement;

    astListener->m_onEnterForParameters = zen_BinaryEntityGenerator_onEnterForParameters;
    astListener->m_onExitForParameters = zen_BinaryEntityGenerator_onExitForParameters;

    astListener->m_onEnterTryStatement = zen_BinaryEntityGenerator_onEnterTryStatement;
    astListener->m_onExitTryStatement = zen_BinaryEntityGenerator_onExitTryStatement;

    astListener->m_onEnterTryClause = zen_BinaryEntityGenerator_onEnterTryClause;
    astListener->m_onExitTryClause = zen_BinaryEntityGenerator_onExitTryClause;

    astListener->m_onEnterCatchClause = zen_BinaryEntityGenerator_onEnterCatchClause;
    astListener->m_onExitCatchClause = zen_BinaryEntityGenerator_onExitCatchClause;

    astListener->m_onEnterCatchFilter = zen_BinaryEntityGenerator_onEnterCatchFilter;
    astListener->m_onExitCatchFilter = zen_BinaryEntityGenerator_onExitCatchFilter;

    astListener->m_onEnterFinallyClause = zen_BinaryEntityGenerator_onEnterFinallyClause;
    astListener->m_onExitFinallyClause = zen_BinaryEntityGenerator_onExitFinallyClause;

    astListener->m_onEnterSynchronizeStatement = zen_BinaryEntityGenerator_onEnterSynchronizeStatement;
    astListener->m_onExitSynchronizeStatement = zen_BinaryEntityGenerator_onExitSynchronizeStatement;

    astListener->m_onEnterWithStatement = zen_BinaryEntityGenerator_onEnterWithStatement;
    astListener->m_onExitWithStatement = zen_BinaryEntityGenerator_onExitWithStatement;

    astListener->m_onEnterClassDeclaration = zen_BinaryEntityGenerator_onEnterClassDeclaration;
    astListener->m_onExitClassDeclaration = zen_BinaryEntityGenerator_onExitClassDeclaration;

    astListener->m_onEnterClassExtendsClause = zen_BinaryEntityGenerator_onEnterClassExtendsClause;
    astListener->m_onExitClassExtendsClause = zen_BinaryEntityGenerator_onExitClassExtendsClause;

    astListener->m_onEnterClassSuite = zen_BinaryEntityGenerator_onEnterClassSuite;
    astListener->m_onExitClassSuite = zen_BinaryEntityGenerator_onExitClassSuite;

    astListener->m_onEnterClassMember = zen_BinaryEntityGenerator_onEnterClassMember;
    astListener->m_onExitClassMember = zen_BinaryEntityGenerator_onExitClassMember;

    // astListener->m_onEnterConstructorDeclaration = zen_BinaryEntityGenerator_onEnterConstructorDeclaration;
    // astListener->m_onExitConstructorDeclaration = zen_BinaryEntityGenerator_onExitConstructorDeclaration;

    astListener->m_onEnterEnumerationDeclaration = zen_BinaryEntityGenerator_onEnterEnumerationDeclaration;
    astListener->m_onExitEnumerationDeclaration = zen_BinaryEntityGenerator_onExitEnumerationDeclaration;

    astListener->m_onEnterEnumerationBaseClause = zen_BinaryEntityGenerator_onEnterEnumrationBaseClass;
    astListener->m_onExitEnumerationBaseClause = zen_BinaryEntityGenerator_onExitEnumerationBaseClause;

    astListener->m_onEnterEnumerationSuite = zen_BinaryEntityGenerator_onEnterEnumerationSuite;
    astListener->m_onExitEnumerationSuite = zen_BinaryEntityGenerator_onExitEnumerationSuite;

    astListener->m_onEnterEnumerate = zen_BinaryEntityGenerator_onEnterEnumerate;
    astListener->m_onExitEnumerate = zen_BinaryEntityGenerator_onExitEnumerate;

    astListener->m_onEnterExpressions = zen_BinaryEntityGenerator_onEnterExpressions;
    astListener->m_onExitExpressions = zen_BinaryEntityGenerator_onExitExpressions;

    astListener->m_onEnterExpression = zen_BinaryEntityGenerator_onEnterExpression;
    astListener->m_onExitExpression = zen_BinaryEntityGenerator_onExitExpression;

    astListener->m_onEnterAssignmentExpression = zen_BinaryEntityGenerator_onEnterAssignmentExpression;
    astListener->m_onExitAssignmentExpression = zen_BinaryEntityGenerator_onExitAssignmentExpression;

    astListener->m_onEnterConditionalExpression = zen_BinaryEntityGenerator_onEnterConditionalExpression;
    astListener->m_onExitConditionalExpression = zen_BinaryEntityGenerator_onExitConditionalExpression;

    astListener->m_onEnterLogicalOrExpression = zen_BinaryEntityGenerator_onEnterLogicalOrExpression;
    astListener->m_onExitLogicalOrExpression = zen_BinaryEntityGenerator_onExitLogicalOrExpression;

    astListener->m_onEnterLogicalAndExpression = zen_BinaryEntityGenerator_onEnterLogicalAndExpression;
    astListener->m_onExitLogicalAndExpression = zen_BinaryEntityGenerator_onExitLogicalAndExpression;

    astListener->m_onEnterInclusiveOrExpression = zen_BinaryEntityGenerator_onEnterInclusiveOrExpression;
    astListener->m_onExitInclusiveOrExpression = zen_BinaryEntityGenerator_onExitInclusiveOrExpression;

    astListener->m_onEnterExclusiveOrExpression = zen_BinaryEntityGenerator_onEnterExclusiveOrExpression;
    astListener->m_onExitExclusiveOrExpression = zen_BinaryEntityGenerator_onExitExclusiveOrExpression;

    astListener->m_onEnterAndExpression = zen_BinaryEntityGenerator_onEnterAndExpression;
    astListener->m_onExitAndExpression = zen_BinaryEntityGenerator_onExitAndExpression;

    astListener->m_onEnterEqualityExpression = zen_BinaryEntityGenerator_onEnterEqualityExpression;
    astListener->m_onExitEqualityExpression = zen_BinaryEntityGenerator_onExitEqualityExpression;

    astListener->m_onEnterRelationalExpression = zen_BinaryEntityGenerator_onEnterRelationalExpression;
    astListener->m_onExitRelationalExpression = zen_BinaryEntityGenerator_onExitRelationalExpression;

    astListener->m_onEnterShiftExpression = zen_BinaryEntityGenerator_onEnterShiftExpression;
    astListener->m_onExitShiftExpression = zen_BinaryEntityGenerator_onExitShiftExpression;

    astListener->m_onEnterAdditiveExpression = zen_BinaryEntityGenerator_onEnterAdditiveExpression;
    astListener->m_onExitAdditiveExpression = zen_BinaryEntityGenerator_onExitAdditiveExpression;

    astListener->m_onEnterMultiplicativeExpression = zen_BinaryEntityGenerator_onEnterMultiplicativeExpression;
    astListener->m_onExitMultiplicativeExpression = zen_BinaryEntityGenerator_onExitMultiplicativeExpression;

    astListener->m_onEnterUnaryExpression = zen_BinaryEntityGenerator_onEnterUnaryExpression;
    astListener->m_onExitUnaryExpression = zen_BinaryEntityGenerator_onExitUnaryExpression;

    astListener->m_onEnterPostfixExpression = zen_BinaryEntityGenerator_onEnterPostfixExpression;
    astListener->m_onExitPostfixExpression = zen_BinaryEntityGenerator_onExitPostfixExpression;

    astListener->m_onEnterSubscript = zen_BinaryEntityGenerator_onEnterSubscript;
    astListener->m_onExitSubscript = zen_BinaryEntityGenerator_onExitSubscript;

    astListener->m_onEnterFunctionArguments = zen_BinaryEntityGenerator_onEnterFunctionArguments;
    astListener->m_onExitFunctionArguments = zen_BinaryEntityGenerator_onExitFunctionArguments;

    astListener->m_onEnterMemberAccess = zen_BinaryEntityGenerator_onEnterMemberAccess;
    astListener->m_onExitMemberAccess = zen_BinaryEntityGenerator_onExitMemberAccess;

    astListener->m_onEnterPostfixOperator = zen_BinaryEntityGenerator_onEnterPostfixOperator;
    astListener->m_onExitPostfixOperator = zen_BinaryEntityGenerator_onExitPostfixOperator;

    astListener->m_onEnterPrimaryExpression = zen_BinaryEntityGenerator_onEnterPrimaryExpression;
    astListener->m_onExitPrimaryExpression = zen_BinaryEntityGenerator_onExitPrimaryExpression;

    astListener->m_onEnterMapExpression = zen_BinaryEntityGenerator_onEnterMapExpression;
    astListener->m_onExitMapExpression = zen_BinaryEntityGenerator_onExitMapExpression;

    astListener->m_onEnterMapEntries = zen_BinaryEntityGenerator_onEnterMapEntries;
    astListener->m_onExitMapEntries = zen_BinaryEntityGenerator_onExitMapEntries;

    astListener->m_onEnterMapEntry = zen_BinaryEntityGenerator_onEnterMapEntry;
    astListener->m_onExitMapEntry = zen_BinaryEntityGenerator_onExitMapEntry;

    astListener->m_onEnterListExpression = zen_BinaryEntityGenerator_onEnterListExpression;
    astListener->m_onExitListExpression = zen_BinaryEntityGenerator_onExitListExpression;

    astListener->m_onEnterNewExpression = zen_BinaryEntityGenerator_onEnterNewExpression;
    astListener->m_onExitNewExpression = zen_BinaryEntityGenerator_onExitNewExpression;

    return generator;
}

// Constructor

void zen_BinaryEntityGenerator_delete(zen_BinaryEntityGenerator_t* generator) {
    jtk_Assert_assertObject(generator, "The specified generator is null.");

    zen_BinaryEntityBuilder_delete(generator->m_instructions);

    int32_t fieldCount = jtk_ArrayList_getSize(generator->m_fields);
    int32_t fieldIndex;
    for (fieldIndex = 0; fieldIndex < fieldCount; fieldIndex++) {
        zen_FieldEntity_t* fieldEntity = (zen_FieldEntity_t*)jtk_ArrayList_getValue(
            generator->m_fields, fieldIndex);
        zen_FieldEntity_delete(fieldEntity);
    }
    jtk_ArrayList_delete(generator->m_fields);

    int32_t functionCount = jtk_ArrayList_getSize(generator->m_functions);
    int32_t functionIndex;
    for (functionIndex = 0; functionIndex < functionCount; functionIndex++) {
        zen_FunctionEntity_t* functionEntity = (zen_FunctionEntity_t*)jtk_ArrayList_getValue(
            generator->m_functions, functionIndex);
        zen_FunctionEntity_delete(functionEntity);
    }
    jtk_ArrayList_delete(generator->m_functions);

    /* Destroy the constant pool builder. It takes care of destroying the
     * constant pool entries.
     */
    zen_ConstantPoolBuilder_delete(generator->m_constantPoolBuilder);
    zen_Memory_deallocate(generator->m_entityFile);

    zen_BinaryEntityBuilder_delete(generator->m_builder);
    zen_ASTListener_delete(generator->m_astListener);
    jtk_Memory_deallocate(generator);
}

// Generate

void zen_BinaryEntityGenerator_generate(zen_BinaryEntityGenerator_t* generator) {
    jtk_Assert_assertObject(generator, "The specified generator is null.");

    zen_ASTWalker_walk(generator->m_astListener, generator->m_compilationUnit);
    // zen_BinaryEntityGenerator_writeBytes(generator, generator->m_outputStream);
    zen_BinaryEntityGenerator_writeEntity(generator);
}

// Reset

void zen_BinaryEntityGenerator_reset(zen_BinaryEntityGenerator_t* generator,
    zen_SymbolTable_t* symbolTable, zen_ASTAnnotations_t* scopes,
    zen_ASTNode_t* compilationUnit, jtk_OutputStream_t* outputStream) {
    jtk_Assert_assertObject(generator, "The specified generator is null.");

    // zen_BinaryEntityBuilder_clear(generator->m_builder);
    generator->m_symbolTable = symbolTable;
    generator->m_scopes = scopes;
    generator->m_compilationUnit = compilationUnit;
    generator->m_outputStream = outputStream;
}

// Event Handlers

void zen_BinaryEntityGenerator_onVisitErrorNode(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {
    fprintf(stderr, "[warning] Cannot generate binary entity on erroneous AST.");
}

void zen_BinaryEntityGenerator_onVisitTerminal(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onEnterEveryRule(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitEveryRule(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

/* compilationUnit */

void zen_BinaryEntityGenerator_onEnterCompilationUnit(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {
    jtk_Assert_assertObject(astListener, "The specified AST listener is null.");
    jtk_Assert_assertObject(node, "The specified AST node is null.");

    /* Retrieve the generator associated with the AST listener. */
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    /* Retrieve the scope associated with the current AST node. */
    zen_Scope_t* scope = zen_ASTAnnotations_get(generator->m_scopes, node);
    /* Activate the scope associated with the current AST node. */
    zen_SymbolTable_setCurrentScope(generator->m_symbolTable, scope);
}

void zen_BinaryEntityGenerator_onExitCompilationUnit(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {
    /* Retrieve the generator associated with the AST listener. */
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;

    /* Set the major version of the target binary entity format. */
    generator->m_entityFile->m_version.m_majorVersion = 0x0000;
    /* Set the minor version of the target binary entity format. */
    generator->m_entityFile->m_version.m_minorVersion = 0x0001;

    /* Invalidate the current scope in the symbol table. */
    zen_SymbolTable_invalidateCurrentScope(generator->m_symbolTable);

    /* Push a data channel, where the bytes generated will be written. */
    zen_BinaryEntityBuilder_pushChannel(generator->m_builder);
}

#include <stdio.h>

void zen_BinaryEntityGenerator_writeEntity(zen_BinaryEntityGenerator_t* generator) {
    /* Write magic number, major version, and minor version on the main channel. */
    zen_BinaryEntityBuilder_writeMagicNumber(generator->m_builder);
    /* Log the magic number written. */
    printf("[log] Encoding stream with magic number 0xFEB72000.\n");

    /* Write the major version of the binary entity file format the stream is encoded in. */
    zen_BinaryEntityBuilder_writeMajorVersion(generator->m_builder,
        generator->m_entityFile->m_version.m_majorVersion);

    /* Write the minor version of the binary entity file format the stream is encoded in. */
    zen_BinaryEntityBuilder_writeMinorVersion(generator->m_builder,
        generator->m_entityFile->m_version.m_minorVersion);

    /* Log the version of the target binary entity file format. */
    printf("[log] The target binary entity format version is v%d.%d\n",
        generator->m_entityFile->m_version.m_majorVersion,
        generator->m_entityFile->m_version.m_minorVersion);

    /* Write additional flags on how the binary entity file should be loaded. */
    zen_BinaryEntityBuilder_writeStreamFlags(generator->m_builder,
        generator->m_entityFile->m_flags);

    /* Log the flags that determine how the binary entity should be treated. */
    printf("[log] Applying flags 0x%X\n", generator->m_entityFile->m_flags);

    /* At this point, all the constant pool entries required by the binary entity
     * file should be available to the constant pool builder. The constant pool
     * can now be built.
     */
    int32_t entryCount = zen_ConstantPoolBuilder_countEntries(generator->m_constantPoolBuilder);
    zen_BinaryEntityBuilder_writeConstantPoolHeader(generator->m_builder, entryCount);
    int32_t i;
    for (i = 0; i < entryCount; i++) {
        zen_ConstantPoolEntry_t* entry = zen_ConstantPoolBuilder_getEntry(generator->m_constantPoolBuilder, i);
        zen_BinaryEntityBuilder_writeConstantPoolEntry(generator->m_builder, entry);
    }

    /* Log the constant pool details, including the number of entries and the trends
     * seen among the entries.
     */
    printf("[log] Constant pool size is %d.\n", entryCount);

    /* Retrieve the entity to write. */
    zen_Entity_t* entity = &generator->m_entityFile->m_entity;

    /* Write the entity header. */
    zen_BinaryEntityBuilder_writeEntityHeader(generator->m_builder, entity->m_type,
        entity->m_flags, entity->m_reference);
    /* Log the entity header information written, including type, flags, and reference. */
    printf("[debug] Entity header includes type = %d, flags = 0x%X, and reference = %d.\n",
        entity->m_type, entity->m_flags, entity->m_reference);

    /* Write the superclasses. */
    zen_BinaryEntityBuilder_writeSuperclasses(generator->m_builder, entity->m_superclassCount,
        entity->m_superclasses);
    /* Log the superclass indexes that the entity inherits. */
    printf("[debug] Entity inherits %d superclasses.\n", entity->m_superclassCount);

    /* Write the attribute count. */
    zen_BinaryEntityBuilder_writeAttributeCount(generator->m_builder, entity->m_attributeTable.m_size);
    /* Log the attribute count. */
    printf("[debug] Entity has %d attributes.\n", entity->m_attributeTable.m_size);

    // TODO: Write the attribute

    /* Retrieve the field count. */
    int32_t fieldCount = jtk_ArrayList_getSize(generator->m_fields);
    /* Write the field count. */
    zen_BinaryEntityBuilder_writeFieldCount(generator->m_builder, fieldCount);
    /* Log the field count. */
    printf("[debug] Entity has %d fields.\n", fieldCount);

    int32_t fieldIndex;
    for (fieldIndex = 0; fieldIndex < fieldCount; fieldIndex++) {
        /* Retrieve the next field to write. */
        zen_FieldEntity_t* fieldEntity = (zen_FieldEntity_t*)jtk_ArrayList_getValue(
            generator->m_fields, fieldIndex);

        /* Write the field to the data channel. */
        zen_BinaryEntityBuilder_writeField(generator->m_builder, fieldEntity->m_flags,
            fieldEntity->m_nameIndex, fieldEntity->m_descriptorIndex);

        /* Log the details of the field. */
        printf("[debug] A field was written with the features (flags = 0x%X, nameIndex = %d, descriptorIndex = %d).\n",
            fieldEntity->m_flags, fieldEntity->m_nameIndex, fieldEntity->m_descriptorIndex);
    }

    /* Retrieve the function count. */
    int32_t functionCount = jtk_ArrayList_getSize(generator->m_functions);
    /* Write the function count. */
    zen_BinaryEntityBuilder_writeFunctionCount(generator->m_builder, functionCount);
    /* Log the function count. */
    printf("[debug] Entity has %d functions.\n", functionCount);

    int32_t functionIndex;
    for (functionIndex = 0; functionIndex < functionCount; functionIndex++) {
        /* Retrieve the next function to write. */
        zen_FunctionEntity_t* functionEntity = (zen_FunctionEntity_t*)jtk_ArrayList_getValue(
            generator->m_functions, functionIndex);

        /* Write the function to the data channel. */
        zen_BinaryEntityBuilder_writeFunction(generator->m_builder,
            functionEntity->m_nameIndex, functionEntity->m_descriptorIndex,
            functionEntity->m_flags);

        /* Log the details of the function. */
        printf("[debug] A function was written with the features (flags = 0x%X, nameIndex = %d, descriptorIndex = %d).\n",
            functionEntity->m_flags, functionEntity->m_nameIndex, functionEntity->m_descriptorIndex);

        /* Retrieve the attribute table for the current function entity. */
        zen_AttributeTable_t* attributeTable = &functionEntity->m_attributeTable;

        /* Write the total number of attributes. */
        zen_BinaryEntityBuilder_writeAttributeCount(generator->m_builder, attributeTable->m_size);

        /* Log the total number of attributes declared in the attribute table. */
        printf("[debug] The function has %d attributes.\n", attributeTable->m_size);

        int32_t attributeIndex;
        for (attributeIndex = 0; attributeIndex < attributeTable->m_size; attributeIndex++) {
            zen_Attribute_t* attribute = attributeTable->m_attributes[attributeIndex];

            /* Retrieve the name of the current attribute. */
            zen_ConstantPoolUtf8_t* name = zen_ConstantPoolBuilder_getUtf8Entry(
                generator->m_constantPoolBuilder, attribute->m_nameIndex);

            /* If the current attribute is an instruction attribute, extract
             * the instruction attribute and write to the data channel.
             */
            if (jtk_CString_equals(name->m_bytes, name->m_length,
                ZEN_PREDEFINED_ATTRIBUTE_INSTRUCTION, ZEN_PREDEFINED_ATTRIBUTE_INSTRUCTION_SIZE)) {
                /* Cast to zen_InstructionAttribute_t to extract further information. */
                zen_InstructionAttribute_t* instructionAttribute =
                    (zen_InstructionAttribute_t*)attribute;

                /* Write the instruction attribute for the current function. */
                zen_BinaryEntityBuilder_writeInstructionAttribute(
                    generator->m_builder,
                    instructionAttribute->m_nameIndex,
                    instructionAttribute->m_length,
                    instructionAttribute->m_maxStackSize,
                    instructionAttribute->m_localVariableCount,
                    instructionAttribute->m_instructionLength,
                    instructionAttribute->m_instructions);

                /* Log the details of the instruction attribute. */
                printf("[debug] The function has an instruction attribute with the features "
                       "(nameIndex = %d, length = %d, maxStackSize = %d, localVariableCount = %d, "
                       "instructionLength = %d)\n",
                        instructionAttribute->m_nameIndex,
                        instructionAttribute->m_length,
                        instructionAttribute->m_maxStackSize,
                        instructionAttribute->m_localVariableCount,
                        instructionAttribute->m_instructionLength);

                // TODO: Write the exception table.
            }
        }
    }

    FILE* fp = fopen("output.feb", "w+");
    if (fp != NULL) {
        zen_DataChannel_t* channel = jtk_ArrayList_getValue(generator->m_builder->m_channels, 0);
        fwrite(channel->m_bytes, channel->m_index, 1, fp);
        fclose(fp);
    }
}

// importDeclaration

void zen_BinaryEntityGenerator_onEnterImportDeclaration(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitImportDeclaration(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// annotatedComponentDeclaration

void zen_BinaryEntityGenerator_onEnterAnnotatedComponentDeclaration(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitAnnotatedComponentDeclaration(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// annotations

void zen_BinaryEntityGenerator_onEnterAnnotations(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitAnnotations(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// annotation

void zen_BinaryEntityGenerator_onEnterAnnotation(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitAnnotation(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// annotationType

void zen_BinaryEntityGenerator_onEnterAnnotationType(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitAnnotationType(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// annotationAttribute

void zen_BinaryEntityGenerator_onEnterAnnotationAttribute(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitAnnotationAttribute(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// componentDeclaration

void zen_BinaryEntityGenerator_onEnterComponentDeclaration(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitComponentDeclaration(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// functionDeclaration

void zen_BinaryEntityGenerator_onEnterFunctionDeclaration(
    zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
    jtk_Assert_assertObject(astListener, "The specified AST listener is null.");
    jtk_Assert_assertObject(node, "The specified AST node is null.");

    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_FunctionDeclarationContext_t* context = (zen_FunctionDeclarationContext_t*)node->m_context;

    zen_Scope_t* scope = zen_ASTAnnotations_get(generator->m_scopes, node);
    zen_SymbolTable_setCurrentScope(generator->m_symbolTable, scope);

    zen_BinaryEntityBuilder_pushChannel(generator->m_instructions);

    // TODO: Remove the following statement. Make sure that the instruction
    // length is never zero.
    zen_BinaryEntityBuilder_emitNop(generator->m_instructions);
}

/* The format of a function descriptor is shown below:
 * descriptor
 * :    returnType ':' parameters
 * ;
 *
 * returnType
 * :    type
 * ;
 *
 * parameters
 * :    type+
 * ;
 *
 * type
 * :    'v'
 * |    properType
 * ;
 *
 * properType
 * :    'z'
 * |    'b'
 * |    's'
 * |    'i'
 * |    'l'
 * |    'f'
 * |    'd'
 * |    className
 * |    arrayType
 * ;
 *
 * arrayType
 * :    '@'+ properType
 * ;
 *
 * className
 * :    '(' IDENTIFIER ('/' IDENTIFIER)* ')'
 * ;
 *
 * Here is an example function.
 * function printf(format, ...arguments)
 *
 * The function descriptor for this function is shown below.
 * (zen/core/Object):(zen/core/Object)@(zen/core/Object)
 */

jtk_String_t* zen_BinaryEntityGenerator_getDescriptor(zen_BinaryEntityGenerator_t* generator,
    zen_ASTNode_t* functionParameters) {
    zen_FunctionParametersContext_t* functionParametersContext =
        (zen_FunctionParametersContext_t*)functionParameters->m_context;


    jtk_StringBuilder_t* builder = jtk_StringBuilder_new();
    jtk_StringBuilder_append_z(builder, "(zen/core/Object):");

    int32_t fixedParameterCount = jtk_ArrayList_getSize(functionParametersContext->m_fixedParameters);
    int32_t i;
    for (i = 0; i < fixedParameterCount; i++) {
        jtk_StringBuilder_append_z(builder, "(zen/core/Object)");
    }

    if (functionParametersContext->m_variableParameter != NULL) {
        jtk_StringBuilder_append_z(builder, "@(zen/core/Object)");
    }

    if ((fixedParameterCount == 0) &&
        (functionParametersContext->m_variableParameter == NULL)) {
        jtk_StringBuilder_append_c(builder, 'v');
    }

    jtk_String_t* result = jtk_StringBuilder_toString(builder);

    /* Destroy the string builder. */
    jtk_StringBuilder_delete(builder);

    return result;
}

zen_InstructionAttribute_t* zen_BinaryEntityGenerator_makeInstructionAttribute(
    zen_BinaryEntityGenerator_t* generator) {

    /* Retrieve the data channel on which the instructions of the
     * function/initializer were written.
     */
    zen_DataChannel_t* channel = (zen_DataChannel_t*)jtk_ArrayList_getValue(
        generator->m_instructions->m_channels, 0);
    /* Retrieve the bytes that were written on the data channel. */
    uint8_t* instructionBytes = zen_DataChannel_getBytes(channel);


    /* Retrieve a valid index into the constant pool where an UTF-8 entry
     * represents "vm/primary/Instruction".
     */
    uint16_t attributeNameIndex = zen_ConstantPoolBuilder_getUtf8EntryIndexEx(
        generator->m_constantPoolBuilder, ZEN_PREDEFINED_ATTRIBUTE_INSTRUCTION,
        ZEN_PREDEFINED_ATTRIBUTE_INSTRUCTION_SIZE);
    /* Load the maximum stack size. */
    uint16_t maxStackSize = generator->m_maxStackSize;
    /* Load the number of local variables. */
    uint16_t localVariableCount = generator->m_localVariableCount;
    /* Retrieve the length of the instructions. */
    uint32_t instructionLength = zen_DataChannel_getSize(channel);
    /* The instructions of the function. */
    uint8_t* instructions = jtk_Arrays_clone_b(instructionBytes, instructionLength);
    /* The total number of exception handler sistes within the function. */
    uint16_t exceptionHandlerSiteCount = 0;
    /* Calculate the length of the attribute immediately after the m_length
     * field.
     */
    uint32_t attributeLength =
        2 + // maxStackSize occupies two bytes.
        2 + // localVariableCount occupies two bytes.
        4 + // instructionLength occupies four bytes.
        instructionLength + // The total number of bytes the instructions occupy.
        2 + // m_exceptionTable.m_size occupies two bytes.
        exceptionHandlerSiteCount * ( // Each exception handler site entry occupies the following width.
            2 + // startIndex occupies two bytes.
            2 + // stopIndex occupies two bytes.
            2 + // handlerIndex occupies two bytes.
            2); // exceptionClassIndex occupies two bytes.

    zen_InstructionAttribute_t* instructionAttribute = zen_InstructionAttribute_new(
        attributeNameIndex,
        attributeLength,
        maxStackSize,
        localVariableCount,
        instructionLength,
        instructions);
    return instructionAttribute;
}

// TODO: Somebody has to destroy the instruction attribute that was allocated here.
void zen_BinaryEntityGenerator_onExitFunctionDeclaration(
    zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
    jtk_Assert_assertObject(astListener, "The specified AST listener is null.");
    jtk_Assert_assertObject(node, "The specified AST node is null.");

    /* Retrieve the generator associated with the AST listener. */
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;

    /* Retrieve the context associated with the AST node. */
    zen_FunctionDeclarationContext_t* context = (zen_FunctionDeclarationContext_t*)node->m_context;

    zen_ASTNode_t* identifier = context->m_identifier;
    zen_Token_t* identifierToken = (zen_Token_t*)identifier->m_context;

    jtk_String_t* descriptor = zen_BinaryEntityGenerator_getDescriptor(generator, context->m_functionParameters);

    uint16_t flags = 0;
    uint16_t nameIndex = zen_ConstantPoolBuilder_getUtf8EntryIndexEx(
        generator->m_constantPoolBuilder, identifierToken->m_text,
        identifierToken->m_length);
    uint16_t descriptorIndex = zen_ConstantPoolBuilder_getUtf8EntryIndex(
        generator->m_constantPoolBuilder, descriptor);

    /* Destroy the descriptor. A constant pool reference to the descriptor was
     * acquired.
     */
    jtk_String_delete(descriptor);

    zen_InstructionAttribute_t* instructionAttribute =
        zen_BinaryEntityGenerator_makeInstructionAttribute(generator);

    zen_FunctionEntity_t* functionEntity = zen_FunctionEntity_new(flags,
        nameIndex, descriptorIndex);
    zen_AttributeTable_t* attributeTable = &functionEntity->m_attributeTable;

    attributeTable->m_size = 1;
    attributeTable->m_attributes = zen_Memory_allocate(zen_Attribute_t*, 1);
    attributeTable->m_attributes[0] = (zen_Attribute_t*)instructionAttribute;

    /* Add the function entity to the list of functions. */
    jtk_ArrayList_add(generator->m_functions, functionEntity);

    zen_SymbolTable_invalidateCurrentScope(generator->m_symbolTable);
}

// functionParameters

void zen_BinaryEntityGenerator_onEnterFunctionParameters(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitFunctionParameters(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// functionBody

void zen_BinaryEntityGenerator_onEnterFunctionBody(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitFunctionBody(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// statementSuite

void zen_BinaryEntityGenerator_onEnterStatementSuite(
    zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
    jtk_Assert_assertObject(astListener, "The specified AST listener is null.");
    jtk_Assert_assertObject(node, "The specified AST node is null.");

    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;

    zen_Scope_t* scope = zen_ASTAnnotations_get(generator->m_scopes, node);
    zen_SymbolTable_setCurrentScope(generator->m_symbolTable, scope);
}

void zen_BinaryEntityGenerator_onExitStatementSuite(
    zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
    jtk_Assert_assertObject(astListener, "The specified AST listener is null.");
    jtk_Assert_assertObject(node, "The specified AST node is null.");

    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;

    zen_SymbolTable_invalidateCurrentScope(generator->m_symbolTable);
}

// simpleStatement

void zen_BinaryEntityGenerator_onEnterSimpleStatement(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitSimpleStatement(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// statement

void zen_BinaryEntityGenerator_onEnterStatement(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitStatement(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// emptyStatement

void zen_BinaryEntityGenerator_onEnterEmptyStatement(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitEmptyStatement(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// variableDeclaration

void zen_BinaryEntityGenerator_onEnterVariableDeclaration(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitVariableDeclaration(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// variableDeclarator

void zen_BinaryEntityGenerator_onEnterVariableDeclarator(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitVariableDeclarator(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
    /* Retrieve the generator associated with the AST listener. */
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;

    /* Retrieve the context associated with the AST node. */
    zen_VariableDeclaratorContext_t* context = (zen_VariableDeclaratorContext_t*)node->m_context;

    /* Retrieve the current scope from the symbol table. */
    zen_Scope_t* currentScope = zen_SymbolTable_getCurrentScope(generator->m_symbolTable);

    /* If a class scope encloses the variable being declared, we are processing
     * a class member declaration.
     */
    if (zen_Scope_isClassScope(currentScope)) {
        /* Retrieve the identifier AST node. */
        zen_ASTNode_t* identifier = context->m_identifier;
        /* Retrieve the identifier token. */
        zen_Token_t* identifierToken = (zen_Token_t*)identifier->m_context;

        uint16_t flags = 0;

        /* Retrieve the constant pool index for the variable name. */
        uint16_t nameIndex = zen_ConstantPoolBuilder_getUtf8EntryIndexEx(
            generator->m_constantPoolBuilder,
            identifierToken->m_text, identifierToken->m_length);

        uint16_t descriptorIndex = zen_ConstantPoolBuilder_getUtf8EntryIndexEx(
            generator->m_constantPoolBuilder,
            "com/todo/Class", 14);

        /* Create an instance of the field entity that represents the variable
         * declared.
         */
        zen_FieldEntity_t* fieldEntity = zen_FieldEntity_new(flags, nameIndex, descriptorIndex);

        /* Add the field entity to the list of fields. */
        jtk_ArrayList_add(generator->m_fields, fieldEntity);
    }
}

// constantDeclaration

void zen_BinaryEntityGenerator_onEnterConstantDeclaration(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitConstantDeclaration(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// constantDeclarator

void zen_BinaryEntityGenerator_onEnterConstantDeclarator(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitConstantDeclarator(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// assertStatement

void zen_BinaryEntityGenerator_onEnterAssertStatement(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitAssertStatement(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onEnterBreakStatement(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// breakStatement

void zen_BinaryEntityGenerator_onExitBreakStatement(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// continueStatement

void zen_BinaryEntityGenerator_onEnterContinueStatement(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitContinueStatement(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// returnStatement

void zen_BinaryEntityGenerator_onEnterReturnStatement(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitReturnStatement(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// throwStatement

void zen_BinaryEntityGenerator_onEnterThrowStatement(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitThrowStatement(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// compoundStatement

void zen_BinaryEntityGenerator_onEnterCompoundStatement(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitCompoundStatement(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// ifStatement

void zen_BinaryEntityGenerator_onEnterIfStatement(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitIfStatement(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// ifClause

void zen_BinaryEntityGenerator_onEnterIfClause(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitIfClause(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// elseIfClause

void zen_BinaryEntityGenerator_onEnterElseIfClause(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitElseIfClause(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// elseClause

void zen_BinaryEntityGenerator_onEnterElseClause(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitElseClause(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// iterativeStatement

void zen_BinaryEntityGenerator_onEnterIterativeStatement(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitIterativeStatement(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// label

void zen_BinaryEntityGenerator_onEnterLabel(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitLabel(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// whileStatement

void zen_BinaryEntityGenerator_onEnterWhileStatement(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    // zen_BinaryEntityBuilder_pushChannel(generator->m_builder);
}

void zen_BinaryEntityGenerator_onExitWhileStatement(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {/*
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;

    zen_ByteCodeChannel_t* statementSuiteChannel = zen_ChannelManager_peek(generator->m_channelManager);
    uint32_t index = zen_ByteCodeChannel_getSize(statementSuiteChannel);
    zen_BinaryEntityGenerator_emitJump(generator, index);
    zen_ChannelManager_merge(generator->m_channelManager);*/
}

// forStatement

void zen_BinaryEntityGenerator_onEnterForStatement(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitForStatement(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// forParameters

void zen_BinaryEntityGenerator_onEnterForParameters(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitForParameters(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// tryStatement

void zen_BinaryEntityGenerator_onEnterTryStatement(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitTryStatement(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// tryClause

void zen_BinaryEntityGenerator_onEnterTryClause(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitTryClause(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// catchClause

void zen_BinaryEntityGenerator_onEnterCatchClause(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitCatchClause(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// catchFilter

void zen_BinaryEntityGenerator_onEnterCatchFilter(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitCatchFilter(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// finallyClause

void zen_BinaryEntityGenerator_onEnterFinallyClause(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitFinallyClause(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// synchronizeStatement

void zen_BinaryEntityGenerator_onEnterSynchronizeStatement(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitSynchronizeStatement(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// withStatement

void zen_BinaryEntityGenerator_onEnterWithStatement(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitWithStatement(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// classDeclaration

// BUG: Looks like we are not setting the current scope in this function.
void zen_BinaryEntityGenerator_onEnterClassDeclaration(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {
    jtk_Assert_assertObject(astListener, "The specified AST listener is null.");
    jtk_Assert_assertObject(node, "The specified AST node is null.");

    /* Retrieve the generator associated with the AST listener. */
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;

    /* Retrieve the current scope from the symbol table. At this point, it is
     * the enclosing scope.
     */
    zen_Scope_t* parentScope = zen_SymbolTable_getCurrentScope(generator->m_symbolTable);

    /* Retrieve the context of the AST node. */
    zen_ClassDeclarationContext_t* context =
        (zen_ClassDeclarationContext_t*)node->m_context;

    /* Retrieve the scope associated with the class being declared. */
    zen_Scope_t* scope = zen_ASTAnnotations_get(generator->m_scopes, node);

    /* Update the current scope in the symbol table. */
    zen_SymbolTable_setCurrentScope(generator->m_symbolTable, scope);

    zen_ASTNode_t* identifier = context->m_identifier;
    zen_Token_t* identifierToken = (zen_Token_t*)identifier->m_context;
    jtk_String_t* reference = NULL;
    if (generator->m_package != NULL) {
        reference = jtk_String_newFromJoinEx(generator->m_package->m_value,
            generator->m_package->m_size, identifierToken->m_text, identifierToken->m_length);
    }
    else {
        reference = jtk_String_newEx(identifierToken->m_text, identifierToken->m_length);
    }

    uint16_t flags = 0;
    uint16_t referenceIndex =
        zen_ConstantPoolBuilder_getUtf8EntryIndex(generator->m_constantPoolBuilder, reference);
    uint16_t* superclassIndexes = NULL;
    uint16_t superclassCount = 0;

    /* At this point, the reference is not required anymore. We have the index into
     * the constant pool which represents it. Therefore, destroy it.
     */
    jtk_String_delete(reference);

    if (context->m_classExtendsClause != NULL) {
        /* Retrieve the extends clause context to extract information about the
         * superclasses.
         */
        zen_ClassExtendsClauseContext_t* extendsClauseContext =
            (zen_ClassExtendsClauseContext_t*)context->m_classExtendsClause->m_context;

        /* Calculate the total number of superclasses. */
        superclassCount = jtk_ArrayList_getSize(extendsClauseContext->m_typeNames);
        /* Allocate an array to store the constant pool indexes to the superclass
         * references.
         */
        superclassIndexes = jtk_Memory_allocate(uint16_t, superclassCount);

        /* The entity generator does not ensure the validity of the identifiers.
         * It is the responsibility of the resolution phase.
         */
        int32_t index;
        for (index = 0; index < superclassCount; index++) {
            zen_ASTNode_t* typeNameNode = (zen_ASTNode_t*)jtk_ArrayList_getValue(extendsClauseContext->m_typeNames, index);
            zen_TypeNameContext_t* typeNameContext = (zen_TypeNameContext_t*)typeNameNode->m_context;

            // TODO: Prepare a qualified name from the type name context.
            jtk_String_t* qualifiedName = NULL;

            /* Retrieve the symbol for the current superclass. Do not begin the resolution
             * from the current scope, which is this class. In the future, if Zen allows
             * inner classes, this would allow inner classes to be inherited by their enclosing
             * classes!
             */
            zen_Symbol_t* symbol = zen_Scope_resolveQualifiedSymbol(
                parentScope, qualifiedName);
            if (zen_Symbol_isClass(symbol)) {
                zen_ClassSymbol_t* classSymbol = (zen_ClassSymbol_t*)symbol->m_context;
                jtk_String_t* fullyQualifiedName = zen_ClassSymbol_getQualifiedName(classSymbol);

                uint16_t superclassIndex = zen_ConstantPoolBuilder_getUtf8EntryIndex(
                    generator->m_constantPoolBuilder, fullyQualifiedName);
                superclassIndexes[index] = superclassIndex;
            }
            else {
                printf("[error] Looks like the semantic anlysis failed.\n");
            }
        }
    }
    else {
        /* The extends clause has not been explicitly written. Therefore,
         * the compiler generates the default extends clause which inherits
         * the zen.core.Object class.
         */
        jtk_String_t* objectClassName = jtk_String_newEx("zen.core.Object", 15);

        superclassCount = 1;
        superclassIndexes = jtk_Memory_allocate(uint16_t, 1);
        superclassIndexes[0] = zen_ConstantPoolBuilder_getUtf8EntryIndex(
            generator->m_constantPoolBuilder, objectClassName);

        jtk_String_delete(objectClassName);
    }

    zen_Entity_t* entity = &generator->m_entityFile->m_entity;
    entity->m_type = ZEN_ENTITY_TYPE_CLASS;
    entity->m_flags = 0;
    entity->m_reference = referenceIndex;
    entity->m_superclassCount = superclassCount;
    entity->m_superclasses = superclassIndexes;
    entity->m_attributeTable.m_size = 0;
    entity->m_fieldCount = 0;
    entity->m_fields = NULL;
    entity->m_functionCount = 0;
    entity->m_functions = NULL;
}

void zen_BinaryEntityGenerator_onExitClassDeclaration(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {
    jtk_Assert_assertObject(astListener, "The specified AST listener is null.");
    jtk_Assert_assertObject(node, "The specified AST node is null.");

    /* Retrieve the generator associated with the AST listener. */
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;

    /* Retrieve the context of the AST node. */
    zen_ClassDeclarationContext_t* context =
        (zen_ClassDeclarationContext_t*)node->m_context;

    /* Invalidate the current scope in the symbol table. */
    zen_SymbolTable_invalidateCurrentScope(generator->m_symbolTable);



    /* Write the number of fields in this class. *
    zen_BinaryEntityBuilder_writeFieldCount(generator->m_builder, generator->m_fieldCount);

    /* Merge the class and field channels. Further, the data in the field channel
     * is not required anymore. Therefore, clear it.
     *
     * It should be noted that the commit function both merges and clears the
     * source channel.
     *
    zen_ChannelManager_commit(generator->m_channels, generator->m_fieldChannel,
        generator->m_classChannel);

    /* At this point, the class channel should contain the class entity header
     * and the fields. Further, the function channel should contain all the
     * function entities. Therefore, we need to merge the class and function
     * channels together.
     *
     * The channels are separated by function count. Therefore, activate the
     * class channel and write the function count before merging.
     *
    zen_BinaryEntityBuilder_writeFunctionCount(generator->m_builder, generator->m_functionCount);

    /* Merge the class and function channels. Further, the data in the function
     * channel is not required anymore. Therefore, clear it.
     *
     * It should be noted that the commit function both merges and clears the
     * source channel.
     *
    zen_ChannelManager_commit(generator->m_channels, generator->m_functionChannel,
        generator->m_classChannel);

    zen_SymbolTable_invalidateCurrentScope(generator->m_symbolTable);
    */
}

// classExtends

void zen_BinaryEntityGenerator_onEnterClassExtendsClause(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitClassExtendsClause(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// classSuite

void zen_BinaryEntityGenerator_onEnterClassSuite(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitClassSuite(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// classMember

void zen_BinaryEntityGenerator_onEnterClassMember(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitClassMember(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// void zen_BinaryEntityGenerator_onEnterConstructorDeclaration(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
// }

// void zen_BinaryEntityGenerator_onExitConstructorDeclaration(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
// }


// enumerationDeclaration

void zen_BinaryEntityGenerator_onEnterEnumerationDeclaration(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitEnumerationDeclaration(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// enumerationBaseClass

void zen_BinaryEntityGenerator_onEnterEnumrationBaseClass(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitEnumerationBaseClause(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// enumerationSuite

void zen_BinaryEntityGenerator_onEnterEnumerationSuite(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitEnumerationSuite(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// enumerate

void zen_BinaryEntityGenerator_onEnterEnumerate(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitEnumerate(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// expressions

void zen_BinaryEntityGenerator_onEnterExpressions(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitExpressions(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// expression

void zen_BinaryEntityGenerator_onEnterExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {
    /*zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_ExpressionContext_t* context = (zen_AssignmentExpressionContext_t*)node->m_context;

    zen_ASTAnnotation_t* annotation = zen_ASTAnnotations_get(generator->m_annotations, node);
    if (annotation != NULL) {
        if (zen_ASTAnnotation_getType(annotation) == ZEN_AST_ANNOTATION_TYPE_ASYMETRICAL_CHANNEL_MANAGEMENT) {
            zen_BinaryEntityBuilder_pushChannel(generator->m_builder);
        }
    }*/
}

void zen_BinaryEntityGenerator_onExitExpression(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// assignmentExpression

// How to differentiate between function calls

/*
 *    ...
 *    invoke_virtual #function_descriptor_index
 *    store_a #x
 */
void zen_BinaryEntityGenerator_onEnterAssignmentExpression(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_AssignmentExpressionContext_t* context = (zen_AssignmentExpressionContext_t*)node->m_context;

    /* The resolution phase ensures that the expression on the left hand
     * side of the assignment operator evaluates to an identifier or
     * field access.
     */
    // jtk_ArrayList_t* children = jtk_ArrayList_new();
    // zen_ASTHelper_getFilteredTokens(context->m_conditionalExpression, children, ZEN_TOKEN_IDENTIFIER);

    /* The following line is valid when zen_ASTHelper_getFilteredTerminalNodes() function
     * is invoked instead of zen_ASTHelper_getFilteredTokens().
     */
    // zen_ASTNode_t* identifier = (zen_ASTNode_t*)jtk_ArrayList_getValue(children, 0);
    // zen_Token_t* identifierToken = (zen_Token_t*)jtk_ArrayList_getValue(children, 0);
    // const uint8_t* identifierText = zen_Token_getText(identifierToken);

    // zen_Symbol_t* symbol = zen_SymbolTable_resolve(generator->m_symbolTable, identifierText);
    // if (zen_Symbol_isVariable(symbol)) {
       //  int32_t targetIndex;
        // bool localVariable = false;

        // zen_Scope_t* enclosingScope = zen_Symbol_getEnclosingScope(symbol);
        // if (zen_Scope_isClassScope(enclosingScope)) {
           //  targetIndex = zen_BinaryEntityGenerator_findFieldDescriptorIndex(generator, children);
        // }
        // else {
           /* int32_t size = jtk_ArrayList_getSize(children);
            if (size == 1) {
                targetIndex = zen_BinaryEntityGenerator_findLocalVariableIndex(generator, identifierText);
                localVariable = true;
            }
            else {
                targetIndex = zen_BinaryEntityGenerator_findFieldDescriptorIndex(generator, children);
            }
        }

        zen_BinaryEntityGenerator_emitLoadReference(generator, targetIndex);

        zen_ASTNode_t* assignmentOperator = context->m_assignmentOperator;
        zen_Token_t* operatorToken = (zen_Token_t*)assignmentOperator->m_context;
        switch (operatorToken->m_type) {
            case ZEN_TOKEN_EQUAL: {
                break;
            }

            case ZEN_TOKEN_PLUS_EQUAL: {
                zen_BinaryEntityGenerator_emitInvokeVirtual(generator, "Czen.base.Object;plusOperator(Czen.base.Object;)Czen.base.Object;");
                break;
            }

            case ZEN_TOKEN_DASH_EQUAL: {
                zen_BinaryEntityGenerator_emitInvokeVirtual(generator, "Czen.base.Object;minusOperator(Czen.base.Object;)Czen.base.Object;");
                break;
            }

            default: {
                // [internal error]
            }
        }

        void** state = zen_Memory_allocate(void*, ZEN_ASSIGNMENT_EXPRESSION_STATE_SIZE);
        state[ZEN_ASSIGNMENT_EXPRESSION_STATE_TARGET_INDEX] = (void*)targetIndex;
        state[ZEN_ASSIGNMENT_EXPRESSION_STATE_LOCAL_VARIABLE] = (void*)localVariable;
        zen_ASTAnnotations_put(generator->m_annotations, node, state);

    }
    else if (zen_Symbol_isConstant(symbol)) {
        // error: Constant cannot be assigned
    }
    else {
        // error: LHS is not variable
    }*/

    // jtk_ArrayList_delete(children);
}

void zen_BinaryEntityGenerator_onExitAssignmentExpression(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {/*
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_AssignmentExpressionContext_t* context = (zen_AssignmentExpressionContext_t*)node->m_context;

    if (context->m_assignmentOperator != NULL) {
        zen_ASTNode_t* assignmentOperator = context->m_assignmentOperator;
        zen_Token_t* operatorToken = (zen_Token_t*)assignmentOperator->m_context;
        switch (operatorToken->m_type) {
            /* The equal operator only stores a reference. Therefore, it requires no
             * special implementation.
             *
            /*
            case ZEN_TOKEN_EQUAL: {
                break;
            }
            *

            case ZEN_TOKEN_ASTERISK_EQUAL: {
                break;
            }

            case ZEN_TOKEN_FORWARD_SLASH_EQUAL: {
                break;
            }

            case ZEN_TOKEN_MODULUS_EQUAL: {
                break;
            }

            case ZEN_TOKEN_PLUS_EQUAL: {
                break;
            }

            case ZEN_TOKEN_DASH_EQUAL: {
                break;
            }

            case ZEN_TOKEN_LEFT_ANGLE_BRACKET_2_EQUAL: {
                break;
            }

            case ZEN_TOKEN_RIGHT_ANGLE_BRACKET_2_EQUAL: {
                break;
            }

            case ZEN_TOKEN_RIGHT_ANGLE_BRACKET_3_EQUAL: {
                break;
            }

            case ZEN_TOKEN_AMPERSAND_EQUAL: {
                break;
            }

            case ZEN_TOKEN_CARET_EQUAL: {
                break;
            }

            case ZEN_TOKEN_VERTICAL_BAR_EQUAL: {
                break;
            }

            default: {
                // [internal error]
            }
        }
        // Depending on the first push reference
        zen_BinaryEntityGenerator_emitStoreReference(generator, 10);
    }*/
}

// conditionalExpression

void zen_BinaryEntityGenerator_onEnterConditionalExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_ConditionalExpressionContext_t* context = (zen_ConditionalExpressionContext_t*)node->m_context;
}

void zen_BinaryEntityGenerator_onExitConditionalExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_ConditionalExpressionContext_t* context = (zen_ConditionalExpressionContext_t*)node->m_context;

    /* TODO */
}

// logicalOrExpression

void zen_BinaryEntityGenerator_onEnterLogicalOrExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_LogicalOrExpressionContext_t* context = (zen_LogicalOrExpressionContext_t*)node->m_context;
}

void zen_BinaryEntityGenerator_onExitLogicalOrExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_LogicalOrExpressionContext_t* context = (zen_LogicalOrExpressionContext_t*)node->m_context;

    /* TODO */
}

// logicalAndExpression

void zen_BinaryEntityGenerator_onEnterLogicalAndExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_LogicalAndExpressionContext_t* context = (zen_LogicalAndExpressionContext_t*)node->m_context;
}

void zen_BinaryEntityGenerator_onExitLogicalAndExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_LogicalAndExpressionContext_t* context = (zen_LogicalAndExpressionContext_t*)node->m_context;

    /* TODO */
}

// inclusiveOrExpression

void zen_BinaryEntityGenerator_onEnterInclusiveOrExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_InclusiveOrExpressionContext_t* context = (zen_InclusiveOrExpressionContext_t*)node->m_context;
}

void zen_BinaryEntityGenerator_onExitInclusiveOrExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {/*
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_InclusiveOrExpressionContext_t* context = (zen_InclusiveOrExpressionContext_t*)node->m_context;

    if (context->m_inclusiveOrExpression != NULL) {
        zen_BinaryEntityGenerator_emitInvokeVirtual(generator, 0);
    }*/
}

// exclusiveOrExpression

void zen_BinaryEntityGenerator_onEnterExclusiveOrExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_ExclusiveOrExpressionContext_t* context = (zen_ExclusiveOrExpressionContext_t*)node->m_context;
}

void zen_BinaryEntityGenerator_onExitExclusiveOrExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {/*
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_ExclusiveOrExpressionContext_t* context = (zen_ExclusiveOrExpressionContext_t*)node->m_context;

    if (context->m_exclusiveOrExpression != NULL) {
        zen_BinaryEntityGenerator_emitInvokeVirtual(generator, 0);
    }*/
}

// andExpression

void zen_BinaryEntityGenerator_onEnterAndExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_AndExpressionContext_t* context = (zen_AndExpressionContext_t*)node->m_context;
}

void zen_BinaryEntityGenerator_onExitAndExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {/*
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_AndExpressionContext_t* context = (zen_AndExpressionContext_t*)node->m_context;

    if (context->m_andExpression != NULL) {
        zen_BinaryEntityGenerator_emitInvokeVirtual(generator, 0);
    }*/
}

// equalityExpression

void zen_BinaryEntityGenerator_onEnterEqualityExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_EqualityExpressionContext_t* context = (zen_EqualityExpressionContext_t*)node->m_context;
}

void zen_BinaryEntityGenerator_onExitEqualityExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {/*
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_EqualityExpressionContext_t* context = (zen_EqualityExpressionContext_t*)node->m_context;

    zen_ASTNode_t* equalityOperator = context->m_equalityOperator;
    if (equalityOperator != NULL) {
        zen_Token_t* equalityOperatorToken = (zen_Token_t*)(equalityOperator->m_context);
        zen_TokenType_t equalityOperatorTokenType = zen_Token_getType(equalityOperatorToken);

        switch (equalityOperatorTokenType) {
            case ZEN_TOKEN_EQUAL_2: {
                zen_BinaryEntityGenerator_emitInvokeVirtual(generator, 0);
                break;
            }

            case ZEN_TOKEN_EXCLAMATION_MARK_EQUAL: {
                zen_BinaryEntityGenerator_emitInvokeVirtual(generator, 0);
                break;
            }
        }
    }*/
}

// relationalExpression

void zen_BinaryEntityGenerator_onEnterRelationalExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_RelationalExpressionContext_t* context = (zen_RelationalExpressionContext_t*)node->m_context;
}

void zen_BinaryEntityGenerator_onExitRelationalExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {/*
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_RelationalExpressionContext_t* context = (zen_RelationalExpressionContext_t*)node->m_context;

    zen_ASTNode_t* relationalOperator = context->m_relationalOperator;
    if (relationalOperator != NULL) {
        zen_Token_t* relationalOperatorToken = (zen_Token_t*)(relationalOperator->m_context);
        zen_TokenType_t relationalOperatorTokenType = zen_Token_getType(relationalOperatorToken);
        switch (relationalOperatorTokenType) {
            case ZEN_TOKEN_LEFT_ANGLE_BRACKET: {
                zen_BinaryEntityGenerator_emitInvokeVirtual(generator, 0);
                break;
            }

            case ZEN_TOKEN_RIGHT_ANGLE_BRACKET: {
                zen_BinaryEntityGenerator_emitInvokeVirtual(generator, 0);
                break;
            }

            case ZEN_TOKEN_LEFT_ANGLE_BRACKET_EQUAL: {
                zen_BinaryEntityGenerator_emitInvokeVirtual(generator, 0);
                break;
            }

            case ZEN_TOKEN_RIGHT_ANGLE_BRACKET_EQUAL: {
                zen_BinaryEntityGenerator_emitInvokeVirtual(generator, 0);
                break;
            }

            case ZEN_TOKEN_KEYWORD_IS: {
                zen_BinaryEntityGenerator_emitInvokeVirtual(generator, 0);
                break;
            }
        }
    }*/
}

// shiftExpression

void zen_BinaryEntityGenerator_onEnterShiftExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_ShiftExpressionContext_t* context = (zen_ShiftExpressionContext_t*)node->m_context;
}

void zen_BinaryEntityGenerator_onExitShiftExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {/*
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_ShiftExpressionContext_t* context = (zen_ShiftExpressionContext_t*)node->m_context;

    zen_ASTNode_t* shiftOperator = context->m_shiftOperator;
    if (shiftOperator != NULL) {
        zen_Token_t* shiftOperatorToken = (zen_Token_t*)(shiftOperator->m_context);
        zen_TokenType_t shiftOperatorTokenType = zen_Token_getType(shiftOperatorToken);
        switch (shiftOperatorTokenType) {
            case ZEN_TOKEN_LEFT_ANGLE_BRACKET_2: {
                zen_BinaryEntityGenerator_emitInvokeVirtual(generator, 0);
                break;
            }

            case ZEN_TOKEN_RIGHT_ANGLE_BRACKET_2: {
                zen_BinaryEntityGenerator_emitInvokeVirtual(generator, 0);
                break;
            }

            case ZEN_TOKEN_RIGHT_ANGLE_BRACKET_3: {
                zen_BinaryEntityGenerator_emitInvokeVirtual(generator, 0);
                break;
            }
        }
    }*/
}

// additiveExpression

void zen_BinaryEntityGenerator_onEnterAdditiveExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_AdditiveExpressionContext_t* context = (zen_AdditiveExpressionContext_t*)node->m_context;
}

void zen_BinaryEntityGenerator_onExitAdditiveExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {/*
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_AdditiveExpressionContext_t* context = (zen_AdditiveExpressionContext_t*)node->m_context;

    zen_ASTNode_t* additiveOperator = context->m_additiveOperator;
    if (additiveOperator != NULL) {
        zen_Token_t* additiveOperatorToken = (zen_Token_t*)(additiveOperator->m_context);
        zen_TokenType_t additiveOperatorTokenType = zen_Token_getType(additiveOperatorToken);
        switch (additiveOperatorTokenType) {
            case ZEN_TOKEN_PLUS: {
                zen_BinaryEntityGenerator_emitInvokeVirtual(generator, /* "..." * 0);
                break;
            }

            case ZEN_TOKEN_DASH: {
                zen_BinaryEntityGenerator_emitInvokeVirtual(generator, 0);
                break;
            }

            default: {
                fprintf(stderr, "[internal error] Control should not reach here.\n");
                break;
            }
        }
    }*/
}

// multiplicativeExpression

void zen_BinaryEntityGenerator_onEnterMultiplicativeExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_MultiplicativeExpressionContext_t* context = (zen_MultiplicativeExpressionContext_t*)node->m_context;
}

void zen_BinaryEntityGenerator_onExitMultiplicativeExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {/*
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_MultiplicativeExpressionContext_t* context = (zen_MultiplicativeExpressionContext_t*)node->m_context;

    zen_ASTNode_t* multiplicativeOperator = context->m_multiplicativeOperator;
    if (multiplicativeOperator != NULL) {
        zen_Token_t* multiplicativeOperatorToken = (zen_Token_t*)(multiplicativeOperator->m_context);
        zen_TokenType_t multiplicativeOperatorTokenType = zen_Token_getType(multiplicativeOperatorToken);
        switch (multiplicativeOperatorTokenType) {
            case ZEN_TOKEN_ASTERISK: {
                zen_BinaryEntityGenerator_emitInvokeVirtual(generator, 0);
                break;
            }

            case ZEN_TOKEN_FORWARD_SLASH: {
                zen_BinaryEntityGenerator_emitInvokeVirtual(generator, 0);
                break;
            }

            case ZEN_TOKEN_MODULUS: {
                zen_BinaryEntityGenerator_emitInvokeVirtual(generator, 0);
                break;
            }
        }
    }*/
}

// unaryExpression

void zen_BinaryEntityGenerator_onEnterUnaryExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_UnaryExpressionContext_t* context = (zen_UnaryExpressionContext_t*)node->m_context;
}

void zen_BinaryEntityGenerator_onExitUnaryExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {/*
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_UnaryExpressionContext_t* context = (zen_UnaryExpressionContext_t*)node->m_context;

    zen_ASTNode_t* unaryOperator = context->m_unaryOperator;
    if (unaryOperator != NULL) {
        zen_Token_t* unaryOperatorToken = (zen_Token_t*)unaryOperator->m_context;
        zen_TokenType_t unaryOperatorType = zen_Token_getType(unaryOperatorToken);
        switch (unaryOperatorType) {
            case ZEN_TOKEN_PLUS: {
                /* The UnaryPlusOperator.applyUnaryPlus() function is invoked against the object
                 * whose "positive" internal state is required. It returns an object with its
                 * internal state "positive".
                 *
                zen_BinaryEntityGenerator_emitInvokeVirtual(generator, 0);
                break;
            }

            case ZEN_TOKEN_DASH: {
                /* The UnaryPlusOperator.applyUnaryPlus() function is invoked against the object
                 * whose "negative" internal state is required. It returns an object with its
                 * internal state "negative".
                 *
                zen_BinaryEntityGenerator_emitInvokeVirtual(generator, 0);
                break;
            }

            case ZEN_TOKEN_TILDE: {
                /* The UnaryPlusOperator.applyUnaryPlus() function is invoked against the object
                 * whose "deeply toggled" internal state is required. It returns an object with its
                 * internal state "deeply toggled".
                 *
                zen_BinaryEntityGenerator_emitInvokeVirtual(generator, 0);
                break;
            }

            case ZEN_TOKEN_EXCLAMATION_MARK: {
                /* The UnaryPlusOperator.applyUnaryPlus() function is invoked against the object
                 * whose "toggled" internal state is required. It returns an object with its
                 * internal state "toggled".
                 *
                zen_BinaryEntityGenerator_emitInvokeVirtual(generator, 0);
                break;
            }

            case ZEN_TOKEN_PLUS_2:
            case ZEN_TOKEN_DASH_2: {
                if (unaryOperatorType == ZEN_TOKEN_PLUS_2) {
                    /* The onPreIncrement() function is invoked against the object whose internal
                     * state has to be "incremented by 1". It returns an object with its internal
                     * state "incremented by 1".
                     *
                    zen_BinaryEntityGenerator_emitInvokeVirtual(generator, 0);
                }
                else {
                    /* The onPreDecrement() function is invoked against the object whose internal
                     * state has to be "incremented by 1". It returns an object with its internal
                     * state "incremented by 1".
                     *
                    zen_BinaryEntityGenerator_emitInvokeVirtual(generator, 0);
                }
                /* A copy of the "incremented" object is required on the operand stack for
                 * assignment.
                 *
                zen_BinaryEntityGenerator_emitDuplicate(generator);
                /* Assign the variable the object which represents the new state.
                 *
                 * TODO: Change store_a to store_a1 (and friends) and
                 *       store_field when necessary.
                 *
                zen_BinaryEntityGenerator_emitStoreReference(generator, 0);

                break;
            }
        }
    }*/
}

// postfixExpression

void zen_BinaryEntityGenerator_onEnterPostfixExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitPostfixExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {/*
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_PostfixExpressionContext_t* context = (zen_PostfixExpressionContext_t*)node->m_context;

    int32_t size = jtk_ArrayList_getSize(context->m_postfixParts);
    int32_t i;
    for (i = 0; i < size; i++) {
        zen_ASTNode_t* postfixPart = (zen_ASTNode_t*)jtk_ArrayList_getValue(context->m_postfixParts, i);
        switch (zen_ASTNode_getType(postfixPart)) {
            /*
            case ZEN_AST_NODE_SUBSCRIPT: {
                zen_SubscriptContext_t* subscriptContext = (zen_SubscriptContext_t*)postfixPart->m_context;
                // TODO: Determine whether this invocation is a load or assignment.
                zen_BinaryEntityGenerator_emitLoadArray(generator);
                break;
            }
            *

            case ZEN_AST_NODE_POSTFIX_OPERATOR: {
                zen_PostfixOperatorContext_t* postfixOperatorContext = (zen_PostfixOperatorContext_t*)postfixPart->m_context;
                zen_Token_t* postfixOperator = (zen_Token_t*)(postfixOperatorContext->m_postfixOperator->m_context);

                /* The virtual onPostIncrement() function is invoked against the object.
                 * Therefore, a duplicate of it is necessary on the operand stack.
                 *
                zen_BinaryEntityGenerator_emitDuplicate(generator);
                if (zen_Token_getType(postfixOperator) == ZEN_TOKEN_PLUS_2) {
                    /* The onPostIncrement() function is invoked against the object whose internal
                     * state has to be "incremented by 1". It returns an object with its internal
                     * state "incremented by 1".
                     *
                    // TODO: Emit an invocation to PostIncrementOperator.applyPostDecrement()
                    zen_BinaryEntityGenerator_emitInvokeVirtual(generator, 0);
                }
                else {
                    /* The onPostDecrement() function is invoked against the object whose internal
                     * state has to be "incremented by 1". It returns an object with its internal
                     * state "incremented by 1".
                     *
                    // TODO: Emit an invocation to PostDecrementOperator.applyPostDecrement()
                    zen_BinaryEntityGenerator_emitInvokeVirtual(generator, 0);
                }
                /* Assign the variable the object which represents the new state.
                 *
                 * TODO: Change store_a to store_a1 (and friends) and
                 *       store_field when necessary.
                 *
                zen_BinaryEntityGenerator_emitStoreReference(generator, 0);

                break;
            }
        }
    }*/
}

// subscript

void zen_BinaryEntityGenerator_onEnterSubscript(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitSubscript(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// functionArguments

void zen_BinaryEntityGenerator_onEnterFunctionArguments(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitFunctionArguments(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// memberAccess

void zen_BinaryEntityGenerator_onEnterMemberAccess(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitMemberAccess(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// postfixOperator

void zen_BinaryEntityGenerator_onEnterPostfixOperator(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitPostfixOperator(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
}

// primaryExpression

void zen_BinaryEntityGenerator_onEnterPrimaryExpression(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_PrimaryExpressionContext_t* context = (zen_PrimaryExpressionContext_t*)node->m_context;

    /* Emit a push instruction if the primary expression is a literal or an
     * identifier.
     */
    if (zen_ASTNode_isTerminal(context->m_expression)) {
        /* Retrieve the token that the primary expression represents. */
        zen_Token_t* token = (zen_Token_t*)context->m_expression->m_context;

        switch (zen_Token_getType(token)) {
            /*
            case ZEN_TOKEN_IDENTIFIER: {
                zen_ASTNode_t* assignmentExpression = zen_ASTHelper_getAncestor(context->m_expression, ZEN_AST_NODE_ASSIGNMENT_EXPRESSION);
                zen_AssignmentExpressionContext_t* assignmentExpressionContext = (zen_AssignmentExpressionContext_t*)assignmentExpression->m_context;
                if (assignmentExpression != NULL) {
                    zen_ASTNode_t* assignmentOperator = assignmentExpressionContext->m_assignmentOperator;
                    if (assignmentOperator != NULL) {
                        zen_Token_t* operatorToken = (zen_Token_t*)(assignmentOperator->m_context);
                        if (zen_Token_getType(operatorToken) == ZEN_TOKEN_EQUAL) {
                            if (token->m_startIndex < operatorToken->m_startIndex) {
                                return;
                            }
                        }
                    }
                }

                const uint8_t* identifierText = zen_Token_getText(token);
                zen_Symbol_t* symbol = zen_SymbolTable_resolve(generator->m_symbolTable, identifierText);
                zen_Scope_t* enclosingScope = zen_Symbol_getEnclosingScope(symbol);
                if (zen_Scope_isClassScope(enclosingScope)) {
                    zen_BinaryEntityGenerator_emitLoadReference(generator, 0);
                    zen_BinaryEntityGenerator_emitLoadInstanceField(generator, 10);
                }
                else {
                    zen_BinaryEntityGenerator_emitLoadReference(generator, 10);
                }
                break;
            }
            */

            case ZEN_TOKEN_INTEGER_LITERAL: {
                uint8_t* integerText = zen_Token_getText(token);
                int32_t actualIntegerLength = zen_Token_getLength(token);
                int32_t integerLength = actualIntegerLength;

                int32_t radix = 10;
                if (integerLength > 2) {
                    if ((integerText[0] == '0') && ((integerText[0] == 'x') || (integerText[0] == 'X'))) {
                        radix = 16;
                        integerText += 2;
                        integerLength -= 2;
                    }
                    else if ((integerText[0] == '0') && ((integerText[0] == 'b') || (integerText[0] == 'B'))) {
                        radix = 2;
                        integerText += 2;
                        integerLength -= 2;
                    }
                    else if ((integerText[0] == '0') && ((integerText[0] == 'c') || (integerText[0] == 'C'))) {
                        /* TODO: Octal integer literals begin with 0c or 0C according
                         * to the logic written here. Therefore, please modify the
                         * lexer accordingly.
                         */

                        radix = 8;
                        integerText += 2;
                        integerLength -= 2;
                    }
                }

                bool longLiteral = (integerText[actualIntegerLength - 1] == 'L') ||
                    (integerText[actualIntegerLength - 1] == 'l');

                if (longLiteral) {
                    integerLength--;
                }

                int64_t value = jtk_Long_parseEx(integerText, integerLength, radix);

                if (longLiteral) {
                    switch (value) {
                        case 0: {
                            zen_BinaryEntityBuilder_emitPushLong0(generator->m_instructions);

                            break;
                        }

                        case 1: {
                            zen_BinaryEntityBuilder_emitPushLong1(generator->m_instructions);

                            break;
                        }

                        case 2: {
                            zen_BinaryEntityBuilder_emitPushLong2(generator->m_instructions);

                            break;
                        }
                    }
                }
                else {
                    switch (value) {
                        case 0: {
                            /* Emit the push_i0 instruction. */
                            zen_BinaryEntityBuilder_emitPushInteger0(generator->m_instructions);

                            /* Log the emission of the instruction. */
                            printf("[debug] Emitted push_i0\n");

                            break;
                        }

                        case 1: {
                            /* Emit the push_i1 instruction. */
                            zen_BinaryEntityBuilder_emitPushInteger1(generator->m_instructions);

                            /* Log the emission of the instruction. */
                            printf("[debug] Emitted push_i1\n");

                            break;
                        }

                        case 2: {
                            /* Emit the push_i2 instruction. */
                            zen_BinaryEntityBuilder_emitPushInteger2(generator->m_instructions);

                            /* Log the emission of the instruction. */
                            printf("[debug] Emitted push_i2\n");

                            break;
                        }

                        case 3: {
                            /* Emit the push_i3 instruction. */
                            zen_BinaryEntityBuilder_emitPushInteger3(generator->m_instructions);

                            /* Log the emission of the instruction. */
                            printf("[debug] Emitted push_i3\n");

                            break;
                        }

                        case 4: {
                            /* Emit the push_i4 instruction. */
                            zen_BinaryEntityBuilder_emitPushInteger4(generator->m_instructions);

                            /* Log the emission of the instruction. */
                            printf("[debug] Emitted push_i4\n");

                            break;
                        }

                        case 5: {
                            /* Emit the push_i5 instruction. */
                            zen_BinaryEntityBuilder_emitPushInteger5(generator->m_instructions);

                            /* Log the emission of the instruction. */
                            printf("[debug] Emitted push_i5\n");

                            break;
                        }

                        default: {
                            /* If the integer literal is less than or equal to 255, then it can be
                             * pushed onto the operand stack with the push_b instruction.
                             *
                             * The range of a byte is [-128, 127].
                             *
                             * As of now, the compiler generates negative values with two instructions.
                             * One instruction pushes the value onto the operand stack. The other
                             * instruction takes care of multiplying a value of -1 to the previously
                             * pushed integer. This will be fixed in the future.
                             */
                            if ((value >= 6) && (value <= 127)) {
                                /* Emit the push_b instruction. */
                                zen_BinaryEntityBuilder_emitPushByte(generator->m_instructions,
                                    value);

                                /* Log the emission of the instruction. */
                                printf("[debug] Emitted push_b %d\n", value);
                            }
                            else if (value <= 32767) {
                                /* If the integer literal is less than or equal to 32767, then it
                                 * can be pushed onto the operand stack with the push_s instruction.
                                 *
                                 * The range of a short is [-32768, 32767].
                                 *
                                 * As of now, the compiler generates negative values with two instructions.
                                 * One instruction pushes the value onto the operand stack. The other
                                 * instruction takes care of multiplying a value of -1 to the previously
                                 * pushed integer. This will be fixed in the future.
                                 */
                                /* Emit the push_s instruction. */
                                zen_BinaryEntityBuilder_emitPushShort(generator->m_instructions,
                                    value);

                                /* Log the emission of the instruction. */
                                printf("[debug] Emitted push_s %d\n", value);
                            }
                            else {
                                /* If the integer literal is larger than 32767, then it should be pushed
                                 * the operand stack with the load_cpr instruction.
                                 *
                                 * As of now, the compiler generates negative values with two instructions.
                                 * One instruction pushes the value onto the operand stack. The other
                                 * instruction takes care of multiplying a value of -1 to the previously
                                 * pushed integer. This will be fixed in the future.
                                 */

                                uint8_t integerIndex = zen_ConstantPoolBuilder_getIntegerEntryIndex(
                                    generator->m_constantPoolBuilder, value);

                                /* Emit the load_cpr instruction. */
                                zen_BinaryEntityBuilder_emitLoadCPR(generator->m_instructions,
                                    integerIndex);

                                /* Log the emission of the instruction. */
                                printf("[debug] Emitted load_cpr %d\n", integerIndex);
                            }

                            break;
                        }
                    }
                }

                break;
            }

            case ZEN_TOKEN_KEYWORD_TRUE: {
                /* Emit push_i1. In the operand stack, 1 represents true. */
                zen_BinaryEntityBuilder_emitPushInteger1(generator->m_instructions);

                /* Log the emission of the instruction. */
                printf("[debug] Emitted push_i1\n");

                break;
            }

            case ZEN_TOKEN_KEYWORD_FALSE: {
                /* Emit push_i0 instruction. In the operand stack, 0 represents false. */
                zen_BinaryEntityBuilder_emitPushInteger0(generator->m_instructions);

                /* Log the emission of the instruction. */
                printf("[debug] Emitted push_i0\n");

                break;
            }

            case ZEN_TOKEN_STRING_LITERAL: {
                /* Retrieve a valid index into the constant pool. The entry at
                 * this index is a constant pool string. The token text encloses
                 * the content within double quotes. Therefore, the first quote
                 * is skipped using pointer arithmetic and the last quote
                 * is skipped by subtracting 1 from the length of the text.
                 * Another 1 is subtracted from the text length because the first
                 * quote was skipped.
                 */
                uint8_t stringIndex = zen_ConstantPoolBuilder_getStringEntryIndexEx(
                    generator->m_constantPoolBuilder, token->m_text + 1, token->m_length - 2);

                /* Emit load_cpr instruction. */
                zen_BinaryEntityBuilder_emitLoadCPR(generator->m_instructions,
                    stringIndex);

                /* Log the emission of the instruction. */
                printf("[debug] Emitted load_cpr %d\n", stringIndex);

                break;
            }

            case ZEN_TOKEN_KEYWORD_NULL: {
                /* Emit the push_null instruction. */
                zen_BinaryEntityBuilder_emitPushNull(generator);

                /* Log the emission of the instruction. */
                printf("[debug] Emitted push_null\n");

                break;
            }

            /*
            default: {
                /* [internal error] *
            }*/
        }
    }
}

void zen_BinaryEntityGenerator_onExitPrimaryExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {
}

// mapExpression

void zen_BinaryEntityGenerator_onEnterMapExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {/*
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_MapExpressionContext_t* context = (zen_MapExpressionContext_t*)node->m_context;

    zen_BinaryEntityGenerator_emitPushByte(generator, 0); // TODO: With proper size
    zen_BinaryEntityGenerator_emitNewReferenceArray(generator, 0); // TODO: new_array_n*/
}

void zen_BinaryEntityGenerator_onExitMapExpression(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
    /*zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_MapExpressionContext_t* context = (zen_MapExpressionContext_t*)node->m_context;

    zen_BinaryEntityGenerator_emitInvokeVirtual(generator, 0); // TODO*/
}

// mapEntries

void zen_BinaryEntityGenerator_onEnterMapEntries(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_MapEntriesContext_t* context = (zen_MapEntriesContext_t*)node->m_context;

    /*
    int32_t i;
    for (i = 0; i < size; i++) {
        zen_ASTNode_t* mapEntry = (zen_ASTNode_t*)jtk_ArrayList_getValue(context->m_mapEntries, i);
        zen_ASTAnnotation_t* mapEntryAnnotation = zen_ASTAnnotation_new(ZEN_AST_ANNOTATION_TYPE_ASYMETRICAL_CHANNEL_MANAGEMENT, NULL);
        zen_ASTAnnotations_put(generator->m_annotations, mapEntryAnnotation);
    }
    */
}

void zen_BinaryEntityGenerator_onExitMapEntries(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {/*
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_MapEntriesContext_t* context = (zen_MapEntriesContext_t*)node->m_context;

    /* TODO: Apparently, storing integers as void* is dangerous. Find an alternative. *

    int32_t size = jtk_ArrayList_getSize(context->m_mapEntries);
    // TODO: Filter out map with 0 entries!
    /* Currently, a map entry channel is on top of the stack. Whatever,
     * byte codes are emitted will be placed on it. Therefore, we need to
     * swap the map entry channel with the local channel.
     *
    zen_ChannelManager_swap(generator->m_channelManager, 0, size);

    int32_t i;
    for (i = 0; i < size; i++) {
        /* The instructions which insert the second dimension array into the
         * first dimension array.
         *
         * The instructions are generated in the reverse order because the
         * channels are stored in a special stack, with an exception of the last
         * element. The last element was previously swapped with the local
         * channel which was previously on the bottom of the stack. Thus, the
         * last element is swapped from top to bottom of the stack. Moreover,
         * the execution order may remain the same for constant entries, but
         * changes for variable entries when the stack ordered in followed.
         *

        /* A textual representation of the channels for a map with four
         * entries.
         *
         * [0] local channel
         * [1] entry_channel_3
         * [2] entry_channel_2
         * [3] entry_channel_1
         * [4] entry_channel_4
         * ...
         *

        /* A new array was previously created for the map entries. Create a
         * duplicate of its reference on the operand stack for calling the
         * {@code store_aa} instruction.
         *

        /* Create a duplicate of the 2-d array. *
        zen_BinaryEntityGenerator_emitDuplicate(generator);
        zen_BinaryEntityGenerator_emitPushByte(generator, i);

        int32_t entryChannelIndex = ((i + 1) == size)? 1 : (size - i - 1);
        zen_ChannelManager_append(generator->m_channelManager, entryChannelIndex, 0);
        zen_ChannelManager_remove(generator->m_channelManager, entryChannelIndex);

        zen_BinaryEntityGenerator_emitStoreReferenceArray(generator);
    }*/
}

// mapEntry

void zen_BinaryEntityGenerator_onEnterMapEntry(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {/*
    zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_MapEntryContext_t* context = (zen_MapEntryContext_t*)node->m_context;

    /* The byte codes generated for every map entry is redirected on a new
     * channel. Thus, an oppurtunity to insert some arbitrary instructions
     * between the map entries is provided.
     *
    zen_BinaryEntityBuilder_pushChannel(generator->m_builder);

    zen_BinaryEntityGenerator_emitPushByte(generator, 2);
    zen_BinaryEntityGenerator_emitNewReferenceArray(generator, 0);

    zen_ASTAnnotation_t* keyExpressionAnnotation = zen_ASTAnnotation_new(ZEN_AST_ANNOTATION_TYPE_ASYMETRICAL_CHANNEL_MANAGEMENT, NULL);
    zen_ASTAnnotations_put(generator->m_annotations, context->m_keyExpression, keyExpressionAnnotation);

    zen_ASTAnnotation_t* valueExpressionAnnotation = zen_ASTAnnotation_new(ZEN_AST_ANNOTATION_TYPE_ASYMETRICAL_CHANNEL_MANAGEMENT, NULL);
    zen_ASTAnnotations_put(generator->m_annotations, context->m_valueExpression, valueExpressionAnnotation);*/
}

void zen_BinaryEntityGenerator_onExitMapEntry(zen_ASTListener_t* astListener, zen_ASTNode_t* node) {
    /*zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_MapEntryContext_t* context = (zen_MapEntryContext_t*)node->m_context;

    /* Before swapping:
     *
     *   value_expression_channel
     *   key_expression_channel
     *   map_entry_channel
     *   ...
     *

    /* Currently, the value expression channel is on top of the stack. Whatever,
     * byte codes are emitted will be placed on it. Therefore, we need to swap
     * the value expression channel with the local channel.
     *
    zen_ChannelManager_swap(generator->m_channelManager, 0, 2);

    /* After swapping:
     *
     *   [0] map_entry_channel
     *   [1] key_expression_channel
     *   [2] value_expression_channel
     *   ...
     *

    /* The following section generates instructions which insert the key into
     * the array in second dimension.
     */
    /* A new array was previously created for this map entry. Create a duplicate
     * of its reference on the operand stack for calling the {@code store_aa}
     * instruction.
     *
    zen_BinaryEntityGenerator_emitDuplicate(generator);
    zen_BinaryEntityGenerator_emitPushByte(generator, 0); // TODO: push_i0
    zen_ChannelManager_append(generator->m_channelManager, 1, 0);
    zen_ChannelManager_remove(generator->m_channelManager, 1);
    zen_BinaryEntityGenerator_emitStoreReferenceArray(generator);

    /* The following section generates instructions which insert the value into
     * the array in second dimension.
     *
    /* A new array was previously created for this map entry. Create a duplicate
     * of its reference on the operand stack for calling the {@code store_aa}
     * instruction.
     *
    zen_BinaryEntityGenerator_emitDuplicate(generator);
    zen_BinaryEntityGenerator_emitPushByte(generator, 1);
    zen_ChannelManager_append(generator->m_channelManager, 1, 0);
    zen_ChannelManager_remove(generator->m_channelManager, 1);
    zen_BinaryEntityGenerator_emitStoreReferenceArray(generator);

    zen_ASTAnnotation_t* keyExpressionAnnotation = zen_ASTAnnotations_get(generator->m_annotations, context->m_keyExpression);
    zen_ASTAnnotation_t* valueExpressionAnnotation = zen_ASTAnnotations_get(generator->m_annotations, context->m_valueExpression);

    zen_ASTAnnotation_delete(keyExpressionAnnotation);
    zen_ASTAnnotation_delete(valueExpressionAnnotation);*/
}

/*
new_array_a #1
push_i 0
... -> key
... -> value
store_aa


new_array_a #1

duplicate
push_i 0
... -> key
store_aa

duplicate
push_i 1
... -> value
store_aa
*/

// listExpression

void zen_BinaryEntityGenerator_onEnterListExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {
    /*zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    zen_ListExpressionContext_t* context = (zen_ListExpressionContext_t*)node->m_context;
    zen_ExpressionsContext_t* expressionsContext = (zen_ExpressionsContext_t*)context->m_expressions->m_context;

    int32_t size = jtk_ArrayList_getSize(expressionsContext->m_expressions);

    zen_BinaryEntityGenerator_emitPushByte(generator, size);
    zen_BinaryEntityGenerator_emitNewReferenceArray(generator, 0);

    int32_t i;
    for (i = 0; i < size; i++) {
        zen_ASTNode_t* expression = (zen_ASTNode_t*)jtk_ArrayList_getValue(expressionsContext->m_expressions, i);
        zen_ASTAnnotation_t* expressionAnnotation = zen_ASTAnnotation_new(ZEN_AST_ANNOTATION_TYPE_ASYMETRICAL_CHANNEL_MANAGEMENT, NULL);
        zen_ASTAnnotations_put(generator->m_annotations, expression, expressionAnnotation);
    }
    */
}

void zen_BinaryEntityGenerator_onExitListExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {
    // zen_BinaryEntityGenerator_t* generator = (zen_BinaryEntityGenerator_t*)astListener->m_context;
    // zen_ListExpressionContext_t* context = (zen_ListExpressionContext_t*)node->m_context;
    // zen_ExpressionsContext_t* expressionsContext = (zen_ExpressionsContext_t*)context->m_expressions->m_context;

    // int32_t size = jtk_ArrayList_getSize(expressionsContext->m_expressions);
    /* Currently, an expression channel is on top of the stack. Whatever,
     * byte codes are emitted will be placed on it. Therefore, we need to
     * swap the expression channel with the local channel.
     */
    // zen_BinaryEntityBuilder_swapChannels(generator->m_channels, 0, size);

    // int32_t i;
    // for (i = 0; i < size; i++) {
        /* A textual representation of the channels for an array with four
         * elements.
         *
         * [0] local channel
         * [1] expression_channel_3
         * [2] expression_channel_2
         * [3] expression_channel_1
         * [4] expression_channel_4
         * ...
         */

        /* Create a duplicate of the array. */
        // zen_BinaryEntityGenerator_emitDuplicate(generator);
        // zen_BinaryEntityGenerator_emitPushByte(generator, i);

        // int32_t expressionChannelIndex = ((i + 1) == size)? 1 : (size - i - 1);
        // zen_ChannelManager_append(generator->m_channelManager, expressionChannelIndex, 0);
        // zen_ChannelManager_remove(generator->m_channelManager, expressionChannelIndex);

        // zen_BinaryEntityGenerator_emitStoreReferenceArray(generator);

        // zen_ASTNode_t* expression = (zen_ASTNode_t*)jtk_ArrayList_getValue(expressionsContext->m_expressions, i);
        // zen_ASTAnnotation_t* expressionAnnotation = zen_ASTAnnotations_get(generator->m_annotations, expression);
        // zen_ASTAnnotation_delete(expressionAnnotation);
    // }
}

// New Expression

void zen_BinaryEntityGenerator_onEnterNewExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {
}

void zen_BinaryEntityGenerator_onExitNewExpression(zen_ASTListener_t* astListener,
    zen_ASTNode_t* node) {
}