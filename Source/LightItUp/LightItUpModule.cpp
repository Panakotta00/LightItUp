#include "LightItUpModule.h"

#include "FGGameMode.h"
#include "FGGameState.h"
#include "FGPlayerController.h"
#include "LampSubsystem.h"
#include "LampBase.h"
#include "NativeHookManager.h"

void GameModePostLogin(CallScope<void(*)(AFGGameMode*, APlayerController*)>& scope, AFGGameMode* gm, APlayerController * pc) {
	if (gm->HasAuthority() && !gm->IsMainMenuGameMode()) {
		gm->RegisterRemoteCallObjectClass(ULampSubsystemRCO::StaticClass());
		gm->RegisterRemoteCallObjectClass(ULampBaseRCO::StaticClass());
	}
}

void FLightItUpModule::StartupModule() {
	SUBSCRIBE_METHOD(AFGGameMode::PostLogin, &GameModePostLogin)
}

IMPLEMENT_GAME_MODULE(FLightItUpModule, LightItUp);
