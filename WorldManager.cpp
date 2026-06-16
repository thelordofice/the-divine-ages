#include "Components/CharacterAttributeComponent.h"

UCharacterAttributeComponent::UCharacterAttributeComponent()
	: CurrentHealth(100.0f)
	, MaxHealth(100.0f)
	, CurrentMana(100.0f)
	, MaxMana(100.0f)
	, CurrentStamina(100.0f)
	, MaxStamina(100.0f)
	, HealthRegenRate(5.0f)
	, ManaRegenRate(15.0f)
	, StaminaRegenRate(25.0f)
	, Strength(10.0f)
	, Dexterity(10.0f)
	, Intelligence(10.0f)
	, Wisdom(10.0f)
	, Endurance(10.0f)
	, Luck(10.0f)
	, ClassType(MedievalRPG::ECharacterClass::Warrior)
{
	PrimaryComponentTick.TickInterval = 0.1f;
}

void UCharacterAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCharacterAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCharacterAttributeComponent::InitializeAttributes(MedievalRPG::ECharacterClass CharacterClass)
{
	ClassType = CharacterClass;

	// Set base attributes based on class
	switch (CharacterClass)
	{
	case MedievalRPG::ECharacterClass::Warrior:
		Strength = 16.0f;
		Dexterity = 12.0f;
		Intelligence = 8.0f;
		Wisdom = 10.0f;
		Endurance = 15.0f;
		MaxHealth = 150.0f;
		MaxMana = 50.0f;
		break;
	case MedievalRPG::ECharacterClass::Rogue:
		Strength = 12.0f;
		Dexterity = 16.0f;
		Intelligence = 12.0f;
		Wisdom = 10.0f;
		Endurance = 12.0f;
		MaxHealth = 100.0f;
		MaxMana = 75.0f;
		break;
	case MedievalRPG::ECharacterClass::Mage:
		Strength = 8.0f;
		Dexterity = 10.0f;
		Intelligence = 18.0f;
		Wisdom = 14.0f;
		Endurance = 10.0f;
		MaxHealth = 80.0f;
		MaxMana = 200.0f;
		break;
	case MedievalRPG::ECharacterClass::Ranger:
		Strength = 14.0f;
		Dexterity = 15.0f;
		Intelligence = 11.0f;
		Wisdom = 13.0f;
		Endurance = 13.0f;
		MaxHealth = 110.0f;
		MaxMana = 80.0f;
		break;
	}

	CurrentHealth = MaxHealth;
	CurrentMana = MaxMana;
	CurrentStamina = MaxStamina;
}

void UCharacterAttributeComponent::TakeDamage(float Amount)
{
	CurrentHealth = FMath::Max(0.0f, CurrentHealth - Amount);
	BroadcastHealthChanged();

	if (IsDead())
	{
		RPG_LOG("Character died");
	}
}

void UCharacterAttributeComponent::Heal(float Amount)
{
	CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + Amount);
	BroadcastHealthChanged();
}

void UCharacterAttributeComponent::ConsumeMana(float Amount)
{
	CurrentMana = FMath::Max(0.0f, CurrentMana - Amount);
	BroadcastManaChanged();
}

void UCharacterAttributeComponent::RegenerateMana(float Amount)
{
	CurrentMana = FMath::Min(MaxMana, CurrentMana + Amount);
	BroadcastManaChanged();
}

void UCharacterAttributeComponent::ConsumeStamina(float Amount)
{
	CurrentStamina = FMath::Max(0.0f, CurrentStamina - Amount);
	BroadcastStaminaChanged();
}

void UCharacterAttributeComponent::LevelUp()
{
	// Increase stats based on class
	switch (ClassType)
	{
	case MedievalRPG::ECharacterClass::Warrior:
		MaxHealth += 25.0f;
		MaxMana += 5.0f;
		Strength += 1.0f;
		Endurance += 1.0f;
		break;
	case MedievalRPG::ECharacterClass::Rogue:
		MaxHealth += 15.0f;
		MaxMana += 10.0f;
		Dexterity += 1.0f;
		Strength += 0.5f;
		break;
	case MedievalRPG::ECharacterClass::Mage:
		MaxHealth += 10.0f;
		MaxMana += 25.0f;
		Intelligence += 1.0f;
		Wisdom += 0.5f;
		break;
	case MedievalRPG::ECharacterClass::Ranger:
		MaxHealth += 18.0f;
		MaxMana += 12.0f;
		Dexterity += 1.0f;
		Wisdom += 0.5f;
		break;
	}

	CurrentHealth = MaxHealth;
	CurrentMana = MaxMana;
	CurrentStamina = MaxStamina;

	BroadcastHealthChanged();
	BroadcastManaChanged();
	BroadcastStaminaChanged();
}

void UCharacterAttributeComponent::RegenerateResources(float DeltaTime)
{
	// Regenerate health
	if (CurrentHealth < MaxHealth)
	{
		CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + (HealthRegenRate * DeltaTime));
		BroadcastHealthChanged();
	}

	// Regenerate mana
	if (CurrentMana < MaxMana)
	{
		CurrentMana = FMath::Min(MaxMana, CurrentMana + (ManaRegenRate * DeltaTime));
		BroadcastManaChanged();
	}

	// Regenerate stamina
	if (CurrentStamina < MaxStamina)
	{
		CurrentStamina = FMath::Min(MaxStamina, CurrentStamina + (StaminaRegenRate * DeltaTime));
		BroadcastStaminaChanged();
	}
}

void UCharacterAttributeComponent::BroadcastHealthChanged()
{
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UCharacterAttributeComponent::BroadcastManaChanged()
{
	OnManaChanged.Broadcast(CurrentMana, MaxMana);
}

void UCharacterAttributeComponent::BroadcastStaminaChanged()
{
	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
}
