#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FMyCustomModule : public IModuleInterface {
public:
	virtual void StartupModule() override;
};