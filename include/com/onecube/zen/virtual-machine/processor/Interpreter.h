// Sunday, September 16, 2018

#ifndef COM_ONECUBE_ZEN_VIRTUAL_MACHINE_PROCESSOR_INTERPRETER_H
#define COM_ONECUBE_ZEN_VIRTUAL_MACHINE_PROCESSOR_INTERPRETER_H

#include <jtk/log/Logger.h>

#include <com/onecube/zen/Configuration.h>
#include <com/onecube/zen/virtual-machine/processor/InvocationStack.h>
#include <com/onecube/zen/virtual-machine/processor/ProcessorThread.h>

#define ZEN_INTERPRETER_NULL_REFERENCE 0

/*******************************************************************************
 * Interpreter                                                                 *
 *******************************************************************************/

/**
 * @class Interpreter
 * @ingroup zen_vm_processor
 * @author Samuel Rowe
 * @since zen 1.0
 */
struct zen_Interpreter_t {
    int32_t m_counter;
    zen_InvocationStack_t* m_invocationStack;
    zen_ProcessorThread_t* m_processorThread;
    jtk_Logger_t* m_logger;
};

/**
 * @memberof Interpreter
 */
typedef struct zen_Interpreter_t zen_Interpreter_t;

/* Constructor */

/**
 * @memberof Interpreter
 */
zen_Interpreter_t* zen_Interpreter_new(zen_MemoryManager_t* memoryManager, zen_ProcessorThread_t* processorThread);

/* Destructor */

/**
 * @memberof Interpreter
 */
void zen_Interpreter_delete(zen_MemoryManager_t* memoryManager, zen_Interpreter_t* interpreter);

/* Read */

uint8_t zen_Interpreter_readByte(zen_Interpreter_t* interpreter);
uint16_t zen_Interpreter_readShort(zen_Interpreter_t* interpreter);

#endif /* COM_ONECUBE_ZEN_VIRTUAL_MACHINE_PROCESSOR_INTERPRETER_H */