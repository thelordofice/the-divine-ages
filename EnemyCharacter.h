#include "UI/GameHUD.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UGameHUD::NativeConstruct()
{
	Super::NativeConstruct();
}

void UGameHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UGameHUD::UpdateHealthBar(float CurrentHealth, float MaxHealth)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(FMath::Clamp(CurrentHealth / MaxHealth, 0.0f, 1.0f));
	}
}

void UGameHUD::UpdateManaBar(float CurrentMana, float MaxMana)
{
	if (ManaBar)
	{
		ManaBar->SetPercent(FMath::Clamp(CurrentMana / MaxMana, 0.0f, 1.0f));
	}
}

void UGameHUD::UpdateStaminaBar(float CurrentStamina, float MaxStamina)
{
	if (StaminaBar)
	{
		StaminaBar->SetPercent(FMath::Clamp(CurrentStamina / MaxStamina, 0.0f, 1.0f));
	}
}

void UGameHUD::UpdateExperienceBar(float CurrentExp, float MaxExp)
{
	if (MaxExp > 0.0f)
	{
		// Update experience bar
	}
}

void UGameHUD::ShowQuestNotification(const FString& QuestName, const FString& Objective)
{
	RPG_LOG("Quest: %s - %s", *QuestName, *Objective);
}

void UGameHUD::ShowCombatLog(const FString& Message)
{
	if (CombatLogText)
	{
		CombatLogText->SetText(FText::FromString(Message));
	}
}

void UGameHUD::UpdateMinimap()
{
	// Update minimap
}
