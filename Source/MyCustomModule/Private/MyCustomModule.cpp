#include "../Public/MyCustomModule.h"

void FMyCustomModule::StartupModule()
{
	FString BaseDir = FPaths::Combine(FPaths::GameSourceDir(), TEXT("MyCustomModule"));
	FString ModuleShaderDir = FPaths::Combine(BaseDir, TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/MyCustomModule"), ModuleShaderDir);
}

IMPLEMENT_MODULE(FMyCustomModule, MyCustomModule)