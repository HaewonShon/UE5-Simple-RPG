// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "AbilitySystemComponent.h"
#include "../Gameplay/CharacterAttributeSet.h"
#include "Abilities/PlayerAttackAbilityBase.h"
#include "SimpleRPGPlayerState.h"
#include "Inventory/InventoryComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY(LogCharacter);

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	SpringArm->SetupAttachment(GetCapsuleComponent());
	SpringArm->TargetArmLength = DefaultArmLength;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	Camera->SetupAttachment(SpringArm);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate *= 2.f;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	this->AnimRootMotionTranslationScale = 0.1f;
	this->SetAnimRootMotionTranslationScale(0.1f);
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (ASimpleRPGPlayerState* PS = Cast<ASimpleRPGPlayerState>(GetPlayerState()))
	{
		UE_LOG(LogCharacter, Log, TEXT("PlayerState initialized"));
		AbilitySystemComponent = PS->GetAbilitySystemComponent();
	}

	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		// ASC init
		InitializeAttributes();
		AddCharacterAbilities();

		FGameplayAttribute HealthAttribute = UCharacterAttributeSet::GetHealthAttribute();
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HealthAttribute).AddUObject(this, &APlayerCharacter::OnHealthChanged);

		//temp : add sword tag
		//AbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("Weapon.Sword"));
		//AbilitySystemComponent->gameplaytag
	}
}

bool APlayerCharacter::AddItem(FItemInstance& Item)
{
	if (ASimpleRPGPlayerState* PS = Cast<ASimpleRPGPlayerState>(GetPlayerState()))
	{
		TWeakObjectPtr<UInventoryComponent> InventoryComponent = PS->GetInventoryComponent();
		if (InventoryComponent.IsValid())
		{
			return InventoryComponent->AddItem(Item);
		}
	}
	return false;
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		EIC->BindAction(AttackAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Attack);
		EIC->BindAction(DashAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Dash);
		//EIC->BindAction(ConsumeAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EIC->BindAction(SkillAction, ETriggerEvent::Triggered, this, &APlayerCharacter::SkillAttack);

		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				InputSystem->AddMappingContext(DefaultMapping, 0);
			}
		}
	}
	else
	{
		UE_LOG(LogCharacter, Error, TEXT("PlayerCharacter requires EnhancedInputComponent"));
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.Y);
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	AddControllerYawInput(-LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void APlayerCharacter::Attack()
{
	// Currently in attack -> set next combo if possible
	if (AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Ability.Attack.Combo")))
	{
		if (UPlayerAttackAbilityBase* AttackAbility = Cast<UPlayerAttackAbilityBase>(AbilitySystemComponent->GetAnimatingAbility()))
		{
			AttackAbility->SetNextComboFlag(true);
		}
	}
	else // Otherwise run 1st attack of the combo series
	{
		AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTag::RequestGameplayTag("Ability.Attack.Combo.1").GetSingleTagContainer());
	}
}

void APlayerCharacter::Dash()
{
	AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTag::RequestGameplayTag("Ability.Dash").GetSingleTagContainer());
}

void APlayerCharacter::SkillAttack()
{
	AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTag::RequestGameplayTag("Ability.Attack.Skill").GetSingleTagContainer());
}

void APlayerCharacter::InitializeAttributes()
{
	if (!AbilitySystemComponent.IsValid() || !DefaultAttributeSet.Get())
		return;

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(*DefaultAttributeSet, 1.0f, EffectContext);
	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*NewHandle.Data.Get());
	}
}

void APlayerCharacter::AddCharacterAbilities()
{
	if (!AbilitySystemComponent.IsValid())
		return;

	for (TSubclassOf<USimpleRPGGameplayAbility>& Ability : CommonAbilitySet)
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability, 1, -1, this));
	}

	AddWeaponAbilities();
}

void APlayerCharacter::AddWeaponAbilities()
{
	FGameplayTag CurrentWeaponTag = AbilitySystemComponent->GetOwnedGameplayTags().Filter(FGameplayTag::RequestGameplayTag("Weapon").GetSingleTagContainer()).First();

	for (FWeaponAbilitySet& AbilitySet : WeaponAbilitySets)
	{
		if (AbilitySet.WeaponTag == CurrentWeaponTag)
		{
			for (TSubclassOf<USimpleRPGGameplayAbility>& Ability : AbilitySet.Abilities)
			{
				WeaponAbilitySpecHandles.Add(AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability, 1, -1, this)));
			}
		}
	}
}

UAbilitySystemComponent* APlayerCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

void APlayerCharacter::OnHealthChanged(const FOnAttributeChangeData& Data)
{

}

