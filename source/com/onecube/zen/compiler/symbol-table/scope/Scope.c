#include <zen/CString.h>
#include <zen/st/Scope.h>

zen_Scope_t* zen_Scope_new(const uint8_t* name, zen_ScopeType_t type,
    zen_Scope_t* enclosingScope, void* context) {
    jtk_Assert_assertObject(name, "The specified name is null.");

    zen_Scope_t* scope = zen_Memory_allocate(zen_Scope_t, 1);
    scope->m_name = zen_String_new(name);
    scope->m_type = type;
    scope->m_enclosingScope = enclosingScope;
    scope->m_context = context;
    scope->m_resolveSymbol = NULL;
    
    return scope;
}

void zen_Scope_delete(zen_Scope_t* scope) {
    jtk_Assert_assertObject(scope, "The specified scope is null.");

    zen_String_delete(scope->m_name);
    zen_Memory_deallocate(scope);
}

bool zen_Scope_isEnumerationScope(zen_Scope_t* scope) {
    return scope->m_type == ZEN_SCOPE_ENUMERATION;
}

bool zen_Scope_isClassScope(zen_Scope_t* scope) {
    return scope->m_type == ZEN_SCOPE_CLASS;
}

bool zen_Scope_isCompilationUnitScope(zen_Scope_t* scope) {
    return scope->m_type == ZEN_SCOPE_COMPILATION_UNIT;
}

bool zen_Scope_isLocalScope(zen_Scope_t* scope) {
    return scope->m_type == ZEN_SCOPE_LOCAL;
}

zen_Scope_t* zen_Scope_getEnclosingScope(zen_Scope_t* scope) {
    jtk_Assert_assertObject(scope, "The specified scope is null.");
    return scope->m_enclosingScope;
}

void zen_Scope_define(zen_Scope_t* scope, zen_Symbol_t* symbol) {
    scope->m_defineSymbol(scope->m_context, symbol);
}

zen_Symbol_t* zen_Scope_resolve(zen_Scope_t* scope, const uint8_t* identifier) {
    return scope->m_resolveSymbol(scope->m_context, identifier);
}

const uint8_t* zen_Scope_getName(zen_Scope_t* scope) {
    return scope->m_name;
}

zen_ScopeType_t zen_Scope_getType(zen_Scope_t* scope) {
    return scope->m_type;
}

void* zen_Scope_getContext(zen_Scope_t* scope) {
    return scope->m_context;
}