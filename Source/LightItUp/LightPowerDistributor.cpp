#include "LightPowerDistributor.h"

ALightPowerDistributor::ALightPowerDistributor() {
	PowerConnection = CreateDefaultSubobject<UFGPowerConnectionComponent>(TEXT("PowerConnection"));
	PowerConnection->AttachTo(RootComponent);
	PowerConnection->bEditableWhenInherited = true;
	PowerInfo = CreateDefaultSubobject<UFGPowerInfoComponent>(TEXT("PowerInfo"));
	PowerConnection->SetPowerInfo(PowerInfo);

	LightPowerRange = CreateDefaultSubobject<USphereComponent>(TEXT("LightPowerRange"));
	LightPowerRange->AttachTo(RootComponent);

	mFactoryTickFunction.SetTickFunctionEnable(true);
}

void ALightPowerDistributor::BeginPlay() {
	TArray<AActor*> actors;
	TArray<TEnumAsByte<EObjectTypeQuery>> channels;
	channels.Add(TEnumAsByte<EObjectTypeQuery>(ECollisionChannel::ECC_WorldStatic));
	channels.Add(TEnumAsByte<EObjectTypeQuery>(ECollisionChannel::ECC_WorldDynamic));
	TArray<AActor*> ignore;
	ignore.Add(this);
	UKismetSystemLibrary::SphereOverlapActors(this, this->GetActorLocation(), LightPowerRange->GetScaledSphereRadius(), channels, nullptr, ignore, actors);
	for (auto& actor : actors) {
		auto connector = Cast<ULightPowerConnector>(actor->GetComponentByClass(ULightPowerConnector::StaticClass()));
		if (!connector) continue;
		if (connector->mDistributor) {
			float oldSize = (connector->mDistributor->GetActorLocation() - connector->GetComponentLocation()).Size();
			float newSize = (GetActorLocation() - connector->GetComponentLocation()).Size();
			if (newSize >= oldSize) continue;
		}
		registerConnector(connector);
	}
	Super::BeginPlay();
}

void ALightPowerDistributor::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	for (auto conn : mConnectors) unregisterConnector(conn);
	Super::EndPlay(EndPlayReason);
}

void ALightPowerDistributor::registerConnector(ULightPowerConnector * connector) {
	if (!connector) return;
	if (mConnectors.Contains(connector)) return;
	mConnectors.Add(connector);
	connector->registerDistributor(this);
}

void ALightPowerDistributor::unregisterConnector(ULightPowerConnector * connector) {
	if (!mConnectors.Remove(connector)) return;
	connector->unregisterDistributor();
}
