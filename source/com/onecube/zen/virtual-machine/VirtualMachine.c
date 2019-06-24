// Friday, June 08, 2018

#include <com/onecube/zen/virtual-machine/VirtualMachine.h>
#include <com/onecube/zen/virtual-machine/loader/EntityLoader.h>
#include <com/onecube/zen/virtual-machine/ExceptionManager.h>

/*******************************************************************************
 * VirtualMachine                                                              *
 *******************************************************************************/

/* Constructor */

zen_VirtualMachine_t* zen_VirtualMachine_new(zen_VirtualMachineConfiguration_t* configuration) {
    jtk_Assert_assertObject(configuration, "The specified virtual machine configuration is null.");

    jtk_Iterator_t* entityDirectoryIterator = jtk_ArrayList_getIterator(configuration->m_entityDirectories);

    zen_VirtualMachine_t* virtualMachine = zen_Memory_allocate(zen_VirtualMachine_t, 1);
    virtualMachine->m_configuration = configuration;
    virtualMachine->m_entityLoader = zen_EntityLoader_newWithEntityDirectories(entityDirectoryIterator);

    return virtualMachine;
}

/* Destructor */

void zen_VirtualMachine_delete(zen_VirtualMachine_t* virtualMachine) {
    jtk_Assert_assertObject(virtualMachine, "The specified virtual machine is null.");

    zen_Memory_deallocate(virtualMachine);
}

/* Context */
/*
zen_VirtualMachine_t* zen_VirtualMachine_getContext(zen_VirtualMachine_t* virtualMachine) {
    jtk_Assert_assertObject(virtualMachine, "The specified virtual machine is null.");

    return virtualMachine->m_context;
}
*/

/* Class */

zen_Class_t* zen_VirtualMachine_getClass(zen_VirtualMachine_t* virtualMachine,
    const uint8_t* descriptor) {
    jtk_Assert_assertObject(virtualMachine, "The specified virtual machine is null.");
    jtk_Assert_assertObject(descriptor, "The specified class descriptor is null.");

    zen_EntityLoader_t* loader = zen_VirtualMachine_getEntityLoader(virtualMachine);
#warning "Ignoring return value of findEntity!"
    zen_EntityLoader_findEntity(loader, descriptor);

    zen_Class_t* class0 = NULL;

    /* The class with the specified descriptor was not found. Throw an instance
     * of the `zen.base.ClassNotFoundException` class.
     */
    if (class0 == NULL) {
        zen_VirtualMachine_raiseClassNotFoundException(virtualMachine, descriptor);
    }
    return class0;
}

/* Raise Exception */

void zen_VirtualMachine_raiseClassNotFoundException(zen_VirtualMachine_t* virtualMachine,
    const uint8_t* reason) {
    jtk_Assert_assertObject(virtualMachine, "The specified virtual machine is null.");
    jtk_Assert_assertObject(reason, "The specified class descriptor is null.");

    /* Find the `ClassNotFoundException` class. */
    const uint8_t* descriptor = "zen.core.ClassNotFoundException";

    zen_Class_t* handle = NULL;

    // zen_EntityLoader_findClass(entityLoader, descriptor);
    zen_EntityLoader_findEntity(virtualMachine->m_entityLoader, descriptor);

    /* The message of the exception. */
    zen_Object_t* message = zen_VirtualMachine_newString(virtualMachine, reason);

    /* The descriptor of the constructor which will initializer the exception
     * object.
     */
    const uint8_t* constructor = "(Czen.core.String;)";

    /* Create an instance of the `zen.core.ClassNotFoundException` class. */
    zen_Object_t* exception = zen_VirtualMachine_newObject(virtualMachine, descriptor, constructor, message);

    /* Throw the exception. */
    zen_VirtualMachine_raiseException(virtualMachine, exception);
}

void zen_VirtualMachine_raiseFunctionNotFoundException(zen_VirtualMachine_t* virtualMachine,
    const uint8_t* identifier, const uint8_t* reason) {
    jtk_Assert_assertObject(virtualMachine, "The specified virtual machine is null.");
    jtk_Assert_assertObject(reason, "The specified class descriptor is null.");

    /* Find the `FunctionNotFoundException` class. */
    const uint8_t* descriptor = "zen.core.FunctionNotFoundException";
    zen_Class_t* handle = NULL;

    // zen_EntityLoader_findClass(entityLoader, descriptor);
    zen_EntityLoader_findEntity(virtualMachine->m_entityLoader, descriptor);

    /* The message of the exception. */
    zen_Object_t* message = zen_VirtualMachine_newString(virtualMachine, reason);

    /* The descriptor of the constructor which will initializer the exception
     * object.
     */
    const uint8_t* constructor = "(Czen.core.String;)";

    /* Create an instance of the `zen.core.ClassNotFoundException` class. */
    zen_Object_t* exception = zen_VirtualMachine_newObject(virtualMachine, descriptor, constructor, message);

    /* Throw the exception. */
    zen_VirtualMachine_raiseException(virtualMachine, exception);
}

