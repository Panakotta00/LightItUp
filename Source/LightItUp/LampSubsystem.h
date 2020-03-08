// ILikeBanas

#pragma once

#include "CoreMinimal.h"
#include "FGSubsystem.h"
#include "FGSaveInterface.h"
#include "LampSubsystem.generated.h"

#define DefaultLampGroup TEXT("Default")

UENUM()
enum ELampMode {
	OFF,
	ON,
	AUTO,
};

/**
 * Controlls the Lamps in the world.
 * Espetially the group settings.
 */
UCLASS()
class LIGHTITUP_API ALampSubsystem : public AFGSubsystem, public IFGSaveInterface
{
	GENERATED_BODY()
	
private:
	UPROPERTY(SaveGame)
		TMap<FString, TEnumAsByte<ELampMode>> groups;

public:
	// Begin IFGSaveInterface
	bool ShouldSave_Implementation() const override;
	// End IFGSaveInterface

	ALampSubsystem();

	/** Get the lamp subsystem in the current world, can be nullptr, e.g. on game ending (destroy) or game startup. */
	static ALampSubsystem* Get(UWorld* world);

	/** Get the lamp subsystem in the current world, can be nullptr, e.g. on game ending (destroy) or game startup. */
	UFUNCTION(BlueprintPure, Category = "Light", DisplayName = "GetLampSubsystem", meta = (DefaultToSelf = "WorldContext"))
		static ALampSubsystem* Get(UObject* worldContext);

	UFUNCTION(BlueprintCallable, Category = "Light")
		bool setGroup(FString group, ELampMode mode, bool create = false);

	UFUNCTION(BlueprintCallable, Category = "Light")
		ELampMode getGroup(FString group, bool& found);

	UFUNCTION(BlueprintCallable, Category = "Light")
		bool removeGroup(FString group);

	UFUNCTION(BlueprintCallable, Category = "Light")
		void getGroups(TArray<FString>& groupNames);
	
	UFUNCTION(BlueprintCallable)
		static void Log(FString logStr);
};
