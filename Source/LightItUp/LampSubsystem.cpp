#include "LampSubsystem.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "util/Logging.h"
#include "FGPlayerController.h"
#include "FGGameMode.h"
#include "UnrealNetwork.h"

ULampSubsystemRCO::ULampSubsystemRCO() {
	
}

void ULampSubsystemRCO::SetGroup_Implementation(ALampSubsystem* subsys, const FString& group, ELampMode mode, bool create) {
	if (group.Len() < 1) return;
	if (auto g = subsys->Groups.FindByKey(group)) {
		g->Mode = mode;
		subsys->ForceNetUpdate();
		subsys->OnGroupsChanged();
	} else if (create) {
		subsys->Groups.Add({group, mode});
		subsys->ForceNetUpdate();
		subsys->OnGroupsChanged();
	}
}

bool ULampSubsystemRCO::SetGroup_Validate(ALampSubsystem* subsys, const FString& group, ELampMode mode, bool create) {
	return true;
}

void ULampSubsystemRCO::RemoveGroup_Implementation(ALampSubsystem* subsys, const FString& groupName) {
	if (groupName == DefaultLampGroup || groupName.Len() < 1) return;
	auto group = subsys->Groups.FindByKey(groupName);
	subsys->Groups.Remove(*group);
	subsys->ForceNetUpdate();
	subsys->OnGroupsChanged();
}

bool ULampSubsystemRCO::RemoveGroup_Validate(ALampSubsystem* subsys, const FString& group) {
	return true;
}

void ULampSubsystemRCO::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ULampSubsystemRCO, Test);
}


bool ALampSubsystem::ShouldSave_Implementation() const {
	return true;
}

void ALampSubsystem::OnGroupsChanged() {
	bool hard = Groups.Num() != LastGroupCount;
	if (hard) LastGroupCount = Groups.Num();
	OnGroupChanged.Broadcast(hard);
}

ALampSubsystem::ALampSubsystem() {
	bReplicates = true;
}

void ALampSubsystem::BeginPlay() {
	Super::BeginPlay();
	if (!Groups.Contains(FString(DefaultLampGroup))) Groups.Add({DefaultLampGroup, AUTO});
	if (FLampGroup* group = Groups.FindByKey(TEXT(""))) Groups.Remove(*group);
}

ALampSubsystem * ALampSubsystem::Get(UWorld* world) {
	TArray<AActor*> subsystems;
	UGameplayStatics::GetAllActorsOfClass(world, ALampSubsystem::StaticClass(), subsystems);
	if (subsystems.Num() > 0) return (ALampSubsystem*) subsystems[0];
	return nullptr;
}

ALampSubsystem * ALampSubsystem::Get(UObject * worldContext) {
	return Get(worldContext->GetWorld());
}

void ALampSubsystem::setGroup(const FString& group, ELampMode mode, bool create) {
	auto rco = Cast<ULampSubsystemRCO>(Cast<AFGPlayerController>(GetWorld()->GetFirstPlayerController())->GetRemoteCallObjectOfClass(ULampSubsystemRCO::StaticClass()));
	if (rco) {
		rco->SetGroup(this, group, mode, create);
	}
}

ELampMode ALampSubsystem::getGroup(const FString& group, bool& found) {
	auto g = Groups.FindByKey(group);
	found = g != nullptr;
	if (g) {
		return g->Mode;
	} else return ELampMode::OFF;
}

void ALampSubsystem::removeGroup(const FString& group) {
	auto rco = Cast<ULampSubsystemRCO>(Cast<AFGPlayerController>(GetWorld()->GetFirstPlayerController())->GetRemoteCallObjectOfClass(ULampSubsystemRCO::StaticClass()));
	if (rco) rco->RemoveGroup(this, group);
}

void ALampSubsystem::getGroups(TArray<FString>& keys) {
	TArray<FString> arr;
	for (auto& group : Groups) {
		keys.Add(group.Name);
	}
}

void ALampSubsystem::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALampSubsystem, Groups);
}

void ULampSubsystemHolder::InitSubsystems() {
	LampSubsystem = Cast<ALampSubsystem>(K2_SpawnSubsystem(ALampSubsystem::StaticClass(), "LampSubsystem"));
}