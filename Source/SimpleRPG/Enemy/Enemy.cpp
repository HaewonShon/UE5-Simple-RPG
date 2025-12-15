// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "../Gameplay/CharacterAttributeSet.h"
#include "../UI/EnemyHPDisplayWidgetComponent.h"
#include "../Gameplay/DamageTextActor.h"
#include "../Item/ItemLootSubsystem.h"

DEFINE_LOG_CATEGORY(LogEnemy);
// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	AttributeSet = CreateDefaultSubobject<UCharacterAttributeSet>(TEXT("AttributeSet"));
	AbilitySystemComponent->AddAttributeSetSubobject(AttributeSet.Get());

	HPDisplayWidgetComponent = CreateDefaultSubobject<UEnemyHPDisplayWidgetComponent>(TEXT("HPDisplayWidget"));
	HPDisplayWidgetComponent->SetupAttachment(RootComponent);
	HPDisplayWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		// ASC init
		InitializeAttributes();
		AddCharacterAbilities();

		FGameplayAttribute HealthAttribute = UCharacterAttributeSet::GetHealthAttribute();
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HealthAttribute).AddUObject(this, &AEnemy::OnHealthChanged);
	}
}

void AEnemy::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	float Damage = Data.OldValue - Data.NewValue;
	if(Damage > 0)
	{ 
		ADamageTextActor* DamageTextActor = GetWorld()->SpawnActor<ADamageTextActor>(DamageTextClass, GetActorLocation() + FVector(0.f, 0.f, 200.f), GetActorRotation());
		if (DamageTextActor)
		{
			DamageTextActor->SetDamageValue(Damage, false);
		}
		else
		{
			UE_LOG(LogEnemy, Warning, TEXT("Failed to set damage value for damageactor"));
		}

		if (Data.NewValue <= 0.f)
		{
			OnDeath();
		}
	}
}

void AEnemy::OnDeath()
{
	if (DeathAnimMontage)
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			FOnMontageEnded EndDelegate;
			EndDelegate.BindLambda([this](UAnimMontage* Montage, bool bInterrupted) { Destroy(); });

			AnimInstance->Montage_Play(DeathAnimMontage);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, DeathAnimMontage);
			OnEnemyDeath.Broadcast();
		}
		else
		{
			UE_LOG(LogEnemy, Warning, TEXT("%s cannot find EnemyAnimInstance"), *GetFName().ToString());
		}
	}

	// Item Drop Request
	if (UItemLootSubsystem* ItemLootSubsystem = GetGameInstance()->GetSubsystem<UItemLootSubsystem>())
	{
		UE_LOG(LogEnemy, Log, TEXT("Item spawn requested"));
		ItemLootSubsystem->SpawnItem(EnemyTag, GetActorLocation());
	}
}

void AEnemy::InitializeAttributes()
{
	if (!AbilitySystemComponent.Get() || !DefaultAttributeSet.Get())
		return;

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(*DefaultAttributeSet, 1.0f, EffectContext);
	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
	}
}

void AEnemy::AddCharacterAbilities()
{
	if (!AbilitySystemComponent)
		return;

	for (TSubclassOf<UGameplayAbility>& Ability : OwningAbilities)
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability, 1, -1, this));
	}
}

UAbilitySystemComponent* AEnemy::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}