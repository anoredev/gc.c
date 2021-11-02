#include <stdio.h>
#include <stdlib.h>


#define STACK_MAX_SIZE 256
#define IGCT 8



typedef enum {
    INT,
    TWIN
} oType;

typedef struct sObject {
    oType type;
    unsigned char marked = 0; // gc algorithm flag
    struct sObject* next;

    union {
        int value;
        struct {
            struct sObject* head;
            struct sObject* tail;
        };
    };
} Object;

typedef struct {
    unsigned int stackSize;
    unsigned int numObjects;
    unsigned int maxObjects;
    Object* stack[STACK_MAX_SIZE];
    Object* firstObject;
} VirtualMachine;



VirtualMachine* newVirtualMachine() {
    VirtualMachine* VMInstance = (VirtualMachine*)malloc(sizeof(VirtualMachine));
    VMInstance->stackSize = 0;
    VMInstance->firstObject = NULL;
    VMInstance->numObjects = 0;
    VMInstance->maxObjects = IGCT;

    return VMInstance;
}

void push(VirtualMachine* vm, Object* value) {
    vm->stack[vm->stackSize++] = value;
}

Object* pop(VirtualMachine* vm) {
    return vm->stack[--vm->stackSize];
}



void markObject(Object* object) {
    if (object->marked) return;
    object->marked = 1;
    if (object->type == TWIN) {
        markObject(object->head);
        markObject(object->tail);
    }
}

void markAll(VirtualMachine* vm) {
    for (unsigned int i = 0; i < vm->stackSize; i++) {
        markObject(vm->stack[i]);
    }
}

/*
  Mark Sweep Algorithm
*/
void markSweep(VirtualMachine* vm) {
    Object** object = &vm->firstObject;
    while (*object) {
        if (!(*object)->marked) {
            Object* unreached = *object;
            *object = unreached->next;
            free(unreached);
        } else {
            (*object)->marked = 0;
            object = &(*object)->next;
        }
    }
}

void collectGarbage(VirtualMachine* vm) {
    markAll(vm);
    markSweep(vm);
    vm->maxObjects = vm->numObjects * 2;
}



Object* newObject(VirtualMachine* vm, oType type) {
    if (vm->numObjects == vm->maxObjects) collectGarbage(vm);

    Object* object = (Object*)malloc(sizeof(Object));
    object->type = type;

    vm->maxObjects++;

    return object;
}



Object* pushInt(VirtualMachine* vm, int intValue) {
    Object* intObject = newObject(vm, INT);
    intObject->value = intValue;
    push(vm, intObject);

    return intObject;
}

Object* pushPair(VirtualMachine* vm) {
    Object* twinObject = newObject(vm, TWIN);
    twinObject->tail = pop(vm);
    twinObject->head = pop(vm);
    push(vm, twinObject);
    
    return twinObject;
}



int main() {
    return 0;
}