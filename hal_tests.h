#ifndef HAL_TESTS_H
#define HAL_TESTS_H

void test_HalReadSMCTrayState();
void test_HalClearSoftwareInterrupt();
void test_HalDisableSystemInterrupt();
void test_HalDiskCachePartitionCount();
void test_HalDiskModelNumber();
void test_HalDiskSerialNumber();
void test_HalEnableSystemInterrupt();
void test_HalGetInterruptVector();
void test_HalReadSMBusValue();
void test_HalReadWritePCISpace();
void test_HalRegisterShutdownNotification();
void test_HalRequestSoftwareInterrupt();
void test_HalReturnToFirmware();
void test_HalWriteSMBusValue();
void test_HalBootSMCVideoMode();
void test_HalIsResetOrShutdownPending();
void test_HalInitiateShutdown();
void test_HalEnableSecureTrayEject();
void test_HalWriteSMCScratchRegister();


#endif
