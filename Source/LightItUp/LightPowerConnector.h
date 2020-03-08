// ILikeBanas

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "FGPowerConnectionComponent.h"
#include "LightPowerConnector.generated.h"

class ALightPowerDistributor;

/**
 * Connects to the nearest power distributor
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class LIGHTITUP_API ULightPowerConnector : public USceneComponent {
	GENERATED_BODY()

public:
	UPROPERTY()
		ALightPowerDistributor* mDistributor;

	UPROPERTY()
		UFGPowerConnectionComponent* mPowerConnection;

	// Begin ActorComponent Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// End ActorComponent Interface

	void registerDistributor(ALightPowerDistributor* distributor);
	void unregisterDistributor();
};
