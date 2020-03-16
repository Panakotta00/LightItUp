#include "LightItUpModule.h"

#include "FGGameMode.h"
#include "FGGameState.h"
#include "FGPlayerController.h"
#include "LampSubsystem.h"
#include "LampBase.h"

#include "SML/mod/hooking.h"
#include "SML/util/Logging.h"

void PlayerBeginPlay(CallScope<void(*)(AFGPlayerController*)>& scope, AFGPlayerController* pc) {
	
}

void GameStateInit(CallScope<void(*)(AFGGameState*)>& scope, AFGGameState* gs) {
	scope(gs);

	gs->GetWorld()->SpawnActor<ALampSubsystem>();
}

void GameModePostLogin(CallScope<void(*)(AFGGameMode*, APlayerController*)>& scope, AFGGameMode* gm, APlayerController * pc) {
	if (gm->HasAuthority() && !gm->IsMainMenuGameMode()) {
		gm->RegisterRemoteCallObjectClass(ULampSubsystemRCO::StaticClass());
		gm->RegisterRemoteCallObjectClass(ULampBaseRCO::StaticClass());
	}
}

void FLightItUpModule::StartupModule() {
	SUBSCRIBE_METHOD("?BeginPlay@AFGPlayerController@@UEAAXXZ", AFGPlayerController::BeginPlay, &PlayerBeginPlay)
	SUBSCRIBE_METHOD("?Init@AFGGameState@@UEAAXXZ", AFGGameState::Init, &GameStateInit)
	SUBSCRIBE_METHOD("?PostLogin@AFGGameMode@@UEAAXPEAVAPlayerController@@@Z", AFGGameMode::PostLogin, &GameModePostLogin)
}

IMPLEMENT_GAME_MODULE(FLightItUpModule, LightItUp);
