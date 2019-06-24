// Monday, January 08, 2018

#include <zen/st/Symbol.h>

/*******************************************************************************
 * Symbol                                                                      *
 *******************************************************************************/

zen_Symbol_t* zen_Symbol_new(zen_SymbolCategory_t category,
    zen_ASTNode_t* identifier, zen_Scope_t* enclosingScope, void* context) {
    zen_Symbol_t* symbol = zen_Memory_allocate(zen_Symbol_t, 1);
    symbol->m_category = category;
    symbol->m_identifier = identifier;
    symbol->m_enclosingScope = enclosingScope;
    symbol->m_context = context;

    return symbol;
}

void zen_Symbol_delete(zen_Symbol_t* symbol) {
    jtk_Assert_assertObject(symbol, "The specified symbol is null.");

    zen_Memory_deallocate(symbol);
}

bool zen_Symbol_isEnumeration(zen_Symbol_t* symbol) {
    jtk_Assert_assertObject(symbol, "The specified symbol is null.");
    return (symbol->m_category == ZEN_SYMBOL_CATEGORY_ENUMERATION);
}

bool zen_Symbol_isEnumerate(zen_Symbol_t* symbol) {
    jtk_Assert_assertObject(symbol, "The specified symbol is null.");
    return (symbol->m_category == ZEN_SYMBOL_CATEGORY_ENUMERATE);
}

bool zen_Symbol_isFunction(zen_Symbol_t* symbol) {
    jtk_Assert_assertObject(symbol, "The specified symbol is null.");
    return (symbol->m_category == ZEN_SYMBOL_CATEGORY_FUNCTION);
}

bool zen_Symbol_isConstant(zen_Symbol_t* symbol) {
    jtk_Assert_assertObject(symbol, "The specified symbol is null.");
    return (symbol->m_category == ZEN_SYMBOL_CATEGORY_CONSTANT);
}

bool zen_Symbol_isVariable(zen_Symbol_t* symbol) {
    jtk_Assert_assertObject(symbol, "The specified symbol is null.");
    return (symbol->m_category == ZEN_SYMBOL_CATEGORY_VARIABLE);
}

bool zen_Symbol_isClass(zen_Symbol_t* symbol) {
    jtk_Assert_assertObject(symbol, "The specified symbol is null.");
    return (symbol->m_category == ZEN_SYMBOL_CATEGORY_CLASS);
}

zen_Scope_t* zen_Symbol_getEnclosingScope(zen_Symbol_t* symbol) {
    jtk_Assert_assertObject(symbol, "The specified symbol is null.");
    return symbol->m_enclosingScope;
}

zen_ASTNode_t* zen_Symbol_getIdentifier(zen_Symbol_t* symbol) {
    return symbol->m_identifier;
}