#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMedievalRPG, Log, All);

#define RPG_LOG(Format, ...) UE_LOG(LogMedievalRPG, Log, TEXT(Format), ##__VA_ARGS__)
#define RPG_WARNING(Format, ...) UE_LOG(LogMedievalRPG, Warning, TEXT(Format), ##__VA_ARGS__)
#define RPG_ERROR(Format, ...) UE_LOG(LogMedievalRPG, Error, TEXT(Format), ##__VA_ARGS__)

namespace MedievalRPG
{
	// Character Classes
	enum class ECharacterClass : uint8
	{
		Warrior = 0,
		Rogue = 1,
		Mage = 2,
		Ranger = 3
	};

	// Magic Schools
	enum class EMagicSchool : uint8
	{
		Pyromancy = 0,
		Cryomancy = 1,
		Electromancy = 2,
		Necromancy = 3,
		Restoration = 4
	};

	// Item Rarity
	enum class EItemRarity : uint8
	{
		Common = 0,
		Uncommon = 1,
		Rare = 2,
		Epic = 3,
		Legendary = 4
	};

	// Difficulty Levels
	enum class EDifficulty : uint8
	{
		Easy = 0,
		Normal = 1,
		Hard = 2,
		Legendary = 3,
		Nightmare = 4
	};

	// Faction Allegiance
	enum class EFactionAllegiance : uint8
	{
		Friendly = 0,
		Neutral = 1,
		Hostile = 2
	};
}
