// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemLootSubsystem.h"
#include "ItemActor.h"
#include "ItemDatabaseSubsystem.h"
#include "../SimpleRPGGameInstance.h"
#include "../SimpleRPGAssetManager.h" // item

DEFINE_LOG_CATEGORY(LogItemLootSubsystem);

UItemLootSubsystem::UItemLootSubsystem()
{
	static ConstructorHelpers::FClassFinder<AItemActor> BPClass(TEXT("/Game/Items/BP_ItemActor2.BP_ItemActor2_C"));
	if (BPClass.Succeeded())
	{
		ItemActor = BPClass.Class;
	}
	else
	{
		UE_LOG(LogItemLootSubsystem, Warning, TEXT("Could not find Game/Items/BP_ItemActor2.BP_ItemActor2_C"));
	}
}

void UItemLootSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	ReadLootTable();
}

void UItemLootSubsystem::SpawnItem(FGameplayTag EnemyTag, FVector Location)
{
	UE_LOG(LogItemLootSubsystem, Log, TEXT("Spawn called"));
	if (!EnemyTag.MatchesTag(FGameplayTag::RequestGameplayTag("Enemy")))
	{
		UE_LOG(LogItemLootSubsystem, Warning, TEXT("Given Enemy Tag is not valid. Tag: %s"), *EnemyTag.ToString());
		return;
	}

	const UItemData* ItemData = SelectRandomItem(EnemyTag);
	if (ItemData)
	{
		FItemInstance ItemInstance;
		ItemInstance.SetItem(ItemData);

		AItemActor* ItemActorInWorld = GetWorld()->SpawnActor<AItemActor>(ItemActor, Location, FRotator());
		if (ItemActorInWorld)
		{
			ItemActorInWorld->SetItem(ItemInstance);
			UE_LOG(LogItemLootSubsystem, Log, TEXT("Item spawned: %s"), *(ItemData->DisplayName.ToString()));
		}
		else
		{
			UE_LOG(LogItemLootSubsystem, Warning, TEXT("Failed to spawn Item Actor"));
		}
	}
	else
	{
		UE_LOG(LogItemLootSubsystem, Warning, TEXT("Failed to select Item"));
	}
}

void UItemLootSubsystem::ReadLootTable()
{
	UDataTable* LootTable;
	if (USimpleRPGGameInstance* GameInstance = Cast< USimpleRPGGameInstance>(GetGameInstance()))
	{
		LootTable = GameInstance->GetLootTable().Get();
		UE_LOG(LogItemLootSubsystem, Log, TEXT("Got LootTable from GameInstance, size: %i"), LootTable->GetRowNames().Num());
	}
	else
	{
		UE_LOG(LogItemLootSubsystem, Warning, TEXT("Cannot able to load loot table from game instance."));
		return;
	}

	LootTable->ForeachRow<FLootTableRow>(TEXT("Caching LootTable Info..."), [this](const FName& RowName, const FLootTableRow& Row) {
		UE_LOG(LogItemLootSubsystem, Log, TEXT("For Row - EnemyTag %s, ItemList Size: %i"), *(Row.EnemyTag.ToString()), Row.LootItemList.Num());
		FLootInfo& LootInfo = Cache.Add({ Row.EnemyTag, FLootInfo() });
		for (const FLootItem& Item : Row.LootItemList)
		{
			LootInfo.Item.Add(Item);
			LootInfo.TotalWeight += Item.Weight;
			++LootInfo.ItemCount;
		}
	});

	UE_LOG(LogItemLootSubsystem, Log, TEXT("Item Load 3"));
}

const UItemData* UItemLootSubsystem::SelectRandomItem(FGameplayTag EnemyTag)
{
	UE_LOG(LogItemLootSubsystem, Warning, TEXT("SelectRandomItem with tag: %s"), *(EnemyTag.ToString()));
	UItemDatabaseSubsystem* ItemDB = GetGameInstance()->GetSubsystem<UItemDatabaseSubsystem>();
	const FLootInfo& LootInfo = Cache.FindRef(EnemyTag);

	if (LootInfo.ItemCount == 0)
	{
		UE_LOG(LogItemLootSubsystem, Warning, TEXT("LootTable for %s is empty."), *(EnemyTag.ToString()));
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
			//return USimpleRPGAssetManager::Get().GetItemData(Item.ItemId);
		}
	}

	return ItemDB->Get(LootInfo.Item[LootInfo.ItemCount-1].ItemId);
	//return USimpleRPGAssetManager::Get().GetItemData(LootInfo.Item[LootInfo.ItemCount-1].ItemId);
}
