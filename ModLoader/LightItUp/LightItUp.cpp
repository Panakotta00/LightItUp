#include <stdafx.h>
#include <string>
#include <game/Global.h>
#include <game/Input.h>
#include <mod/Mod.h>
#include <HookLoaderInternal.h>
#include <mod/ModFunctions.h>
#include <mod/MathFunctions.h>
#include <util/JsonConfig.h>
#include <assets/AssetFunctions.h>
#include <assets/FObjectSpawnParameters.h>
#include <util/Utility.h>
#include <detours.h>
#include <../SDK.hpp>
#include <FG_FactoryGame_classes.hpp>

using namespace SML::Mod;
using namespace SML::Objects;

#define SML_VERSION "1.0.0-pr6"
#define MOD_NAME "LightItUp"

#define LOG(msg) SML::Utility::infoMod(MOD_NAME, msg)

json config = SML::Utility::JsonConfig::load(MOD_NAME, {
	
});

Mod::Info modInfo {
	SML_VERSION,
	MOD_NAME,
	"0.1",
	"A Satisfactory mod which adds a couple of Lamps.",
	"Panakotta00, PortalGamesMais, Trxnce, RosszEmber",
	{}
};

class LightItUp : public Mod {
	void beginPlay(Functions::ModReturns* modReturns, AFGPlayerController* playerIn) {
		LOG("LightItUp adding recipes...");
		SDK::UClass* add = static_cast<SDK::UClass*>(Functions::loadObjectFromPak(SDK::UClass::StaticClass(), L"/Game/FactoryGame/LightItUp/Lamps/StreetLamp/Recipe_StreetLamp.Recipe_StreetLamp_C"));
		Functions::addRecipe(add);
		add = static_cast<SDK::UClass*>(Functions::loadObjectFromPak(SDK::UClass::StaticClass(), L"/Game/FactoryGame/LightItUp/Lamps/LampPole/Recipe_LampPole.Recipe_LampPole_C"));
		Functions::addRecipe(add);
		add = static_cast<SDK::UClass*>(Functions::loadObjectFromPak(SDK::UClass::StaticClass(), L"/Game/FactoryGame/LightItUp/Lamps/PowerPoleLight/BR_PowerPoleLight.BR_PowerPoleLight_C"));
		Functions::addRecipe(add);
		//add = static_cast<SDK::UClass*>(Functions::loadObjectFromPak(SDK::UClass::StaticClass(), L"/Game/FactoryGame/LightItUp/Lamps/PoleLight/BR_PoleLight.BR_PoleLight_C"));
		//Functions::addRecipe(add);
		//add = static_cast<SDK::UClass*>(Functions::loadObjectFromPak(SDK::UClass::StaticClass(), L"/Game/FactoryGame/LightItUp/Lamps/WallLight/BR_WallLight.BR_WallLight_C"));
		//Functions::addRecipe(add);
		add = static_cast<SDK::UClass*>(Functions::loadObjectFromPak(SDK::UClass::StaticClass(), L"/Game/FactoryGame/LightItUp/Lamps/ConstructionLight/BR_ConstructionLight.BR_ConstructionLight_C"));
		Functions::addRecipe(add);
		add = static_cast<SDK::UClass*>(Functions::loadObjectFromPak(SDK::UClass::StaticClass(), L"/Game/FactoryGame/LightItUp/Lamps/PortableLight/BR_PortableLamp.BR_PortableLamp_C"));
		Functions::addRecipe(add);
		//add = static_cast<SDK::UClass*>(Functions::loadObjectFromPak(SDK::UClass::StaticClass(), L"/Game/FactoryGame/LightItUp/Lamps/SpiderLightPost/Recipe_SpiderLamp.Recipe_SpiderLamp_C"));
		//Functions::addRecipe(add);
		//add = static_cast<SDK::UClass*>(Functions::loadObjectFromPak(SDK::UClass::StaticClass(), L"/Game/FactoryGame/PanaMod/Power/Recipe_LightConnector.Recipe_LightConnector_C"));
		//Functions::addRecipe(add);
		LOG("LightItUp Recipes added!");
	}
	
public:
	LightItUp() : Mod(modInfo) {
	}

	void setup() override {
		using namespace std::placeholders;

		SDK::InitSDK();
		::subscribe<&AFGPlayerController::BeginPlay>(std::bind(&LightItUp::beginPlay, this, _1, _2));
		
		LOG("Finished LightItUp setup!");
	}

	void postSetup() override {
		
	}

	~LightItUp() {
		LOG("LightItUp finished cleanup!");
	}
};

MOD_API Mod* ModCreate() {
	return new LightItUp();
}