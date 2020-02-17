#pragma once

#include "Modules/ModuleManager.h"

class FLightItUpModule : public FDefaultGameModuleImpl {
public:
	virtual void StartupModule() override;

	virtual bool IsGameModule() const override { return true; }
};