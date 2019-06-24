// Saturday, February 24, 2018

#include <zen/st/FunctionSymbol.h>

/*******************************************************************************
 * FunctionSymbol                                                              *
 *******************************************************************************/

zen_FunctionSymbol_t* zen_FunctionSymbol_new(zen_ASTNode_t* identifier, zen_Scope_t* enclosingScope) {
    zen_FunctionSymbol_t* functionSymbol = zen_Memory_allocate(zen_FunctionSymbol_t, 1);

    zen_Symbol_t* symbol = zen_Symbol_new(ZEN_SYMBOL_CATEGORY_FUNCTION, identifier, enclosingScope, functionSymbol);

    functionSymbol->m_symbol = symbol;
    functionSymbol->m_signatures = zen_ArrayList_new();
    functionSymbol->m_modifiers = zen_ArrayList_new();
    functionSymbol->m_parameterThreshold = -1;

    return functionSymbol;
}

void zen_FunctionSymbol_delete(zen_FunctionSymbol_t* symbol) {
    jtk_Assert_assertObject(symbol, "The specified symbol is null.");

    zen_Symbol_delete(symbol->m_symbol);
    int32_t size = zen_ArrayList_getSize(symbol->m_signatures);
    int32_t i;
    for (i = 0; i < size; i++) {
        zen_FunctionSignature_t* signature = (zen_FunctionSignature_t*)zen_ArrayList_get(symbol->m_signatures, i);
        zen_FunctionSignature_delete(signature);
    }
    zen_ArrayList_delete(symbol->m_signatures);
    zen_ArrayList_delete(symbol->m_modifiers);
    zen_Memory_deallocate(symbol);
}

zen_ArrayList_t* zen_FunctionSymbol_getSignatures(zen_FunctionSymbol_t* symbol) {
    jtk_Assert_assertObject(symbol, "The specified symbol is null.");

    return symbol->m_signatures;
}

void zen_FunctionSymbol_addSignature(zen_FunctionSymbol_t* symbol, zen_FunctionSignature_t* signature) {
    jtk_Assert_assertObject(symbol, "The specified symbol is null.");

    zen_ArrayList_add(symbol->m_signatures, signature);
}

int32_t zen_FunctionSymbol_getParameterThreshold(zen_FunctionSymbol_t* symbol) {
    jtk_Assert_assertObject(symbol, "The specified symbol is null.");

    return symbol->m_parameterThreshold;
}

bool zen_FunctionSymbol_hasParameterThreshold(zen_FunctionSymbol_t* symbol) {
    jtk_Assert_assertObject(symbol, "The specified symbol is null.");

    printf("has pt: %d\n", symbol->m_parameterThreshold != -1);
    return symbol->m_parameterThreshold != -1;
}

void zen_FunctionSymbol_setParameterThreshold(zen_FunctionSymbol_t* symbol, int32_t parameterThreshold) {
    jtk_Assert_assertObject(symbol, "The specified symbol is null.");

    symbol->m_parameterThreshold = parameterThreshold;
}

zen_Symbol_t* zen_FunctionSymbol_getSymbol(zen_FunctionSymbol_t* symbol) {
    jtk_Assert_assertObject(symbol, "The specified symbol is null.");

    return symbol->m_symbol;
}