#include "LampSubsystem.h"

#include "FGDismantleInterface.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "util/Logging.h"
#include "FGPlayerController.h"
#include "FGGameMode.h"
#include "FGInventoryLibrary.h"
#include "FGItemPickup_Spawnable.h"
#include "UnrealNetwork.h"
#include "FGInventoryComponentEquipment.h"
#include "Kismet/KismetMathLibrary.h"

ULampSubsystemRCO::ULampSubsystemRCO() {
	
}

#pragma optimize("", off)
void ULampSubsystemRCO::PlacePortableLamp_Implementation(AFGPlayerController* Player) {
	static UClass* portableLampClass = nullptr;
	if (!portableLampClass) portableLampClass = LoadObject<UClass>(NULL, TEXT("/Game/LightItUp/Lamps/PortableLight/PortableLamp.PortableLamp_C"));
	static UClass* portableLampEDClass = nullptr;
	if (!portableLampEDClass) portableLampEDClass = LoadObject<UClass>(NULL, TEXT("/Game/LightItUp/Lamps/PortableLight/ED_PortableLamp.ED_PortableLamp_C"));

	AFGCharacterPlayer* Char = Cast<AFGCharacterPlayer>(Player->GetCharacter());
	FVector startDir = UKismetMathLibrary::GetForwardVector(Char->GetActorRotation());
	startDir = startDir * FVector(200,200,0);
	FVector start = Char->GetActorTransform().GetLocation() + startDir + FVector(0,0,100);
	FVector end = start - FVector(0,0,400);
	FHitResult hit;
	if (Player->GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_GameTraceChannel6)) {
		FActorSpawnParameters params;
		params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AActor* lamp = Player->GetWorld()->SpawnActor<AActor>(portableLampClass, hit.Location, Char->GetActorRotation(), params);
		if (IsValid(lamp)) {
			UFGInventoryComponentEquipment* inv = Char->GetEquipmentSlot(EEquipmentSlot::ES_ARMS);
			inv->Remove(portableLampEDClass, 1);
		}
	}
}
#pragma optimize("", on)

bool ULampSubsystemRCO::PlacePortableLamp_Validate(AFGPlayerController* Player) {
	return true;
}

void ULampSubsystemRCO::PickupPortableLamp_Implementation(AActor* portableLamp, AFGPlayerController* Player) {
	TArray<FInventoryStack> stacks;
	IFGDismantleInterface::Execute_GetDismantleRefund(portableLamp, stacks);
	UFGInventoryComponent* inv = Cast<AFGCharacterPlayer>(Player->GetPawn())->GetInventory();
	if (inv->HasEnoughSpaceForStacks(stacks)) inv->AddStacks(stacks);
	else {
		float radius;
		FVector pos = IFGDismantleInterface::Execute_GetRefundSpawnLocationAndArea(portableLamp, Player->GetPawn()->GetActorLocation(), radius);
		TArray<class AFGItemPickup_Spawnable*> drops;
		AFGItemPickup_Spawnable::CreateItemDropsInCylinder(portableLamp->GetWorld(), stacks, pos, radius, {portableLamp}, drops);
	}
	portableLamp->Destroy();
}

bool ULampSubsystemRCO::PickupPortableLamp_Validate(AActor* portableLamp, AFGPlayerController* Player) {
	return true;
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