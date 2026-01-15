// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "InputAction.h"
#include "Abilities/GameplayAbility.h"
#include "../Item/ItemData.h"
#include "PlayerCharacter.generated.h"

// Define Log Category for character-specific logs
DECLARE_LOG_CATEGORY_EXTERN(LogCharacter, Log, All)

USTRUCT(BlueprintType)
struct FWeaponAbilitySet
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FGameplayTag WeaponTag;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TArray<TSubclassOf<class USimpleRPGGameplayAbility>> Abilities;
};

UCLASS()
class SIMPLERPG_API APlayerCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()


public:
	/*******************************************
	*	BASE COMPONENTS FOR CHARACTER & Interface
	*******************************************/
	APlayerCharacter();

	virtual void PossessedBy(AController* NewController);

	bool AddItem(FItemInstance& Item);

	void SetInteractableNPC(class ANPCCharacter* NPC);
	void ClearInteractableNPC(class ANPCCharacter* NPC);

	/*******************************************
	*	Input
	*******************************************/
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* DefaultMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* AttackAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* DashAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* ConsumeAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* SkillAction;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* Interaction;

	/*******************************************
	*	GAMEPLAY ABILITIY SYSTEM
	*******************************************/
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	void OnHealthChanged(const FOnAttributeChangeData& Data);


protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	class USpringArmComponent* SpringArm;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MinZoomLength = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MaxZoomLength = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float DefaultArmLength = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float ZoomStep = 40.f;

	/*******************************************
	*	INPUT
	*******************************************/
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Attack();
	void Dash();
	void SkillAttack();

	/*******************************************
	*	GAMEPLAY ABILITIY SYSTEM
	*******************************************/
	void InitializeAttributes();
	void AddCharacterAbilities();
	void AddWeaponAbilities();

	TWeakObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Abilities")
	TArray<TSubclassOf<class USimpleRPGGameplayAbility>> CommonAbilitySet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Abilities")
	TArray<FWeaponAbilitySet> WeaponAbilitySets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributeSet;

	// used to manage weapon-specific ability managment
	TArray<FGameplayAbilitySpecHandle> WeaponAbilitySpecHandles;

	/**********************
	 *	Others
	************************/
	TWeakObjectPtr<class ANPCCharacter> InteractableNPC;
	void Interact();
};
