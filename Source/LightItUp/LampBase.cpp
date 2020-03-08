#include "LampBase.h"

#include "FGTimeSubsystem.h"
#include "util/Logging.h"
#include "UnrealNetwork.h"
#include "Components/SceneComponent.h"

#define DefaultLampGroup TEXT("Default")

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

	mFactoryTickFunction.SetTickFunctionEnable(true);
	
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	PrimaryActorTick.SetTickFunctionEnable(true);
}

void ALampBase::BeginPlay() {
	Super::BeginPlay();

	lampSubSys = ALampSubsystem::Get(this);
	
	AFGTimeOfDaySubsystem::Get(this)->SetDaySeconds(0);
}

void ALampBase::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction & ThisTickFunction) {
	if ((mLampOn != (bool)mLastLampOn) || mLastLampOn == 2) {
		mLastLampOn = mLampOn;
		auto lights = GetComponentsByClass(ULightComponent::StaticClass());
		for (auto& light : lights) {
			Cast<USceneComponent>(light)->SetVisibilitySML(mLampOn);
		}
		auto meshes = GetComponentsByClass(UStaticMeshComponent::StaticClass());
		for (auto mesh : meshes) {
			Cast<UStaticMeshComponent>(mesh)->SetScalarParameterValueOnMaterials("LitUp", mLampOn ? mEmessiveStrength : 0.0f);
		}
	}
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);
}

void ALampBase::Factory_Tick(float dt) {
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
	mLampOn = mPowerInfo->HasPower();
}

bool ALampBase::ShouldSave_Implementation() const {
	return true;
}

void ALampBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALampBase, mLampOn);
}