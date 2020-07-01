#include "FueledLampBase.h"
#include "FGActorRepresentationManager.h"
#include "Components/SpotLightComponent.h"
#include "UnrealNetwork.h"

AFueledLampBase::AFueledLampBase() {
	PowerConnectionProduction = CreateDefaultSubobject<UFGPowerConnectionComponent>("PowerConnectionProduction");
	PowerConnectionProduction->SetupAttachment(RootComponent);

	PowerConnectionConsumption = CreateDefaultSubobject<UFGPowerConnectionComponent>("PowerConnectionConsumption");
	PowerConnectionConsumption->SetupAttachment(RootComponent);
}

void AFueledLampBase::BeginPlay() {
	Super::BeginPlay();

	if (HasAuthority()) {
		PowerConnectionProduction->SetPowerInfo(GetPowerInfo());
		PowerConnectionConsumption->SetPowerInfo(GetPowerInfo());
		PowerConnectionConsumption->AddHiddenConnection(PowerConnectionProduction);
	}
}

void AFueledLampBase::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction) {
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	TArray<UActorComponent*> lights = GetComponentsByClass(ULightComponent::StaticClass());
	bool bLampOn = HasPower() && !IsProductionPaused();
	for (auto& light : lights) {
		Cast<ULightComponent>(light)->SetVisibilitySML(bLampOn, true);
	}
}

void AFueledLampBase::Factory_Tick(float dt) {
	if (HasAuthority())	GetPowerInfo()->SetTargetConsumption(GetPendingPotential() * mPowerProduction / 4.1);
	Super::Factory_Tick(dt);
}