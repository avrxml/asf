// implemented interface(s)
#ifndef PDS_USE_EXTERNAL_FLASH
#define S_Nv_ItemInit S_Nv_ItemInit_Impl
#define S_Nv_Write S_Nv_Write_Impl
#define S_Nv_ItemLength S_Nv_ItemLength_Impl
#define S_Nv_IsItemAvailable S_Nv_IsItemAvailable_Impl
#define S_Nv_Read S_Nv_Read_Impl
#define S_Nv_Delete S_Nv_Delete_Impl
#define S_Nv_EraseAll S_Nv_EraseAll_Impl
#else

#define S_Nv_ItemInit S_Nv_ItemInit_Impl
#define S_Nv_Write S_Nv_Write_Impl
#define S_Nv_ItemLength S_Nv_ItemLength_Impl
#define S_Nv_IsItemAvailable S_Nv_IsItemAvailable_Impl
#define S_Nv_Read S_Nv_Read_Impl
#define S_Nv_Delete S_Nv_Delete_Impl
#define S_Nv_EraseAll S_Nv_EraseAll_Impl

#define S_Nv_Init S_XNv_Init
#define D_Nv_Init D_XNv_Init

#endif
