#pragma once

#include "CoreMinimal.h"
#include "FGBuildable.h"
#include "FGPowerConnectionComponent.h"
#include "FGPowerInfoComponent.h"
#include "Components/SphereComponent.h"
#include "LightPowerConnector.h"
#include "LightPowerDistributor.generated.h"

/**
 * Converts and distributes light power from normal power.
 */
UCLASS()
class LIGHTITUP_API ALightPowerDistributor : public AFGBuildable {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		UFGPowerConnectionComponent* PowerConnection;

	UPROPERTY(EditAnywhere)
		UFGPowerInfoComponent* PowerInfo;

	UPROPERTY(EditAnywhere)
		USphereComponent* LightPowerRange;

	UPROPERTY()
		TArray<ULightPowerConnector*> mConnectors;

	ALightPowerDistributor();

	// Begin Actor Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// End Actor Interface

	void registerConnector(ULightPowerConnector* connector);
	void unregisterConnector(ULightPowerConnector* connector);
};
