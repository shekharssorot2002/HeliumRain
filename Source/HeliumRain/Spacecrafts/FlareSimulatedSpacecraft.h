
#pragma once

#include "Object.h"
#include "FlareSpacecraftInterface.h"
#include "Subsystems/FlareSimulatedSpacecraftDamageSystem.h"
#include "Subsystems/FlareSimulatedSpacecraftNavigationSystem.h"
#include "Subsystems/FlareSimulatedSpacecraftDockingSystem.h"
#include "Subsystems/FlareSimulatedSpacecraftWeaponsSystem.h"
#include "FlareSimulatedSpacecraft.generated.h"

class UFlareGame;
class UFlareSimulatedSector;
class UFlareFleet;
class UFlareCargoBay;

UCLASS()
class HELIUMRAIN_API UFlareSimulatedSpacecraft : public UObject, public IFlareSpacecraftInterface
{
	 GENERATED_UCLASS_BODY()

public:

	/*----------------------------------------------------
		Save
	----------------------------------------------------*/

	/** Load the ship from a save file */
	virtual void Load(const FFlareSpacecraftSave& Data) override;

	/** Save the ship to a save file */
	virtual FFlareSpacecraftSave* Save() override;

	/** Get the parent company */
	virtual UFlareCompany* GetCompany() override;

	/** Get the ship size class */
	virtual EFlarePartSize::Type GetSize() override;

	virtual FName GetImmatriculation() const override;

	/** Check if this is a military ship */
	virtual bool IsMilitary() const override;

	/** Check if this is a station ship */
	virtual bool IsStation() const override;


	/*----------------------------------------------------
		Sub system
	----------------------------------------------------*/

	virtual UFlareSimulatedSpacecraftDamageSystem* GetDamageSystem() const override;

	virtual UFlareSimulatedSpacecraftNavigationSystem* GetNavigationSystem() const override;

	virtual UFlareSimulatedSpacecraftDockingSystem* GetDockingSystem() const override;

	virtual UFlareSimulatedSpacecraftWeaponsSystem* GetWeaponsSystem() const override;

    /*----------------------------------------------------
        Gameplay
    ----------------------------------------------------*/

	virtual void SetCurrentSector(UFlareSimulatedSector* Sector);

	virtual void SetCurrentFleet(UFlareFleet* Fleet)
	{
		CurrentFleet = Fleet;
	}

	virtual void SetSpawnMode(EFlareSpawnMode::Type SpawnMode);

	virtual bool CanBeFlown() const override
	{
		return !IsStation() && (CurrentSector != NULL) && ! IsAssignedToSector();
	}

	virtual bool IsAssignedToSector() const override
	{
		return SpacecraftData.IsAssigned;
	}

	void AssignToSector(bool Assign) override;

	/** Set asteroid data from an asteroid save */
	void SetAsteroidData(FFlareAsteroidSave* Data);


	/*----------------------------------------------------
		Resources
	----------------------------------------------------*/

	bool CanTradeWith(UFlareSimulatedSpacecraft* OtherSpacecraft);

protected:

    /*----------------------------------------------------
        Protected data
    ----------------------------------------------------*/

    // Gameplay data
	FFlareSpacecraftSave          SpacecraftData;
	FFlareSpacecraftDescription*  SpacecraftDescription;

	AFlareGame*                   Game;

	UFlareFleet*                  CurrentFleet;
	UFlareSimulatedSector*        CurrentSector;

	// Systems
	UPROPERTY()
	UFlareSimulatedSpacecraftDamageSystem*                  DamageSystem;
	UPROPERTY()
	UFlareSimulatedSpacecraftNavigationSystem*              NavigationSystem;
	UPROPERTY()
	UFlareSimulatedSpacecraftDockingSystem*                 DockingSystem;
	UPROPERTY()
	UFlareSimulatedSpacecraftWeaponsSystem*                 WeaponsSystem;

	UPROPERTY()
	TArray<UFlareFactory*>                                  Factories;

	UPROPERTY()
	UFlareCargoBay*                                         CargoBay;
public:

    /*----------------------------------------------------
        Getters
    ----------------------------------------------------*/

	inline AFlareGame* GetGame() const override
	{
		return Game;
	}	

	inline FName GetNickName() const override
	{
		return SpacecraftData.NickName;
	}

	/** Return null if traveling */
	inline UFlareSimulatedSector* GetCurrentSector()
	{
		return CurrentSector;
	}

	/** Return null if not in a fleet */
	inline UFlareFleet* GetCurrentFleet()
	{
		return CurrentFleet;
	}

	/** Return null if not in a trade route */
	inline UFlareTradeRoute* GetCurrentTradeRoute()
	{
		return (CurrentFleet ? CurrentFleet->GetCurrentTradeRoute() : NULL);
	}

	inline FFlareSpacecraftDescription* GetDescription() const
	{
		return SpacecraftDescription;
	}

	inline UFlareCargoBay* GetCargoBay() override
	{
		return CargoBay;
	}

	inline UFlareSectorInterface* GetCurrentSectorInterface() override
	{
		return CurrentSector;
	}

	inline TArray<UFlareFactory*>& GetFactories()
	{
		return Factories;
	}

	inline FVector GetSpawnLocation() const
	{
		return SpacecraftData.Location;
	}

	inline bool HasCapability(EFlareSpacecraftCapability::Type Capability) const
	{
		return SpacecraftDescription->Capabilities.Contains(Capability);
	}

	inline bool IsConsumeResource(FFlareResourceDescription* Resource) const
	{
		return HasCapability(EFlareSpacecraftCapability::Consumer) && !SpacecraftData.SalesExcludedResources.Contains(Resource->Identifier);
	}

};
