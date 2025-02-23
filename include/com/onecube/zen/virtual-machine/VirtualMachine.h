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

// Friday, May 18, 2018

#ifndef COM_ONECUBE_ZEN_VIRTUAL_MACHINE_VIRTUAL_MACHINE_H
#define COM_ONECUBE_ZEN_VIRTUAL_MACHINE_VIRTUAL_MACHINE_H

#include <jtk/collection/map/HashMap.h>

#include <com/onecube/zen/Configuration.h>
#include <com/onecube/zen/virtual-machine/ExceptionManager.h>
#include <com/onecube/zen/virtual-machine/VirtualMachineConfiguration.h>
#include <com/onecube/zen/virtual-machine/object/Class.h>
#include <com/onecube/zen/virtual-machine/object/Function.h>
#include <com/onecube/zen/virtual-machine/object/NativeFunction.h>
#include <com/onecube/zen/virtual-machine/object/ObjectArray.h>
#include <com/onecube/zen/virtual-machine/loader/EntityLoader.h>
#include <com/onecube/zen/virtual-machine/loader/ClassLoader.h>
#include <com/onecube/zen/virtual-machine/processor/Interpreter.h>

/*******************************************************************************
 * VirtualMachine                                                              *
 *******************************************************************************/

/**
 * @class VirtualMachine
 * @ingroup zen_virtual_machine_native
 * @author Samuel Rowe
 * @since zen 1.0
 */
struct zen_VirtualMachine_t {
    zen_VirtualMachineConfiguration_t* m_configuration;
    zen_EntityLoader_t* m_entityLoader;
    zen_ClassLoader_t* m_classLoader;
    zen_Interpreter_t* m_interpreter;
    jtk_HashMap_t* m_nativeFunctions;
};

/**
 * @memberof VirtualMachine
 */
typedef struct zen_VirtualMachine_t zen_VirtualMachine_t;

/* Constructor */

/**
 * @memberof VirtualMachine
 */
zen_VirtualMachine_t* zen_VirtualMachine_new(zen_VirtualMachineConfiguration_t* configuration);

/* Destructor */

/**
 * @memberof VirtualMachine
 */
void zen_VirtualMachine_delete(zen_VirtualMachine_t* virtualMachine);

/* Context */

/**
 * The standard provides two classes to manipulate the virtual machine directly.
 * - `VirtualMachine`
 *   The `VirtualMachine` class provides a low-level API.
 * - `Context`
 *   The `Context` class provides a high-level API. Internally, this class is
 *   powered by the low-level APIs provided by the `VirtualMachine` class.
 */
// zen_VirtualMachineContext_t* zen_VirtualMachine_getContext(zen_VirtualMachine_t* virtualMachine);


/* Class */

zen_Class_t* zen_VirtualMachine_getClass(zen_VirtualMachine_t* context, const uint8_t* descriptor);

/* Clear */

bool zen_VirtualMachine_isClear(zen_VirtualMachine_t* context);

/* Handle Exception */

void zen_VirtualMachine_handleException(zen_VirtualMachine_t* context);

/* New Object Array */

zen_ObjectArray_t* zen_VirtualMachine_newObjectArray(zen_VirtualMachine_t* context, zen_Class_t* class0, int32_t size);

/* Static Function */

zen_Function_t* zen_VirtualMachine_getStaticFunction(zen_VirtualMachine_t* context, zen_Class_t* handle, const uint8_t* identifier, const uint8_t* signature);
void zen_VirtualMachine_invokeStaticFunction(zen_VirtualMachine_t* context, zen_Function_t* function, ...);

/* String */

zen_Object_t* zen_VirtualMachine_newStringFromUtf8(zen_VirtualMachine_t* context, const uint8_t* string, int32_t length);

zen_EntityLoader_t* zen_VirtualMachine_getEntityLoader(zen_VirtualMachine_t* virtualMachine);
zen_Object_t* zen_VirtualMachine_newString(zen_VirtualMachine_t* virtualMachine,
    const uint8_t* string);

/* Exception Manager */

zen_ExceptionManager_t* zen_VirtualMachine_getExceptionManager(zen_VirtualMachine_t* virtualMachine);

void zen_VirtualMachine_raiseException(zen_VirtualMachine_t* virtualMachine,
    zen_Object_t* exception);

zen_Object_t* zen_VirtualMachine_newObject(zen_VirtualMachine_t* virtualMachine,
    const uint8_t* classDescriptor, const uint8_t* constructorDescriptor, ...);

void zen_VirtualMachine_raiseFunctionNotFoundException(zen_VirtualMachine_t* virtualMachine,
    const uint8_t* identifier,  const uint8_t* reason);

#endif /* COM_ONECUBE_ZEN_VIRTUAL_MACHINE_VIRTUAL_MACHINE_H */