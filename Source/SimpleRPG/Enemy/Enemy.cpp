// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "../Gameplay/CharacterAttributeSet.h"
#include "../UI/EnemyHPDisplayWidgetComponent.h"

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