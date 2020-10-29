// ILikeBanas

#pragma once

#include "CoreMinimal.h"
#include "FGSubsystem.h"
#include "FGSaveInterface.h"
#include "FGRemoteCallObject.h"
#include "mod/ModSubsystems.h"

#include "LampSubsystem.generated.h"

#define DefaultLampGroup TEXT("Default")

UENUM(BlueprintType)
enum ELampMode {
	OFF,
	ON,
	AUTO,
};

class ALampSubsystem;

UCLASS(BlueprintType)
class LIGHTITUP_API ULampSubsystemRCO : public UFGRemoteCallObject {
	GENERATED_BODY()

public:
	ULampSubsystemRCO();

	UPROPERTY(Replicated)
		int32 Test;

	UFUNCTION(Server, WithValidation, Reliable)
		void SetGroup(ALampSubsystem* subsys, const FString& group, ELampMode mode, bool create);

	UFUNCTION(Server, WithValidation, Reliable)
		void RemoveGroup(ALampSubsystem* subsys, const FString& group);

	UFUNCTION(BlueprintCallable, Server, WithValidation, Reliable, Category="Light")
        void PickupPortableLamp(AActor* portableLamp, AFGPlayerController* Player);

	UFUNCTION(BlueprintCallable, Server, WithValidation, Reliable, Category="Light")
        void PlacePortableLamp(AFGPlayerController* Player);
};

USTRUCT()
struct FLampGroup {
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	FString Name;

	UPROPERTY(SaveGame)
	TEnumAsByte<ELampMode> Mode;

	bool operator==(const FLampGroup& group) const {
		return Name == group.Name;
	}

	bool operator==(const FString& Key) const {
		return Name == Key;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGroupUpdate, bool, Hard);

/**
 * Controlls the Lamps in the world.
 * Espetially the group settings.
 */
UCLASS()
class LIGHTITUP_API ALampSubsystem : public AFGSubsystem, public IFGSaveInterface
{
	GENERATED_BODY()

	friend ULampSubsystemRCO;
private:
	UPROPERTY(SaveGame, ReplicatedUsing=OnGroupsChanged)
		TArray<FLampGroup> Groups;

	UPROPERTY()
		int32 LastGroupCount = -1;

	UFUNCTION()
		void OnGroupsChanged();

public:
	UPROPERTY(BlueprintAssignable, Category ="Light")
		FGroupUpdate OnGroupChanged;

	ALampSubsystem();

	// Begin IFGSaveInterface
	bool ShouldSave_Implementation() const override;
	// End IFGSaveInterface

	// Begin UObject
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// End UObject

	// Begin AActor
	virtual void BeginPlay() override;
	// End AActor

	/** Get the lamp subsystem in the current world, can be nullptr, e.g. on game ending (destroy) or game startup. */
	static ALampSubsystem* Get(UWorld* world);

	/** Get the lamp subsystem in the current world, can be nullptr, e.g. on game ending (destroy) or game startup. */
	UFUNCTION(BlueprintPure, Category = "Light", DisplayName = "GetLampSubsystem", meta = (DefaultToSelf = "WorldContext"))
		static ALampSubsystem* Get(UObject* worldContext);

	UFUNCTION(BlueprintCallable, Category = "Light")
		void setGroup(const FString& group, ELampMode mode, bool create = false);

	UFUNCTION(BlueprintCallable, Category = "Light")
		ELampMode getGroup(const FString& group, bool& found);

	UFUNCTION(BlueprintCallable, Category = "Light")
		void removeGroup(const FString& group);

	UFUNCTION(BlueprintCallable, Category = "Light")
		void getGroups(TArray<FString>& groupNames);
};

UCLASS()
class ULampSubsystemHolder : public UModSubsystemHolder {
	GENERATED_BODY()

private:
	UPROPERTY()
	ALampSubsystem* LampSubsystem = nullptr;
	
public:
	virtual void InitSubsystems() override;
};
