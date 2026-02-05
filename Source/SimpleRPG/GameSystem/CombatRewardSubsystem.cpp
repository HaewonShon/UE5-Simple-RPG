// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatRewardSubsystem.h"
#include "ItemActor.h"
#include "GoldDropActor.h"
#include "../Item/ItemDatabaseSubsystem.h"
#include "../SimpleRPGGameInstance.h"
#include "../SimpleRPGAssetManager.h" // item

DEFINE_LOG_CATEGORY(LogCombatRewardSystem);

UCombatRewardSubsystem::UCombatRewardSubsystem()
{
	static ConstructorHelpers::FClassFinder<AItemActor> ItemActorBP(TEXT("/Game/Gameplay/BP_ItemActor2.BP_ItemActor2_C"));
	if (ItemActorBP.Succeeded())
	{
		ItemActor = ItemActorBP.Class;
		UE_LOG(LogCombatRewardSystem, Warning, TEXT("Could not find Game/Gameplay/BP_ItemActor2.BP_ItemActor2_C2 %i"), ItemActor.Get() != nullptr);
	}
	else
	{
		UE_LOG(LogCombatRewardSystem, Warning, TEXT("Could not find Game/Gameplay/BP_ItemActor2.BP_ItemActor2_C"));
	}

	static ConstructorHelpers::FClassFinder<AGoldDropActor> GoldDropActorBP(TEXT("/Game/Gameplay/BP_GoldDropActor.BP_GoldDropActor_C"));
	if (GoldDropActorBP.Succeeded())
	{
		GoldDropActor = GoldDropActorBP.Class;
		UE_LOG(LogCombatRewardSystem, Warning, TEXT("Could not find Game/Gameplay/BP_GoldDropActor.BP_GoldDropActor_C2 %i"), GoldDropActor.Get() != nullptr);
	}
	else
	{
		UE_LOG(LogCombatRewardSystem, Warning, TEXT("Could not find Game/Gameplay/BP_GoldDropActor.BP_GoldDropActor_C"));
	}
}

void UCombatRewardSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	ReadLootTable();
}

void UCombatRewardSubsystem::SpawnDropFromEnemy(FGameplayTag EnemyTag, FVector Location)
{
	UE_LOG(LogCombatRewardSystem, Verbose, TEXT("Item Spawn called for enemy %s"), *EnemyTag.ToString());
	if (!EnemyTag.MatchesTag(FGameplayTag::RequestGameplayTag("Enemy")))
	{
		UE_LOG(LogCombatRewardSystem, Warning, TEXT("Given Enemy Tag is not valid. Tag: %s"), *EnemyTag.ToString());
		return;
	}

	// Drop Item
	const UItemData* ItemData = SelectRandomItem(EnemyTag);
	if (ItemData)
	{
		FItemInstance ItemInstance;
		ItemInstance.SetItem(ItemData);

		AItemActor* ItemActorInWorld = GetWorld()->SpawnActor<AItemActor>(ItemActor, Location, FRotator());
		if (ItemActorInWorld)
		{
			ItemActorInWorld->SetItem(ItemInstance);
			UE_LOG(LogCombatRewardSystem, Verbose, TEXT("Item Actor spawned: %s"), *(ItemData->DisplayName.ToString()));
		}
		else
		{
			UE_LOG(LogCombatRewardSystem, Warning, TEXT("Failed to spawn Item Actor"));
		}
	}
	else
	{
		UE_LOG(LogCombatRewardSystem, Warning, TEXT("Failed to select Item"));
	}
	
	// Drop Gold
	int32 GoldAmount = GetRandomGoldAmount(EnemyTag);
	AGoldDropActor* GoldDropActorInWorld = GetWorld()->SpawnActor<AGoldDropActor>(GoldDropActor, Location, FRotator());
	if (GoldDropActorInWorld)
	{
		GoldDropActorInWorld->SetGoldAmount(GoldAmount);
		UE_LOG(LogCombatRewardSystem, Warning, TEXT("Gold Drop Actor spawned with gold amount: %i"), GoldAmount);
	}
	else
	{
		UE_LOG(LogCombatRewardSystem, Warning, TEXT("Failed to spawn goldactor"));
	}
}

