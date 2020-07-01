#pragma once

#include "CoreMinimal.h"
#include "Buildables/FGBuildable.h"
#include "FGPowerConnectionComponent.h"
#include "FGPowerInfoComponent.h"
#include "LampSubsystem.h"
#include "LampBase.generated.h"

#define DefaultLampGroup TEXT("Default")

class ALampBase;

UCLASS()
class LIGHTITUP_API ULampBaseRCO : public UFGRemoteCallObject {
	GENERATED_BODY()

private:
	UPROPERTY(Replicated)
		bool bTest;

public:
	UFUNCTION(Server, WithValidation, Reliable)
		void SetGroup(ALampBase* lamp, const FString& group);

	UFUNCTION(Server, WithValidation, Reliable)
		void SetMode(ALampBase* lamp, ELampMode mode);

	UFUNCTION(Server, WithValidation, Reliable)
		void SetUseGroup(ALampBase* lamp, bool useGroup);
};

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
	bool bLastShouldBeOn = false;

	bool shouldLampBeOn(ELampMode mode);
	
public:
	UPROPERTY(EditAnywhere)
		UFGPowerConnectionComponent* mPowerConnection;

	UPROPERTY()
		UFGPowerInfoComponent* mPowerInfo = nullptr;

	UPROPERTY(BlueprintReadWrite, Replicated, SaveGame, Category = "Light")
		TEnumAsByte<ELampMode> mMode = OFF;

	UPROPERTY(BlueprintReadWrite, Replicated, SaveGame, Category = "Light")
		FString mGroup = DefaultLampGroup;

	UPROPERTY(BlueprintReadWrite, Replicated, SaveGame, Category = "Light")
		bool mUseGroup = false;

	UPROPERTY(EditDefaultsOnly, Category = "Light")
		float mPowerConsumption = 10.0;

	UPROPERTY(EditDefaultsOnly, Category = "Light")
		float mEmessiveStrength = 1.0;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Light")
		bool bLampOn = false;

	ALampBase();

	// Begin UObject
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// End UObject

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

	UFUNCTION(BlueprintCallable, Category = "Lamps")
	void SetGroup(const FString& group);

	UFUNCTION(BlueprintCallable, Category = "Lamps")
	void SetMode(TEnumAsByte<ELampMode> mode);

	UFUNCTION(BlueprintCallable, Category = "Lamps")
	void SetUseGroup(bool useGroup);

	// Begin FicsIt-Networks support
	UFUNCTION()
	void netFunc_setMode(int mode);

	UFUNCTION()
	int netFunc_getMode();

	UFUNCTION()
	void netFunc_setGroup(const FString& group);

	UFUNCTION()
	FString netFunc_getGroup();

	UFUNCTION()
	void netFunc_setUseGroup(bool useGroup);

	UFUNCTION()
	bool netFunc_getUseGroup();

	UFUNCTION()
	void netFunc_setGroupMode(const FString& group, int mode, bool create);

	UFUNCTION()
	int netFunc_getGroupMode(const FString& group);

	UFUNCTION()
	void netFunc_removeGroup(const FString& group);
	// End FicsIt-Networks support
};
