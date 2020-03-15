#include "LampSubsystem.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "util/Logging.h"

bool ALampSubsystem::ShouldSave_Implementation() const {
	return true;
}

ALampSubsystem::ALampSubsystem() {
	if (!groups.Contains(DefaultLampGroup)) groups.Add(DefaultLampGroup, AUTO);
}

ALampSubsystem * ALampSubsystem::Get(UWorld * world) {
	TArray<AActor*> subsystems;
	UGameplayStatics::GetAllActorsOfClass(world, ALampSubsystem::StaticClass(), subsystems);
	if (subsystems.Num() > 0) return (ALampSubsystem*) subsystems[0];
	return world->SpawnActor<ALampSubsystem>();
}

ALampSubsystem * ALampSubsystem::Get(UObject * worldContext) {
	return Get(worldContext->GetWorld());
}

bool ALampSubsystem::setGroup(FString group, ELampMode mode, bool create) {
	group = group.TrimStartAndEnd();
	if (group.Len() < 1) return false;
	if (auto g = groups.Find(group)) {
		*g = mode;
		return true;
	} else if (create) {
		groups.Add(group, mode);
		return true;
	}
	return false;
}

ELampMode ALampSubsystem::getGroup(FString group, bool& found) {
	auto g = groups.Find(group);
	found = g != nullptr;
	if (g) return *g;
	else return ELampMode::OFF;
}

bool ALampSubsystem::removeGroup(FString group) {
	Log(FString(TEXT("Group: ")) + group);
	if (group == DefaultLampGroup) return false;
	return groups.Remove(group) > 0;
}

void ALampSubsystem::getGroups(TArray<FString>& keys) {
	TArray<FString> arr;
	groups.GetKeys(arr);
	for (int i = 0; i < arr.Num(); i++) {
		keys.Add(arr[i]);
	}
}

void ALampSubsystem::Log(FString logStr) {
	SML::Logging::error(*logStr);
}
