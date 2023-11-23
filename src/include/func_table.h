#pragma once

#include "tests_av.h"
#include "tests_dbg.h"
#include "tests_ex.h"
#include "tests_fsc.h"
#include "tests_hal.h"
#include "tests_interlocked.h"
#include "tests_io.h"
#include "tests_iof.h"
#include "tests_kd.h"
#include "tests_ke.h"
#include "tests_kf.h"
#include "tests_ki.h"
#include "tests_mm.h"
#include "tests_nt.h"
#include "tests_ob.h"
#include "tests_obf.h"
#include "tests_obp.h"
#include "tests_phy.h"
#include "tests_ps.h"
#include "tests_rtl.h"
#include "tests_xbox.h"
#include "tests_xe.h"
#include "tests_xc.h"
#include "tests_irt.h"
#include "tests_xprofp.h"
#include "tests_unknown.h"
#include "tests_misc.h"

void (*kernel_thunk_table[])(void) =
{
	test_panic,                                                  // 0x0000 (0) NULL
	/*
	test_AvGetSavedDataAddress,                                  // 0x0001 (1)
	test_AvSendTVEncoderOption,                                  // 0x0002 (2)
	test_AvSetDisplayMode,                                       // 0x0003 (3)
	test_AvSetSavedDataAddress,                                  // 0x0004 (4)
	test_DbgBreakPoint,                                          // 0x0005 (5)
	test_DbgBreakPointWithStatus,                                // 0x0006 (6)
	test_DbgLoadImageSymbols,                                    // 0x0007 (7) DEVKIT
 	*/
	test_DbgPrint,                                               // 0x0008 (8)
	/*
	test_HalReadSMCTrayState,                                    // 0x0009 (9)
	test_DbgPrompt,                                              // 0x000A (10)
	test_DbgUnLoadImageSymbols,                                  // 0x000B (11) DEVKIT
	test_ExAcquireReadWriteLockExclusive,                        // 0x000C (12)
	test_ExAcquireReadWriteLockShared,                           // 0x000D (13)
 	*/
	test_ExAllocatePool,                                         // 0x000E (14)
	test_ExAllocatePoolWithTag,                                  // 0x000F (15)
	/*
	test_ExEventObjectType,                                      // 0x0010 (16)
 	*/
	test_ExFreePool,                                             // 0x0011 (17)
	/*
	test_ExInitializeReadWriteLock,                              // 0x0012 (18)
	test_ExInterlockedAddLargeInteger,                           // 0x0013 (19)
	test_ExInterlockedAddLargeStatistic,                         // 0x0014 (20)
	test_ExInterlockedCompareExchange64,                         // 0x0015 (21)
 	*/
	test_ExMutantObjectType,                                     // 0x0016 (22)
	test_ExQueryPoolBlockSize,                                   // 0x0017 (23)
	test_ExQueryNonVolatileSetting,                              // 0x0018 (24)
	/*
	test_ExReadWriteRefurbInfo,                                  // 0x0019 (25)
 	*/
	test_ExRaiseException,                                       // 0x001A (26)
	test_ExRaiseStatus,                                          // 0x001B (27)
	/*
	test_ExReleaseReadWriteLock,                                 // 0x001C (28)
	test_ExSaveNonVolatileSetting,                               // 0x001D (29)
	test_ExSemaphoreObjectType,                                  // 0x001E (30)
	test_ExTimerObjectType,                                      // 0x001F (31)
	test_ExfInterlockedInsertHeadList,                           // 0x0020 (32)
	test_ExfInterlockedInsertTailList,                           // 0x0021 (33)
	test_ExfInterlockedRemoveHeadList,                           // 0x0022 (34)
	test_FscGetCacheSize,                                        // 0x0023 (35)
	test_FscInvalidateIdleBlocks,                                // 0x0024 (36)
	test_FscSetCacheSize,                                        // 0x0025 (37)
	test_HalClearSoftwareInterrupt,                              // 0x0026 (38)
	test_HalDisableSystemInterrupt,                              // 0x0027 (39)
	test_HalDiskCachePartitionCount,                             // 0x0028 (40) A.k.a. "IdexDiskPartitionPrefixBuffer"
	test_HalDiskModelNumber,                                     // 0x0029 (41)
	test_HalDiskSerialNumber,                                    // 0x002A (42)
 	*/
	test_HalEnableSystemInterrupt,                               // 0x002B (43)
	/*
	test_HalGetInterruptVector,                                  // 0x002C (44)
	test_HalReadSMBusValue,                                      // 0x002D (45)
	test_HalReadWritePCISpace,                                   // 0x002E (46)
	test_HalRegisterShutdownNotification,                        // 0x002F (47)
 	*/
	test_HalRequestSoftwareInterrupt,                            // 0x0030 (48)
	/*
	test_HalReturnToFirmware,                                    // 0x0031 (49)
	test_HalWriteSMBusValue,                                     // 0x0032 (50)
 	*/
	test_InterlockedCompareExchange,                             // 0x0033 (51)
	test_InterlockedDecrement,                                   // 0x0034 (52)
	test_InterlockedIncrement,                                   // 0x0035 (53)
	test_InterlockedExchange,                                    // 0x0036 (54)
	/*
	test_InterlockedExchangeAdd,                                 // 0x0037 (55)
	test_InterlockedFlushSList,                                  // 0x0038 (56)
	test_InterlockedPopEntrySList,                               // 0x0039 (57)
	test_InterlockedPushEntrySList,                              // 0x003A (58)
	test_IoAllocateIrp,                                          // 0x003B (59)
	test_IoBuildAsynchronousFsdRequest,                          // 0x003C (60)
	test_IoBuildDeviceIoControlRequest,                          // 0x003D (61)
	test_IoBuildSynchronousFsdRequest,                           // 0x003E (62)
	test_IoCheckShareAccess,                                     // 0x003F (63)
	test_IoCompletionObjectType,                                 // 0x0040 (64)
	test_IoCreateDevice,                                         // 0x0041 (65)
	test_IoCreateFile,                                           // 0x0042 (66)
	test_IoCreateSymbolicLink,                                   // 0x0043 (67)
	test_IoDeleteDevice,                                         // 0x0044 (68)
	test_IoDeleteSymbolicLink,                                   // 0x0045 (69)
	test_IoDeviceObjectType,                                     // 0x0046 (70)
	test_IoFileObjectType,                                       // 0x0047 (71)
	test_IoFreeIrp,                                              // 0x0048 (72)
	test_IoInitializeIrp,                                        // 0x0049 (73)
	test_IoInvalidDeviceRequest,                                 // 0x004A (74)
	test_IoQueryFileInformation,                                 // 0x004B (75)
	test_IoQueryVolumeInformation,                               // 0x004C (76)
	test_IoQueueThreadIrp,                                       // 0x004D (77)
	test_IoRemoveShareAccess,                                    // 0x004E (78)
	test_IoSetIoCompletion,                                      // 0x004F (79)
	test_IoSetShareAccess,                                       // 0x0050 (80)
	test_IoStartNextPacket,                                      // 0x0051 (81)
	test_IoStartNextPacketByKey,                                 // 0x0052 (82)
	test_IoStartPacket,                                          // 0x0053 (83)
	test_IoSynchronousDeviceIoControlRequest,                    // 0x0054 (84)
	test_IoSynchronousFsdRequest,                                // 0x0055 (85)
	test_IofCallDriver,                                          // 0x0056 (86)
	test_IofCompleteRequest,                                     // 0x0057 (87)
	test_KdDebuggerEnabled,                                      // 0x0058 (88)
	test_KdDebuggerNotPresent,                                   // 0x0059 (89)
	test_IoDismountVolume,                                       // 0x005A (90)
	test_IoDismountVolumeByName,                                 // 0x005B (91)
	test_KeAlertResumeThread,                                    // 0x005C (92)
	test_KeAlertThread,                                          // 0x005D (93)
	test_KeBoostPriorityThread,                                  // 0x005E (94)
 	*/
	test_KeBugCheck,                                             // 0x005F (95)
	test_KeBugCheckEx,                                           // 0x0060 (96)
	/*
	test_KeCancelTimer,                                          // 0x0061 (97)
	test_KeConnectInterrupt,                                     // 0x0062 (98)
 	*/
	test_KeDelayExecutionThread,                                 // 0x0063 (99)
 	/*
	test_KeDisconnectInterrupt,                                  // 0x0064 (100
	*/
	test_KeEnterCriticalRegion,                                  // 0x0065 (101)
	test_MmGlobalData,                                           // 0x0066 (102)
	test_KeGetCurrentIrql,                                       // 0x0067 (103)
	test_KeGetCurrentThread,                                     // 0x0068 (104)
	test_KeInitializeApc,                                        // 0x0069 (105)
	test_KeInitializeDeviceQueue,                                // 0x006A (106)
	test_KeInitializeDpc,                                        // 0x006B (107)
	test_KeInitializeEvent,                                      // 0x006C (108)
	/*
	test_KeInitializeInterrupt,                                  // 0x006D (109)
 	*/
	test_KeInitializeMutant,                                     // 0x006E (110)
	/*
	test_KeInitializeQueue,                                      // 0x006F (111)
 	*/
	test_KeInitializeSemaphore,                                  // 0x0070 (112)
	test_KeInitializeTimerEx,                                    // 0x0071 (113)
	/*
	test_KeInsertByKeyDeviceQueue,                               // 0x0072 (114)
	test_KeInsertDeviceQueue,                                    // 0x0073 (115)
	test_KeInsertHeadQueue,                                      // 0x0074 (116)
	test_KeInsertQueue,                                          // 0x0075 (117)
	test_KeInsertQueueApc,                                       // 0x0076 (118)
 	*/
	test_KeInsertQueueDpc,                                       // 0x0077 (119)
	test_KeInterruptTime,                                        // 0x0078 (120) (Set by ConnectWindowsTimersToThunkTable)
	/*
	test_KeIsExecutingDpc,                                       // 0x0079 (121)
 	*/
	test_KeLeaveCriticalRegion,                                  // 0x007A (122)
	/*
	test_KePulseEvent,                                           // 0x007B (123)
	test_KeQueryBasePriorityThread,                              // 0x007C (124)
 	*/
	test_KeQueryInterruptTime,                                   // 0x007D (125)
	test_KeQueryPerformanceCounter,                              // 0x007E (126)
	test_KeQueryPerformanceFrequency,                            // 0x007F (127)
	test_KeQuerySystemTime,                                      // 0x0080 (128)
	test_KeRaiseIrqlToDpcLevel,                                  // 0x0081 (129)
	/*
	test_KeRaiseIrqlToSynchLevel,                                // 0x0082 (130)
 	*/
	test_KeReleaseMutant,                                        // 0x0083 (131)
	/*
	test_KeReleaseSemaphore,                                     // 0x0084 (132)
	test_KeRemoveByKeyDeviceQueue,                               // 0x0085 (133)
	test_KeRemoveDeviceQueue,                                    // 0x0086 (134)
	test_KeRemoveEntryDeviceQueue,                               // 0x0087 (135)
	test_KeRemoveQueue,                                          // 0x0088 (136)
	test_KeRemoveQueueDpc,                                       // 0x0089 (137)
	test_KeResetEvent,                                           // 0x008A (138)
	test_KeRestoreFloatingPointState,                            // 0x008B (139)
 	*/
	test_KeResumeThread,                                         // 0x008C (140)
	/*
	test_KeRundownQueue,                                         // 0x008D (141)
	test_KeSaveFloatingPointState,                               // 0x008E (142)
	test_KeSetBasePriorityThread,                                // 0x008F (143)
	test_KeSetDisableBoostThread,                                // 0x0090 (144)
 	*/
	test_KeSetEvent,                                             // 0x0091 (145)
	/*
	test_KeSetEventBoostPriority,                                // 0x0092 (146)
	test_KeSetPriorityProcess,                                   // 0x0093 (147)
 	*/
	test_KeSetPriorityThread,                                    // 0x0094 (148)
	test_KeSetTimer,                                             // 0x0095 (149)
	test_KeSetTimerEx,                                           // 0x0096 (150)
	/*
	test_KeStallExecutionProcessor,                              // 0x0097 (151)
 	*/
	test_KeSuspendThread,                                        // 0x0098 (152)
	/*
	test_KeSynchronizeExecution,                                 // 0x0099 (153)
 	*/
	test_KeSystemTime,                                           // 0x009A (154) (Set by ConnectWindowsTimersToThunkTable)
	test_KeTestAlertThread,                                      // 0x009B (155)
	test_KeTickCount,                                            // 0x009C (156)
	test_KeTimeIncrement,                                        // 0x009D (157)
	/*
	test_KeWaitForMultipleObjects,                               // 0x009E (158)
 	*/
	test_KeWaitForSingleObject,                                  // 0x009F (159)
	test_KfRaiseIrql,                                            // 0x00A0 (160)
	test_KfLowerIrql,                                            // 0x00A1 (161)
	/*
	test_KiBugCheckData,                                         // 0x00A2 (162)
 	*/
	test_KiUnlockDispatcherDatabase,                             // 0x00A3 (163)
	test_LaunchDataPage,                                         // 0x00A4 (164)
	/*
	test_MmAllocateContiguousMemory,                             // 0x00A5 (165)
	test_MmAllocateContiguousMemoryEx,                           // 0x00A6 (166)
 	*/
	test_MmAllocateSystemMemory,                                 // 0x00A7 (167)
	/*
	test_MmClaimGpuInstanceMemory,                               // 0x00A8 (168)
 	*/
	test_MmCreateKernelStack,                                    // 0x00A9 (169)
	test_MmDeleteKernelStack,                                    // 0x00AA (170)
	/*
	test_MmFreeContiguousMemory,                                 // 0x00AB (171)
 	*/
	test_MmFreeSystemMemory,                                     // 0x00AC (172)
	/*
	test_MmGetPhysicalAddress,                                   // 0x00AD (173)
	test_MmIsAddressValid,                                       // 0x00AE (174)
	test_MmLockUnlockBufferPages,                                // 0x00AF (175)
	test_MmLockUnlockPhysicalPage,                               // 0x00B0 (176)
	test_MmMapIoSpace,                                           // 0x00B1 (177)
	test_MmPersistContiguousMemory,                              // 0x00B2 (178)
	test_MmQueryAddressProtect,                                  // 0x00B3 (179)
 	*/
	test_MmQueryAllocationSize,                                  // 0x00B4 (180)
	/*
	test_MmQueryStatistics,                                      // 0x00B5 (181)
	test_MmSetAddressProtect,                                    // 0x00B6 (182)
	test_MmUnmapIoSpace,                                         // 0x00B7 (183)
 	*/
	test_NtAllocateVirtualMemory,                                // 0x00B8 (184)
	/*
	test_NtCancelTimer,                                          // 0x00B9 (185)
	test_NtClearEvent,                                           // 0x00BA (186)
 	*/
	/* Can't use NtClose test because it is using filesystem testcase.
	test_NtClose,                                                // 0x00BB (187)
	*/
	/* Force disabled in case of test may be using filesystem usage.
	test_NtCreateDirectoryObject,                                // 0x00BC (188)
 	*/
	/*
	test_NtCreateEvent,                                          // 0x00BD (189)
	test_NtCreateFile,                                           // 0x00BE (190)
	test_NtCreateIoCompletion,                                   // 0x00BF (191)
 	*/
	test_NtCreateMutant,                                         // 0x00C0 (192)
	/*
	test_NtCreateSemaphore,                                      // 0x00C1 (193)
	test_NtCreateTimer,                                          // 0x00C2 (194)
	test_NtDeleteFile,                                           // 0x00C3 (195)
	test_NtDeviceIoControlFile,                                  // 0x00C4 (196)
	test_NtDuplicateObject,                                      // 0x00C5 (197)
	test_NtFlushBuffersFile,                                     // 0x00C6 (198)
 	*/
	test_NtFreeVirtualMemory,                                    // 0x00C7 (199)
	/* NOTE: Do not use below tests as they are filesystem relative.
	test_NtFsControlFile,                                        // 0x00C8 (200)
	test_NtOpenDirectoryObject,                                  // 0x00C9 (201)
	test_NtOpenFile,                                             // 0x00CA (202)
	test_NtOpenSymbolicLinkObject,                               // 0x00CB (203)
 	*/
	test_NtProtectVirtualMemory,                                 // 0x00CC (204)
	/*
	test_NtPulseEvent,                                           // 0x00CD (205)
	test_NtQueryDirectoryFile,                                   // 0x00CF (207)
	test_NtQueueApcThread,                                       // 0x00CE (206)
	test_NtQueryDirectoryObject,                                 // 0x00D0 (208)
	test_NtQueryEvent,                                           // 0x00D1 (209)
	test_NtQueryFullAttributesFile,                              // 0x00D2 (210)
	test_NtQueryInformationFile,                                 // 0x00D3 (211)
	test_NtQueryIoCompletion,                                    // 0x00D4 (212)
	test_NtQueryMutant,                                          // 0x00D5 (213)
	test_NtQuerySemaphore,                                       // 0x00D6 (214)
	test_NtQuerySymbolicLinkObject,                              // 0x00D7 (215)
	test_NtQueryTimer,                                           // 0x00D8 (216)
	test_NtQueryVirtualMemory,                                   // 0x00D9 (217)
	test_NtQueryVolumeInformationFile,                           // 0x00DA (218)
	test_NtReadFile,                                             // 0x00DB (219)
	test_NtReadFileScatter,                                      // 0x00DC (220)
 	*/
	test_NtReleaseMutant,                                        // 0x00DD (221)
	/*
	test_NtReleaseSemaphore,                                     // 0x00DE (222)
	test_NtRemoveIoCompletion,                                   // 0x00DF (223)
	test_NtResumeThread,                                         // 0x00E0 (224)
	test_NtSetEvent,                                             // 0x00E1 (225)
	test_NtSetInformationFile,                                   // 0x00E2 (226)
	test_NtSetIoCompletion,                                      // 0x00E3 (227)
	test_NtSetSystemTime,                                        // 0x00E4 (228)
	test_NtSetTimerEx,                                           // 0x00E5 (229)
	test_NtSignalAndWaitForSingleObjectEx,                       // 0x00E6 (230)
	test_NtSuspendThread,                                        // 0x00E7 (231)
	test_NtUserIoApcDispatcher,                                  // 0x00E8 (232)
 	*/
	test_NtWaitForSingleObject,                                  // 0x00E9 (233)
	test_NtWaitForSingleObjectEx,                                // 0x00EA (234)
	/*
	test_NtWaitForMultipleObjectsEx,                             // 0x00EB (235)
	test_NtWriteFile,                                            // 0x00EC (236)
	test_NtWriteFileGather,                                      // 0x00ED (237)
	test_NtYieldExecution,                                       // 0x00EE (238)
 	*/
	test_ObCreateObject,                                         // 0x00EF (239)
	test_ObDirectoryObjectType,                                  // 0x00F0 (240)
	test_ObInsertObject,                                         // 0x00F1 (241)
	/*
	test_ObMakeTemporaryObject,                                  // 0x00F2 (242)
 	*/
	test_ObOpenObjectByName,                                     // 0x00F3 (243)
	test_ObOpenObjectByPointer,                                  // 0x00F4 (244)
	test_ObpObjectHandleTable,                                   // 0x00F5 (245)
	test_ObReferenceObjectByHandle,                              // 0x00F6 (246)
	test_ObReferenceObjectByName,                                // 0x00F7 (247)
	test_ObReferenceObjectByPointer,                             // 0x00F8 (248)
	test_ObSymbolicLinkObjectType,                               // 0x00F9 (249)
	test_ObfDereferenceObject,                                   // 0x00FA (250)
	test_ObfReferenceObject,                                     // 0x00FB (251)
	/*
	test_PhyGetLinkState,                                        // 0x00FC (252)
	test_PhyInitialize,                                          // 0x00FD (253)
 	*/
	test_PsCreateSystemThread,                                   // 0x00FE (254)
	test_PsCreateSystemThreadEx,                                 // 0x00FF (255)
	/*
	test_PsQueryStatistics,                                      // 0x0100 (256)
	test_PsSetCreateThreadNotifyRoutine,                         // 0x0101 (257)
 	*/
	test_PsTerminateSystemThread,                                // 0x0102 (258)
	test_PsThreadObjectType,                                     // 0x0103 (259)
	/*
	test_RtlAnsiStringToUnicodeString,                           // 0x0104 (260)
	test_RtlAppendStringToString,                                // 0x0105 (261)
	test_RtlAppendUnicodeStringToString,                         // 0x0106 (262)
	test_RtlAppendUnicodeToString,                               // 0x0107 (263)
 	*/
	test_RtlAssert,                                              // 0x0108 (264)
	test_RtlCaptureContext,                                      // 0x0109 (265)
	/*
	test_RtlCaptureStackBackTrace,                               // 0x010A (266)
	test_RtlCharToInteger,                                       // 0x010B (267)
	test_RtlCompareMemory,                                       // 0x010C (268)
	test_RtlCompareMemoryUlong,                                  // 0x010D (269)
	test_RtlCompareString,                                       // 0x010E (270)
	test_RtlCompareUnicodeString,                                // 0x010F (271)
	test_RtlCopyString,                                          // 0x0110 (272)
	test_RtlCopyUnicodeString,                                   // 0x0111 (273)
	test_RtlCreateUnicodeString,                                 // 0x0112 (274)
	test_RtlDowncaseUnicodeChar,                                 // 0x0113 (275)
	test_RtlDowncaseUnicodeString,                               // 0x0114 (276)
 	*/
	test_RtlEnterCriticalSection,                                // 0x0115 (277)
	test_RtlEnterCriticalSectionAndRegion,                       // 0x0116 (278)
	test_RtlEqualString,                                         // 0x0117 (279)
	/*
	test_RtlEqualUnicodeString,                                  // 0x0118 (280)
	test_RtlExtendedIntegerMultiply,                             // 0x0119 (281)
	test_RtlExtendedLargeIntegerDivide,                          // 0x011A (282)
	test_RtlExtendedMagicDivide,                                 // 0x011B (283)
	test_RtlFillMemory,                                          // 0x011C (284)
 	*/
	test_RtlFillMemoryUlong,                                     // 0x011D (285)
	/*
	test_RtlFreeAnsiString,                                      // 0x011E (286)
	test_RtlFreeUnicodeString,                                   // 0x011F (287)
	test_RtlGetCallersAddress,                                   // 0x0120 (288)
	test_RtlInitAnsiString,                                      // 0x0121 (289)
	test_RtlInitUnicodeString,                                   // 0x0122 (290)
 	*/
	test_RtlInitializeCriticalSection,                           // 0x0123 (291)
	/*
	test_RtlIntegerToChar,                                       // 0x0124 (292)
	test_RtlIntegerToUnicodeString,                              // 0x0125 (293)
 	*/
	test_RtlLeaveCriticalSection,                                // 0x0126 (294)
	test_RtlLeaveCriticalSectionAndRegion,                       // 0x0127 (295)
	/*
	test_RtlLowerChar,                                           // 0x0128 (296)
	test_RtlMapGenericMask,                                      // 0x0129 (297)
	test_RtlMoveMemory,                                          // 0x012A (298)
	test_RtlMultiByteToUnicodeN,                                 // 0x012B (299)
	test_RtlMultiByteToUnicodeSize,                              // 0x012C (300)
 	*/
	test_RtlNtStatusToDosError,                                  // 0x012D (301)
	test_RtlRaiseException,                                      // 0x012E (302)
	test_RtlRaiseStatus,                                         // 0x012F (303)
	/*
	test_RtlTimeFieldsToTime,                                    // 0x0130 (304)
	test_RtlTimeToTimeFields,                                    // 0x0131 (305)
	test_RtlTryEnterCriticalSection,                             // 0x0132 (306)
	test_RtlUlongByteSwap,                                       // 0x0133 (307)
	test_RtlUnicodeStringToAnsiString,                           // 0x0134 (308)
	test_RtlUnicodeStringToInteger,                              // 0x0135 (309)
	test_RtlUnicodeToMultiByteN,                                 // 0x0136 (310)
	test_RtlUnicodeToMultiByteSize,                              // 0x0137 (311)
 	*/
	test_RtlUnwind,                                              // 0x0138 (312)
	/*
	test_RtlUpcaseUnicodeChar,                                   // 0x0139 (313)
	test_RtlUpcaseUnicodeString,                                 // 0x013A (314)
	test_RtlUpcaseUnicodeToMultiByteN,                           // 0x013B (315)
 	*/
	test_RtlUpperChar,                                           // 0x013C (316)
	/*
	test_RtlUpperString,                                         // 0x013D (317)
	test_RtlUshortByteSwap,                                      // 0x013E (318)
	test_RtlWalkFrameChain,                                      // 0x013F (319)
 	*/
	test_RtlZeroMemory,                                          // 0x0140 (320)
	test_XboxEEPROMKey,                                          // 0x0141 (321)
	test_XboxHardwareInfo,                                       // 0x0142 (322)
	/*
	test_XboxHDKey,                                              // 0x0143 (323)
	test_XboxKrnlVersion,                                        // 0x0144 (324)
	test_XboxSignatureKey,                                       // 0x0145 (325)
 	*/
	test_XeImageFileName,                                        // 0x0146 (326)
	test_XeLoadSection,                                          // 0x0147 (327)
	/*
	test_XeUnloadSection,                                        // 0x0148 (328)
	test_READ_PORT_BUFFER_UCHAR,                                 // 0x0149 (329)
	test_READ_PORT_BUFFER_USHORT,                                // 0x014A (330)
	test_READ_PORT_BUFFER_ULONG,                                 // 0x014B (331)
	test_WRITE_PORT_BUFFER_UCHAR,                                // 0x014C (332)
	test_WRITE_PORT_BUFFER_USHORT,                               // 0x014D (333)
	test_WRITE_PORT_BUFFER_ULONG,                                // 0x014E (334)
 	*/
	test_XcSHAInit,                                              // 0x014F (335)
	test_XcSHAUpdate,                                            // 0x0150 (336)
	test_XcSHAFinal,                                             // 0x0151 (337)
	/*
	test_XcRC4Key,                                               // 0x0152 (338)
	test_XcRC4Crypt,                                             // 0x0153 (339)
	test_XcHMAC,                                                 // 0x0154 (340)
	test_XcPKEncPublic,                                          // 0x0155 (341)
	test_XcPKDecPrivate,                                         // 0x0156 (342)
	test_XcPKGetKeyLen,                                          // 0x0157 (343)
	test_XcVerifyPKCS1Signature,                                 // 0x0158 (344)
	test_XcModExp,                                               // 0x0159 (345)
	test_XcDESKeyParity,                                         // 0x015A (346)
	test_XcKeyTable,                                             // 0x015B (347)
	test_XcBlockCrypt,                                           // 0x015C (348)
	test_XcBlockCryptCBC,                                        // 0x015D (349)
	test_XcCryptService,                                         // 0x015E (350)
 	*/
	test_XcUpdateCrypto,                                         // 0x015F (351)
	test_RtlRip,                                                 // 0x0160 (352)
	/*
	test_XboxLANKey,                                             // 0x0161 (353)
	test_XboxAlternateSignatureKeys,                             // 0x0162 (354)
	test_XePublicKeyData,                                        // 0x0163 (355)
	test_HalBootSMCVideoMode,                                    // 0x0164 (356)
	test_IdexChannelObject,                                      // 0x0165 (357)
	test_HalIsResetOrShutdownPending,                            // 0x0166 (358)
	test_IoMarkIrpMustComplete,                                  // 0x0167 (359)
	test_HalInitiateShutdown,                                    // 0x0168 (360)
	test__snprintf,                                              // 0x0169 (361)
	test__sprintf,                                        		 // 0x016A (362)
	test__vsnprintf,                                             // 0x016B (363)
	test__vsprintf,                                              // 0x016C (364)
	test_HalEnableSecureTrayEject,                               // 0x016D (365)
	test_HalWriteSMCScratchRegister,                             // 0x016E (366)
	test_UnknownAPI367,                                          // 0x016F (367)
	test_UnknownAPI368,                                          // 0x0170 (368)
	test_UnknownAPI369,                                          // 0x0171 (369)
	test_XProfpControl,                                          // 0x0172 (370) PROFILING
	test_XProfpGetData,                       					 // 0x0173 (371) PROFILING
	test_IrtClientInitFast,                                      // 0x0174 (372) PROFILING
	test_IrtSweep,                                               // 0x0175 (373) PROFILING
	test_MmDbgAllocateMemory,                                    // 0x0177 (374) DEVKIT
	test_MmDbgFreeMemory,                                        // 0x0178 (375) DEVKIT - Returns number of pages released.
	test_MmDbgQueryAvailablePages,                               // 0x0179 (376) DEVKIT
	test_MmDbgReleaseAddress,                                    // 0x017A (377) DEVKIT
	test_MmDbgWriteCheck,                                        // 0x017A (378) DEVKIT
	*/
};
