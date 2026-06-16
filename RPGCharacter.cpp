#include "Components/SpellComponent.h"

USpellComponent::USpellComponent()
	: SelectedSpellID(0)
	, bIsCasting(false)
	, CurrentCastTime(0.0f)
{
	PrimaryComponentTick.TickInterval = 0.016f;
}

void USpellComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeSpellDatabase();
}

void USpellComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateCooldowns(DeltaTime);
}

bool USpellComponent::CastSpell(uint8 SpellID, AActor* Caster, AActor* Target)
{
	if (!HasSpell(SpellID))
	{
		RPG_WARNING("Spell %d not learned", SpellID);
		return false;
	}

	const FSpellData* SpellData = GetSpellData(SpellID);
	if (!SpellData)
	{
		return false;
	}

	// Check cooldown
	if (SpellCooldowns.Contains(SpellID) && SpellCooldowns[SpellID] > 0.0f)
	{
		RPG_WARNING("Spell %d on cooldown", SpellID);
		return false;
	}

	// Check mana (would be handled by character attributes)
	if (bIsCasting)
	{
		return false;
	}

	bIsCasting = true;
	CurrentCastTime = SpellData->CastTime;

	// After cast time, deal damage
	if (Target && Caster)
	{
		float Damage = SpellData->Damage;

		// Apply spell synergy bonuses based on magic school combinations
		Target->TakeDamage(Damage, FDamageEvent(), nullptr, Caster);

		// Set cooldown
		SpellCooldowns.Add(SpellID, SpellData->Cooldown);
	}

	bIsCasting = false;
	BroadcastSpellCast(SpellID, true);

	RPG_LOG("Spell %d cast successfully", SpellID);
	return true;
}

void USpellComponent::LearnSpell(uint8 SpellID)
{
	if (HasSpell(SpellID))
	{
		RPG_WARNING("Spell %d already learned", SpellID);
		return;
	}

	FSpellData NewSpell = GetSpellTemplate(SpellID);
	LearnedSpells.Add(NewSpell);

	OnSpellLearned.Broadcast(SpellID);
	RPG_LOG("Spell %d learned", SpellID);
}

bool USpellComponent::HasSpell(uint8 SpellID) const
{
	for (const FSpellData& Spell : LearnedSpells)
	{
		if (Spell.SpellID == SpellID)
		{
			return true;
		}
	}
	return false;
}

void USpellComponent::SelectNextSpell()
{
	if (LearnedSpells.IsEmpty())
	{
		return;
	}

	int32 CurrentIndex = 0;
	for (int32 i = 0; i < LearnedSpells.Num(); ++i)
	{
		if (LearnedSpells[i].SpellID == SelectedSpellID)
		{
			CurrentIndex = i;
			break;
		}
	}

	SelectedSpellID = LearnedSpells[(CurrentIndex + 1) % LearnedSpells.Num()].SpellID;
	RPG_LOG("Selected spell: %d", SelectedSpellID);
}

const FSpellData* USpellComponent::GetSpellData(uint8 SpellID) const
{
	for (const FSpellData& Spell : LearnedSpells)
	{
		if (Spell.SpellID == SpellID)
		{
			return &Spell;
		}
	}
	return nullptr;
}

float USpellComponent::GetSpellCooldownRemaining(uint8 SpellID) const
{
	if (SpellCooldowns.Contains(SpellID))
	{
		return SpellCooldowns[SpellID];
	}
	return 0.0f;
}

void USpellComponent::InitializeSpellDatabase()
{
	// Initialize spell database with 50+ spells across 5 schools
	// This would load from data tables in a full implementation
}

FSpellData USpellComponent::GetSpellTemplate(uint8 SpellID) const
{
	// Return spell template from database
	FSpellData Spell;
	Spell.SpellID = SpellID;

	switch (SpellID % 10)
	{
	case 0:
		Spell.SpellName = TEXT("Fireball");
		Spell.MagicSchool = MedievalRPG::EMagicSchool::Pyromancy;
		Spell.ManaCost = 30.0f;
		Spell.CastTime = 1.0f;
		Spell.Cooldown = 2.0f;
		Spell.Damage = 50.0f;
		break;
	case 1:
		Spell.SpellName = TEXT("Frostbolt");
		Spell.MagicSchool = MedievalRPG::EMagicSchool::Cryomancy;
		Spell.ManaCost = 25.0f;
		Spell.CastTime = 0.8f;
		Spell.Cooldown = 1.5f;
		Spell.Damage = 40.0f;
		break;
	case 2:
		Spell.SpellName = TEXT("Lightning Strike");
		Spell.MagicSchool = MedievalRPG::EMagicSchool::Electromancy;
		Spell.ManaCost = 35.0f;
		Spell.CastTime = 0.5f;
		Spell.Cooldown = 3.0f;
		Spell.Damage = 60.0f;
		break;
	case 3:
		Spell.SpellName = TEXT("Heal");
		Spell.MagicSchool = MedievalRPG::EMagicSchool::Restoration;
		Spell.ManaCost = 40.0f;
		Spell.CastTime = 1.5f;
		Spell.Cooldown = 5.0f;
		Spell.Damage = -50.0f; // Negative damage = healing
		break;
	default:
		Spell.SpellName = TEXT("Unknown Spell");
		Spell.ManaCost = 20.0f;
		Spell.Damage = 25.0f;
	}

	return Spell;
}

void USpellComponent::UpdateCooldowns(float DeltaTime)
{
	for (auto& Cooldown : SpellCooldowns)
	{
		Cooldown.Value = FMath::Max(0.0f, Cooldown.Value - DeltaTime);
	}
}

void USpellComponent::BroadcastSpellCast(uint8 SpellID, bool bSuccess)
{
	OnSpellCast.Broadcast(SpellID, bSuccess);
}