void UCombatRewardSubsystem::SpawnItem(const FItemInstance& ItemInstance, FVector Location)
{
	if (ItemInstance.ItemData == nullptr)
	{
		UE_LOG(LogCombatRewardSystem, Warning, TEXT("SpawnItem: Given ItemInstance is not valid"));
		return;
	}

	AItemActor* ItemActorInWorld = GetWorld()->SpawnActor<AItemActor>(ItemActor, Location, FRotator());
	if (ItemActorInWorld)
	{
		ItemActorInWorld->SetItem(ItemInstance);
		ItemActorInWorld->SetPickupDelay(ItemPickupDelay);
		UE_LOG(LogCombatRewardSystem, Verbose, TEXT("Item Actor spawned: %s"), *(ItemInstance.ItemData->DisplayName.ToString()));
	}
	else
	{
		UE_LOG(LogCombatRewardSystem, Warning, TEXT("Failed to spawn Item Actor"));
	}
}

void UCombatRewardSubsystem::ReadLootTable()
{
	UDataTable* LootTable;
	if (USimpleRPGGameInstance* GameInstance = Cast<USimpleRPGGameInstance>(GetGameInstance()))
	{
		LootTable = GameInstance->GetLootTable().Get();
		UE_LOG(LogCombatRewardSystem, Log, TEXT("Got LootTable from GameInstance, size: %i"), LootTable->GetRowNames().Num());
	}
	else
	{
		UE_LOG(LogCombatRewardSystem, Warning, TEXT("Cannot able to load loot table from game instance."));
		return;
	}

	LootTable->ForeachRow<FLootTableRow>(TEXT("Caching LootTable Info..."), [this](const FName& RowName, const FLootTableRow& Row) {
		UE_LOG(LogCombatRewardSystem, Verbose, TEXT("For Row - EnemyTag %s, ItemList Size: %i"), *(Row.EnemyTag.ToString()), Row.LootItemList.Num());

		FLootInfo& LootInfo = EnemyLootInfoCache.Add({ Row.EnemyTag, FLootInfo() });
		for (const FLootItem& Item : Row.LootItemList)
		{
			LootInfo.Item.Add(Item);
			LootInfo.TotalWeight += Item.Weight;
			++LootInfo.ItemCount;
		}
		
		LootInfo.MinGoldAmount = Row.Gold.X;
		LootInfo.MaxGoldAmount = Row.Gold.Y;
	});
}

const UItemData* UCombatRewardSubsystem::SelectRandomItem(FGameplayTag EnemyTag) const
{
	UE_LOG(LogCombatRewardSystem, Verbose, TEXT("SelectRandomItem with enemy tag: %s"), *(EnemyTag.ToString()));
	UItemDatabaseSubsystem* ItemDB = GetGameInstance()->GetSubsystem<UItemDatabaseSubsystem>();
	const FLootInfo& LootInfo = EnemyLootInfoCache.FindRef(EnemyTag);

	if (LootInfo.ItemCount == 0)
	{
		UE_LOG(LogCombatRewardSystem, Warning, TEXT("LootTable for %s is empty."), *(EnemyTag.ToString()));
		return nullptr;
	}

	float RandomValue = FMath::RandRange(0.f, LootInfo.TotalWeight);
	float Sum = 0.f;

	for (int32 Index = 0; Index < LootInfo.ItemCount; ++Index)
	{
		const FLootItem& Item = LootInfo.Item[Index];
		Sum += Item.Weight;
		if (Sum >= RandomValue)
		{
			return ItemDB->Get(Item.ItemId);
		}
	}

	return ItemDB->Get(LootInfo.Item[LootInfo.ItemCount-1].ItemId);
}

int32 UCombatRewardSubsystem::GetRandomGoldAmount(FGameplayTag EnemyTag) const
{
	const FLootInfo& LootInfo = EnemyLootInfoCache.FindRef(EnemyTag);
	return FMath::RandRange(LootInfo.MinGoldAmount, LootInfo.MaxGoldAmount);
}