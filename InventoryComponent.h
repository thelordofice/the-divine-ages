#include "AI/EnemyCharacter.h"
#include "Components/CharacterAttributeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "AIController.h"

AEnemyCharacter::AEnemyCharacter()
	: ExperienceReward(100.0f)
	, GoldReward(50)
	, EnemyType(0)
	, Difficulty(MedievalRPG::EDifficulty::Normal)
	, CurrentTarget(nullptr)
	, DetectionRange(1500.0f)
	, AttackRange(150.0f)
	, AttackDamage(15.0f)
	, AttackCooldown(2.0f)
	, LastAttackTime(0.0f)
{
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->MaxWalkSpeed = 400.0f;

	AttributeComponent = CreateDefaultSubobject<UCharacterAttributeComponent>(TEXT("AttributeComponent"));
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AttributeComponent)
	{
		AttributeComponent->InitializeAttributes(MedievalRPG::ECharacterClass::Warrior);
	}

	// Spawn AI controller
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		if (BehaviorTree)
		{
			AIController->RunBehaviorTree(BehaviorTree);
		}
	}

	RPG_LOG("Enemy spawned: Type %d, Difficulty %d", EnemyType, static_cast<int32>(Difficulty));
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LastAttackTime += DeltaTime;
}

float AEnemyCharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = DamageAmount;

	if (AttributeComponent)
	{
		AttributeComponent->TakeDamage(ActualDamage);

		if (AttributeComponent->IsDead())
		{
			Die();
		}
	}

	return ActualDamage;
}

void AEnemyCharacter::AttackTarget(AActor* Target)
{
	if (!CanAttackTarget(Target))
	{
		return;
	}

	if (LastAttackTime < AttackCooldown)
	{
		return;
	}

	LastAttackTime = 0.0f;
	Target->TakeDamage(AttackDamage, FDamageEvent(), GetController(), this);

	RPG_LOG("Enemy attacked target for %.0f damage", AttackDamage);
}

bool AEnemyCharacter::CanAttackTarget(AActor* Target) const
{
	if (!Target || AttributeComponent->IsDead())
	{
		return false;
	}

	float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
	return Distance <= AttackRange;
}

void AEnemyCharacter::OnTargetDetected(AActor* Target)
{
	CurrentTarget = Target;
	RPG_LOG("Enemy detected target");
}

float AEnemyCharacter::GetHealth() const
{
	return AttributeComponent ? AttributeComponent->GetHealth() : 0.0f;
}

float AEnemyCharacter::GetMaxHealth() const
{
	return AttributeComponent ? AttributeComponent->GetMaxHealth() : 100.0f;
}

bool AEnemyCharacter::IsDead() const
{
	return AttributeComponent && AttributeComponent->IsDead();
}

void AEnemyCharacter::Die()
{
	// Drop items
	for (uint16 ItemID : ItemRewards)
	{
		// Spawn item pickup in world
	}

	// Award player
	if (AActor* PlayerCharacter = Cast<AActor>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
	{
		// Award experience and gold
	}

	Destroy();
	RPG_LOG("Enemy died");
}
