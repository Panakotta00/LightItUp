#include "InitGameInstance.h"

#include "LampSubsystem.h"

UInitGameInstanceLightItUp::UInitGameInstanceLightItUp() {
	ModSubsystems.Add(ULampSubsystemHolder::StaticClass());
}
