#include "../Public/KuwaharaFilter.h"

IMPLEMENT_SHADER_TYPE(, FCombineShaderPS, TEXT("/MyCustomModule/KuwaharaFilter.usf"), TEXT("CombineMainPS"), SF_Pixel);
IMPLEMENT_SHADER_TYPE(, FUVMaskShaderPS, TEXT("/MyCustomModule/KuwaharaFilter.usf"), TEXT("UVMaskMainPS"), SF_Pixel);