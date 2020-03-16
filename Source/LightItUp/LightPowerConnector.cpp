#include "LightPowerConnector.h"
#include "LightPowerDistributor.h"

#include "Kismet/KismetSystemLibrary.h"

void ULightPowerConnector::BeginPlay() {
	auto owner = GetOwner();
	TArray<AActor*> actors;
	TArray<TEnumAsByte<EObjectTypeQuery>> channels;
	channels.Add(TEnumAsByte<EObjectTypeQuery>(ECollisionChannel::ECC_WorldStatic));
	channels.Add(TEnumAsByte<EObjectTypeQuery>(ECollisionChannel::ECC_WorldDynamic));
	TArray<AActor*> ignore;
	ignore.Add(GetOwner());
	UKismetSystemLibrary::SphereOverlapActors(owner, GetComponentLocation(), 1.0f, channels, nullptr, ignore, actors);
	ALightPowerDistributor* dist = nullptr;
	float distance = -1.0;
	for (auto& actor : actors) {
		auto distributor = Cast<ALightPowerDistributor>(actor);
		if (!distributor) continue;
		float size;
		if (distance < 0.0f || (size = (distributor->GetActorLocation() - GetComponentLocation()).Size()) < distance) {
			dist = distributor;
			distance = size;
		}
	}
	registerDistributor(dist);
	Super::BeginPlay();
}

void ULightPowerConnector::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	unregisterDistributor();
	Super::EndPlay(EndPlayReason);
}

void ULightPowerConnector::registerDistributor(ALightPowerDistributor * distributor) {
	if (!distributor) return;
	if (mDistributor == distributor) return;
	if (mDistributor) unregisterDistributor();
	mDistributor = distributor;
	distributor->registerConnector(this);
	mPowerConnection = Cast<UFGPowerConnectionComponent>(GetOwner()->GetComponentByClass(UFGPowerConnectionComponent::StaticClass()));
	mPowerConnection->AddHiddenConnection(distributor->PowerConnection);
}

void ULightPowerConnector::unregisterDistributor() {
	if (!mDistributor) return;
	if (mPowerConnection) {
		TArray<UFGCircuitConnectionComponent*> connections;
		mPowerConnection->GetHiddenConnections(connections);
		if (connections.Contains(mDistributor->PowerConnection)) mPowerConnection->RemoveHiddenConnection(mDistributor->PowerConnection);
		mPowerConnection = nullptr;
	}
	auto dist = mDistributor;
	mDistributor = nullptr;
	dist->unregisterConnector(this);
}
