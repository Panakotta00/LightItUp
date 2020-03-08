#pragma once

#include "CoreMinimal.h"
#include "Buildables/FGBuildable.h"
#include "FGPowerConnectionComponent.h"
#include "FGPowerInfoComponent.h"
#include "LampSubsystem.h"
#include "LampBase.generated.h"

#define DefaultLampGroup TEXT("Default")

/**
 * The Base Class for normal lamp buildings consuming power
 */
UCLASS()
class LIGHTITUP_API ALampBase : public AFGBuildable
{
	GENERATED_BODY()

private:
	ALampSubsystem* lampSubSys;

	UPROPERTY()
		uint8 mLastLampOn = 2;

	bool shouldLampBeOn(ELampMode mode);
	
public:
	UPROPERTY(EditAnywhere)
		UFGPowerConnectionComponent* mPowerConnection;

	UPROPERTY()
		UFGPowerInfoComponent* mPowerInfo = nullptr;

	UPROPERTY(BlueprintReadWrite, SaveGame, Category = "Light")
		TEnumAsByte<ELampMode> mMode = OFF;

	UPROPERTY(BlueprintReadWrite, SaveGame, Category = "Light")
		FString mGroup = DefaultLampGroup;

	UPROPERTY(BlueprintReadWrite, SaveGame, Category = "Light")
		bool mUseGroup = false;

	UPROPERTY(EditDefaultsOnly, Category = "Light")
		float mPowerConsumption = 10.0;

	UPROPERTY(EditDefaultsOnly, Category = "Light")
		float mEmessiveStrength = 1.0;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Light")
		bool mLampOn = false;

	ALampBase();

	// Begin Actor Interface
	virtual void BeginPlay() override;
	virtual void TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;
	// End Actor Interface

	// Begin Buildable Interface
	void Factory_Tick(float dt) override;
	// End Buildable Interface

	// Begin IFGSaveInterface
	bool ShouldSave_Implementation() const override;
	// End IFGSaveInterface
};
