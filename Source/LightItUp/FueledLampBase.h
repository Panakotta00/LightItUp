#pragma once

#include "CoreMinimal.h"
#include "FGBuildableGeneratorFuel.h"
#include "FGPowerConnectionComponent.h"
#include "FGPowerInfoComponent.h"
#include "FueledLampBase.generated.h"

UCLASS(Blueprintable)
class AFueledLampBase : public AFGBuildableGeneratorFuel {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Light")
		UFGPowerConnectionComponent* PowerConnectionProduction;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Light")
		UFGPowerConnectionComponent* PowerConnectionConsumption;

	AFueledLampBase();

	// Begin AActor
	virtual void BeginPlay() override;
	virtual void TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;
	// End AActor

	// Begin Buildable Interface
	void Factory_Tick(float dt) override;
	// End Buildable Interface
};