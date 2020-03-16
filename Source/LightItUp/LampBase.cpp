#include "LampBase.h"

#include "FGTimeSubsystem.h"
#include "util/Logging.h"
#include "UnrealNetwork.h"
#include "FGPlayerController.h"
#include "Components/SceneComponent.h"

#define DefaultLampGroup TEXT("Default")

void ULampBaseRCO::SetGroup_Implementation(ALampBase* lamp, const FString& string) {
	lamp->mGroup = string;
	lamp->ForceNetUpdate();
}

bool ULampBaseRCO::SetGroup_Validate(ALampBase* lamp, const FString& group) {
	return true;
}

void ULampBaseRCO::SetMode_Implementation(ALampBase* lamp, ELampMode mode) {
	lamp->mMode = mode;
	lamp->ForceNetUpdate();
}

bool ULampBaseRCO::SetMode_Validate(ALampBase* lamp, ELampMode mode) {
	return true;
}

void ULampBaseRCO::SetUseGroup_Implementation(ALampBase* lamp, bool useGroup) {
	lamp->mUseGroup = useGroup;
	lamp->ForceNetUpdate();
}

bool ULampBaseRCO::SetUseGroup_Validate(ALampBase* lamp, bool useGroup) {
	return true;
}

void ULampBaseRCO::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ULampBaseRCO, bTest)
}

bool ALampBase::shouldLampBeOn(ELampMode mode) {
	switch (mode) {
	case ON:
		return true;
	case AUTO:
		return AFGTimeOfDaySubsystem::Get(this)->IsNight();
	default:
		return false;
	}
}

ALampBase::ALampBase() {
	mPowerConnection = CreateDefaultSubobject<UFGPowerConnectionComponent>(TEXT("PowerConnection"));
	mPowerConnection->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	mPowerConnection->bEditableWhenInherited = true;
	mPowerInfo = CreateDefaultSubobject<UFGPowerInfoComponent>(TEXT("PowerInfo"));
	mPowerConnection->SetPowerInfo(mPowerInfo);

	mFactoryTickFunction.bCanEverTick = true;
	mFactoryTickFunction.bStartWithTickEnabled = true;
	mFactoryTickFunction.bRunOnAnyThread = true;
	mFactoryTickFunction.bAllowTickOnDedicatedServer = true;

	if (HasAuthority()) mFactoryTickFunction.SetTickFunctionEnable(true);
	
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	PrimaryActorTick.SetTickFunctionEnable(true);

	bReplicates = true;
}

void ALampBase::BeginPlay() {
	Super::BeginPlay();

	lampSubSys = ALampSubsystem::Get(this);
}

void ALampBase::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction & ThisTickFunction) {
	if ((bLampOn != (mLastLampOn == 1)) || mLastLampOn == 2) {
		mLastLampOn = bLampOn ? 1 : 0;
		auto lights = GetComponentsByClass(ULightComponent::StaticClass());
		for (auto& light : lights) {
			Cast<USceneComponent>(light)->SetVisibilitySML(bLampOn);
		}
		auto meshes = GetComponentsByClass(UStaticMeshComponent::StaticClass());
		for (auto mesh : meshes) {
			Cast<UStaticMeshComponent>(mesh)->SetScalarParameterValueOnMaterials("LitUp", bLampOn ? mEmessiveStrength : 0.0f);
		}
	}
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);
}

void ALampBase::Factory_Tick(float dt) {
	if (HasAuthority()) {
		ELampMode mode = mMode;
		if (mUseGroup) {
			bool worked = false;
			auto gmode = lampSubSys->getGroup(mGroup, worked);
			if (worked) mode = gmode;
			else {
				mode = mMode = OFF;
				mGroup = DefaultLampGroup;
			}
		}
		auto shouldBeOn = shouldLampBeOn(mode);
		mPowerInfo->SetTargetConsumption(shouldBeOn ? mPowerConsumption : 0.0f);
		bool hasPower = mPowerInfo->HasPower();
		if (hasPower != bLampOn) {
			bLampOn = hasPower;
			ForceNetUpdate();
		}
	}
}

bool ALampBase::ShouldSave_Implementation() const {
	return true;
}

void ALampBase::SetGroup(const FString& group) {
	auto rco = Cast<ULampBaseRCO>(Cast<AFGPlayerController>(GetWorld()->GetFirstPlayerController())->GetRemoteCallObjectOfClass(ULampBaseRCO::StaticClass()));
	if (rco) {
		rco->SetGroup(this, group);
	}
}

void ALampBase::SetMode(TEnumAsByte<ELampMode> mode) {
	auto rco = Cast<ULampBaseRCO>(Cast<AFGPlayerController>(GetWorld()->GetFirstPlayerController())->GetRemoteCallObjectOfClass(ULampBaseRCO::StaticClass()));
	if (rco) {
		rco->SetMode(this, mode);
	}
}

void ALampBase::SetUseGroup(bool useGroup) {
	auto rco = Cast<ULampBaseRCO>(Cast<AFGPlayerController>(GetWorld()->GetFirstPlayerController())->GetRemoteCallObjectOfClass(ULampBaseRCO::StaticClass()));
	if (rco) {
		rco->SetUseGroup(this, useGroup);
	}
}

void ALampBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALampBase, bLampOn);
	DOREPLIFETIME(ALampBase, mMode);
	DOREPLIFETIME(ALampBase, mGroup);
	DOREPLIFETIME(ALampBase, mUseGroup);
}
