#include "World/QuestSystem.h"

AQuestSystem::AQuestSystem()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f;
}

void AQuestSystem::BeginPlay()
{
	Super::BeginPlay();
	InitializeQuestDatabase();
}

void AQuestSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AQuestSystem::StartQuest(uint8 QuestID)
{
	FQuestData* ExistingQuest = FindActiveQuest(QuestID);
	if (ExistingQuest)
	{
		RPG_WARNING("Quest %d already active", QuestID);
		return;
	}

	FQuestData Quest = GetQuestTemplate(QuestID);
	Quest.Status = EQuestStatus::Active;
	ActiveQuests.Add(Quest);

	BroadcastQuestStatusChanged(QuestID, EQuestStatus::Active);
	RPG_LOG("Quest started: %s", *Quest.Title);
}

void AQuestSystem::UpdateObjective(uint8 QuestID, uint8 ObjectiveIndex, int32 Amount)
{
	FQuestData* Quest = FindActiveQuest(QuestID);
	if (!Quest || ObjectiveIndex >= Quest->Objectives.Num())
	{
		return;
	}

	Quest->Objectives[ObjectiveIndex].CurrentCount += Amount;
	if (Quest->Objectives[ObjectiveIndex].CurrentCount >= Quest->Objectives[ObjectiveIndex].TargetCount)
	{
		Quest->Objectives[ObjectiveIndex].bCompleted = true;
	}

	BroadcastQuestObjectiveUpdated(QuestID);
	RPG_LOG("Quest %d objective %d updated", QuestID, ObjectiveIndex);

	// Check if all objectives are complete
	bool bAllComplete = true;
	for (const FQuestObjective& Objective : Quest->Objectives)
	{
		if (!Objective.bCompleted)
		{
			bAllComplete = false;
			break;
		}
	}

	if (bAllComplete)
	{
		CompleteQuest(QuestID);
	}
}

void AQuestSystem::CompleteQuest(uint8 QuestID)
{
	FQuestData* Quest = FindActiveQuest(QuestID);
	if (!Quest)
	{
		return;
	}

	Quest->Status = EQuestStatus::Complete;
	CompletedQuests.Add(*Quest);
	ActiveQuests.RemoveSingle(*Quest);

	BroadcastQuestStatusChanged(QuestID, EQuestStatus::Complete);
	RPG_LOG("Quest completed: %s", *Quest->Title);
}

void AQuestSystem::FailQuest(uint8 QuestID)
{
	FQuestData* Quest = FindActiveQuest(QuestID);
	if (!Quest)
	{
		return;
	}

	Quest->Status = EQuestStatus::Failed;
	FailedQuests.Add(*Quest);
	ActiveQuests.RemoveSingle(*Quest);

	BroadcastQuestStatusChanged(QuestID, EQuestStatus::Failed);
	RPG_LOG("Quest failed: %s", *Quest->Title);
}

void AQuestSystem::AbandonQuest(uint8 QuestID)
{
	FQuestData* Quest = FindActiveQuest(QuestID);
	if (!Quest)
	{
		return;
	}

	ActiveQuests.RemoveSingle(*Quest);
	BroadcastQuestStatusChanged(QuestID, EQuestStatus::Inactive);
	RPG_LOG("Quest abandoned: %s", *Quest->Title);
}

const FQuestData* AQuestSystem::GetQuestData(uint8 QuestID) const
{
	for (const FQuestData& Quest : ActiveQuests)
	{
		if (Quest.QuestID == QuestID)
		{
			return &Quest;
		}
	}

	for (const FQuestData& Quest : CompletedQuests)
	{
		if (Quest.QuestID == QuestID)
		{
			return &Quest;
		}
	}

	return nullptr;
}

EQuestStatus AQuestSystem::GetQuestStatus(uint8 QuestID) const
{
	const FQuestData* Quest = GetQuestData(QuestID);
	return Quest ? Quest->Status : EQuestStatus::Inactive;
}

void AQuestSystem::InitializeQuestDatabase()
{
	// Initialize quest database with 100+ quests
	// In a full implementation, this would load from data tables
}

FQuestData AQuestSystem::GetQuestTemplate(uint8 QuestID) const
{
	FQuestData Quest;
	Quest.QuestID = QuestID;
	Quest.Level = 1 + (QuestID / 20);

	// Template system for generating quests
	switch (QuestID % 10)
	{
	case 0:
		Quest.Title = TEXT("Defeat the Bandits");
		Quest.Description = TEXT("Clear out the bandit camp");
		Quest.ExperienceReward = 500.0f;
		Quest.GoldReward = 100;
		break;
	case 1:
		Quest.Title = TEXT("Collect Herbs");
		Quest.Description = TEXT("Gather 5 healing herbs");
		Quest.ExperienceReward = 250.0f;
		Quest.GoldReward = 50;
		{
			FQuestObjective Obj;
			Obj.Description = TEXT("Collect herbs");
			Obj.TargetCount = 5;
			Quest.Objectives.Add(Obj);
		}
		break;
	default:
		Quest.Title = TEXT("Mystery Quest");
		Quest.Description = TEXT("Complete this mysterious task");
		Quest.ExperienceReward = 300.0f;
		Quest.GoldReward = 75;
	}

	return Quest;
}

FQuestData* AQuestSystem::FindActiveQuest(uint8 QuestID)
{
	for (FQuestData& Quest : ActiveQuests)
	{
		if (Quest.QuestID == QuestID)
		{
			return &Quest;
		}
	}
	return nullptr;
}

void AQuestSystem::BroadcastQuestStatusChanged(uint8 QuestID, EQuestStatus NewStatus)
{
	OnQuestStatusChanged.Broadcast(QuestID, NewStatus);
}

void AQuestSystem::BroadcastQuestObjectiveUpdated(uint8 QuestID)
{
	OnQuestObjectiveUpdated.Broadcast(QuestID);
}