/* Clear */

bool zen_VirtualMachine_isClear(zen_VirtualMachine_t* virtualMachine) {
    zen_ExceptionManager_t* manager = zen_VirtualMachine_getExceptionManager(virtualMachine);
    return zen_ExceptionManager_isClear(manager);
}

/* Entity Loader */

zen_EntityLoader_t* zen_VirtualMachine_getEntityLoader(
    zen_VirtualMachine_t* virtualMachine) {
    jtk_Assert_assertObject(virtualMachine, "The specified virtual machine is null.");

    return virtualMachine->m_entityLoader;
}

/* Exception Manager */

zen_ExceptionManager_t* zen_VirtualMachine_getExceptionManager(zen_VirtualMachine_t* virtualMachine) {
    return NULL;
}

/* Handle Exception */

void zen_VirtualMachine_handleException(zen_VirtualMachine_t* virtualMachine) {
}

/* Object */

zen_Object_t* zen_VirtualMachine_newObject(zen_VirtualMachine_t* virtualMachine,
    const uint8_t* classDescriptor, const uint8_t* constructorDescriptor, ...) {
    return NULL;
}

/* Object Array */

zen_ObjectArray_t* zen_VirtualMachine_newObjectArray(zen_VirtualMachine_t* virtualMachine,
    zen_Class_t* class0, int32_t size) {
    return NULL;
}

/* Raise Exception */

void zen_VirtualMachine_raiseException(zen_VirtualMachine_t* virtualMachine,
    zen_Object_t* exception) {
    jtk_Assert_assertObject(virtualMachine, "The specified virtual machine is null.");
    jtk_Assert_assertObject(exception, "The specified exception is null.");

    zen_ExceptionManager_t* manager = zen_VirtualMachine_getExceptionManager(virtualMachine);
    zen_ExceptionManager_raiseException(manager, exception);
}

/* Static Function */

zen_Function_t* zen_VirtualMachine_getStaticFunction(zen_VirtualMachine_t* virtualMachine,
    zen_Class_t* handle, const uint8_t* identifier, const uint8_t* signature) {
    jtk_Assert_assertObject(virtualMachine, "The specified virtual machine is null.");
    jtk_Assert_assertObject(handle, "The specified class is null.");
    jtk_Assert_assertObject(identifier, "The specified identifier is null.");
    jtk_Assert_assertObject(signature, "The specified signature is null.");

    zen_Function_t* function = zen_Class_getStaticFunction(handle, identifier, signature);
    if (function == NULL) {
        zen_VirtualMachine_raiseFunctionNotFoundException(virtualMachine, identifier, signature);
    }
    return function;
}

void zen_VirtualMachine_invokeStaticFunction(zen_VirtualMachine_t* virtualMachine,
    zen_Function_t* function, ...) {
}

/* String */

zen_Object_t* zen_VirtualMachine_getEmptyString(zen_VirtualMachine_t* virtualMachine) {
    return zen_VirtualMachine_newStringFromUtf8(virtualMachine, NULL, 0);
}

zen_Object_t* zen_VirtualMachine_newStringFromUtf8(zen_VirtualMachine_t* virtualMachine,
    const uint8_t* string, int32_t size) {
    jtk_Assert_assertObject(virtualMachine, "The specified virtual machine is null.");
    /*
    size = (size < 0)? jtk_Utf8String_getSize(string) : size;
    const uint8_t* descriptor

    return result;
    */
    return NULL;
}

zen_Object_t* zen_VirtualMachine_newString(zen_VirtualMachine_t* virtualMachine,
    const uint8_t* string) {
    return NULL;
}

/* Shutdown */

void zen_VirtualMachine_shutDown(zen_VirtualMachine_t* virtualMachine) {
    /* In C, the process is terminated if the main thread exits. Therefore, we
     * need to wait for the threads spawned by the virtual machine to complete.
     */
    zen_VirtualMachine_waitForThreads(virtualMachine);

    /* Notify all the registered listeners of the "shut down" event. */
    zen_VirtualMachine_notifyShutDown(virtualMachine);

    /* The listeners may have spawned threads. The virtual machine has to wait
     * for their completion.
     */
    zen_VirtualMachine_waitForThreads(virtualMachine);

    /* Tear down the virtual machine. */
    zen_VirtualMachine_tearDown(virtualMachine);
}

void zen_VirtualMachine_waitForThreads(zen_VirtualMachine_t* virtualMachine) {
}

void zen_VirtualMachine_notifyShutDown(zen_VirtualMachine_t* virtualMachine) {
}

void zen_VirtualMachine_tearDown(zen_VirtualMachine_t* virtualMachine) {
}