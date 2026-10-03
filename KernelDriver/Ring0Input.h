#pragma once
#include <ntddk.h>
#include "KernelDriver.h"

#ifdef __cplusplus
extern "C" {
#endif

NTSTATUS Ring0InputInitialize();
NTSTATUS Ring0InjectInput(PRING0_INPUT_REQUEST InputReq);

#ifdef __cplusplus
}
#endif
