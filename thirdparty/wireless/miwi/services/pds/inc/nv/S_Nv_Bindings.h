// implemented interface(s)
#define S_Nv_ItemInit S_Nv_ItemInit_Impl
#define S_Nv_Write S_Nv_Write_Impl
#define S_Nv_Read S_Nv_Read_Impl
#define S_Nv_EraseAll S_Nv_EraseAll_Impl
#define S_Nv_SetPowerSupplyCheckingFunction S_Nv_SetPowerSupplyCheckingFunction_Impl
#define S_Nv_ItemLength S_Nv_ItemLength_Impl
#define S_Nv_Delete S_Nv_Delete_Impl
#define S_Nv_SetPowerSupplyCheckingFunction S_Nv_SetPowerSupplyCheckingFunction_Impl
#define S_Nv_IsItemAvailable S_Nv_IsItemAvailable_Impl

// used interfaces
#if defined(TESTHARNESS)
// ...bind to stubs...
#define D_Nv_Write D_Nv_Stub_Write
#define D_Nv_Read D_Nv_Stub_Read
#define D_Nv_EraseSector D_Nv_Stub_EraseSector
#define D_Nv_IsEmpty D_Nv_Stub_IsEmpty
#define D_Nv_IsEqual D_Nv_Stub_IsEqual

// Use actual timers
#define N_Timer_Stop N_Timer_Stop_Impl
#define N_Timer_Start16 N_Timer_Start16_Impl
#define N_Timer_IsRunning N_Timer_IsRunning_Impl

#else
// ...bind to implementation...
#define D_Nv_Write D_Nv_Write_Impl
#define D_Nv_Read D_Nv_Read_Impl
#define D_Nv_EraseSector D_Nv_EraseSector_Impl
#define D_Nv_IsEmpty D_Nv_IsEmpty_Impl
#define D_Nv_IsEqual D_Nv_IsEqual_Impl

#define N_Timer_Stop N_Timer_Stop_Impl
#define N_Timer_Start16 N_Timer_Start16_Impl
#define N_Timer_IsRunning N_Timer_IsRunning_Impl

#endif
#define S_XNv_SetPowerSupplyCheckingFunction S_XNv_SetPowerSupplyCheckingFunction_Impl
