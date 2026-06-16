#include "World/WorldManager.h"

AWorldManager::AWorldManager()
	: WorldTime(6.0f)
	, TimeScale(1.0f)
	, CurrentDifficulty(MedievalRPG::EDifficulty::Normal)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f;
}

void AWorldManager::BeginPlay()
{
	Super::BeginPlay();
	InitializeFactions();
}

void AWorldManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update world time (1 real second = 1 game minute)
	WorldTime += (DeltaTime * TimeScale / 60.0f);
	if (WorldTime >= 24.0f)
	{
		WorldTime -= 24.0f;
	}

	OnWorldTimeChanged.Broadcast(WorldTime);
}

void AWorldManager::AddReputation(uint8 FactionID, float Amount)
{
	FFactionData* Faction = FindFaction(FactionID);
	if (!Faction)
	{
		return;
	}

	Faction->Reputation += Amount;
	Faction->Reputation = FMath::Clamp(Faction->Reputation, -1000.0f, 1000.0f);

	UpdateAllegiance(*Faction);
	BroadcastReputationChanged(FactionID, Faction->Reputation, Faction->Allegiance);

	RPG_LOG("Reputation with faction %d changed by %.0f (total: %.0f)", FactionID, Amount, Faction->Reputation);
}

float AWorldManager::GetReputation(uint8 FactionID) const
{
	for (const FFactionData& Faction : Factions)
	{
		if (Faction.FactionID == FactionID)
		{
			return Faction.Reputation;
		}
	}
	return 0.0f;
}

MedievalRPG::EFactionAllegiance AWorldManager::GetFactionAllegiance(uint8 FactionID) const
{
	for (const FFactionData& Faction : Factions)
	{
		if (Faction.FactionID == FactionID)
		{
			return Faction.Allegiance;
		}
	}
	return MedievalRPG::EFactionAllegiance::Neutral;
}

bool AWorldManager::IsNight() const
{
	float CycleTime = GetDayNightCycle() * 24.0f;
	return CycleTime < 6.0f || CycleTime > 20.0f;
}

void AWorldManager::SetWorldTime(float NewTime)
{
	WorldTime = FMath::Fmod(NewTime, 24.0f);
	OnWorldTimeChanged.Broadcast(WorldTime);
}

void AWorldManager::SetDifficulty(MedievalRPG::EDifficulty NewDifficulty)
{
	CurrentDifficulty = NewDifficulty;
	RPG_LOG("Difficulty set to: %d", static_cast<int32>(NewDifficulty));
}

void AWorldManager::InitializeFactions()
{
	// Create 10 factions
	for (uint8 i = 0; i < 10; ++i)
	{
		FFactionData Faction;
		Faction.FactionID = i;
		Faction.FactionName = FString::Printf(TEXT("Faction_%d"), i);
		Faction.Reputation = 0.0f;
		Faction.Allegiance = MedievalRPG::EFactionAllegiance::Neutral;

		Factions.Add(Faction);
	}

	RPG_LOG("World manager initialized with %d factions", Factions.Num());
}

FFactionData* AWorldManager::FindFaction(uint8 FactionID)
{
	for (FFactionData& Faction : Factions)
	{
		if (Faction.FactionID == FactionID)
		{
			return &Faction;
		}
	}
	return nullptr;
}

void AWorldManager::UpdateAllegiance(FFactionData& Faction)
{
	if (Faction.Reputation < -500.0f)
	{
		Faction.Allegiance = MedievalRPG::EFactionAllegiance::Hostile;
	}
	else if (Faction.Reputation < -100.0f)
	{
		Faction.Allegiance = MedievalRPG::EFactionAllegiance::Neutral;
	}
	else if (Faction.Reputation >= 100.0f)
	{
		Faction.Allegiance = MedievalRPG::EFactionAllegiance::Friendly;
	}
	else
	{
		Faction.Allegiance = MedievalRPG::EFactionAllegiance::Neutral;
	}
}

void AWorldManager::BroadcastReputationChanged(uint8 FactionID, float NewReputation, MedievalRPG::EFactionAllegiance NewAllegiance)
{
	OnReputationChanged.Broadcast(FactionID, NewReputation, NewAllegiance);
}
