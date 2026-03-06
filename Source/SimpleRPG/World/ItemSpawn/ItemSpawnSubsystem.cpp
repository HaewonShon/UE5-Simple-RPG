// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSpawnSubsystem.h"
#include "ItemActor.h"
#include "GoldDropActor.h"
#include "DropActorData.h"
#include "Shared/Item/ItemDatabaseSubsystem.h"
#include "Core/SimpleRPGGameInstance.h"
#include "Core/SimpleRPGAssetManager.h" // item

DEFINE_LOG_CATEGORY(LogCombatRewardSystem);

UItemSpawnSubsystem::UItemSpawnSubsystem()
{
}

void UItemSpawnSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	ReadLootTable();
	LoadDropActorData();
}

void UItemSpawnSubsystem::SpawnDropFromEnemy(FGameplayTag EnemyTag, FVector Location)
{
	UE_LOG(LogCombatRewardSystem, Verbose, TEXT("Item Spawn called for enemy %s"), *EnemyTag.ToString());
	if (!EnemyTag.MatchesTag(FGameplayTag::RequestGameplayTag("Enemy")))
	{
		UE_LOG(LogCombatRewardSystem, Warning, TEXT("Given Enemy Tag is not valid. Tag: %s"), *EnemyTag.ToString());
		return;
	}

	// Drop Item
	UItemData* ItemData = SelectRandomItem(EnemyTag);
	if (ItemData)
	{
		FItemInstance ItemInstance;
		ItemInstance.SetItem(ItemData);

		AItemActor* ItemActorInWorld = GetWorld()->SpawnActor<AItemActor>(DropActorAsset->ItemActor, Location, FRotator());
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
	AGoldDropActor* GoldDropActorInWorld = GetWorld()->SpawnActor<AGoldDropActor>(DropActorAsset->GoldDropActor, Location, FRotator());
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

void UItemSpawnSubsystem::SpawnItem(const FItemInstance& ItemInstance, FVector Location)
{
	if (ItemInstance.DataAsset == nullptr)
	{
		UE_LOG(LogCombatRewardSystem, Warning, TEXT("SpawnItem: Given ItemInstance is not valid"));
		return;
	}

	AItemActor* ItemActorInWorld = GetWorld()->SpawnActor<AItemActor>(DropActorAsset->ItemActor, Location, FRotator());
	if (ItemActorInWorld)
	{
		ItemActorInWorld->SetItem(ItemInstance);
		ItemActorInWorld->SetPickupDelay(ItemPickupDelay);
		UE_LOG(LogCombatRewardSystem, Verbose, TEXT("Item Actor spawned: %s"), *(ItemInstance.DataAsset->DisplayName.ToString()));
	}
	else
	{
		UE_LOG(LogCombatRewardSystem, Warning, TEXT("Failed to spawn Item Actor"));
	}
}

void UItemSpawnSubsystem::ReadLootTable()
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

UItemData* UItemSpawnSubsystem::SelectRandomItem(FGameplayTag EnemyTag) const
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

int32 UItemSpawnSubsystem::GetRandomGoldAmount(FGameplayTag EnemyTag) const
{
	const FLootInfo& LootInfo = EnemyLootInfoCache.FindRef(EnemyTag);
	return FMath::RandRange(LootInfo.MinGoldAmount, LootInfo.MaxGoldAmount);
}

void UItemSpawnSubsystem::LoadDropActorData()
{
	UAssetManager& Manager = UAssetManager::Get();

	// 1. 특정 타입의 모든 에셋 리스트를 가져오거나, 특정 ID를 직접 지정
	FPrimaryAssetId TargetId = FPrimaryAssetId("DropActorData", FName("DA_DropActor"));

	// 2. 경로 정보(SoftObjectPath) 가져오기 (메모리 로드 X, 경로만 확보)
	FSoftObjectPath AssetPath = Manager.GetPrimaryAssetPath(TargetId);
	UE_LOG(LogTemp, Log, TEXT("Drop actors Load request! %s"), *AssetPath.ToString());

	// 3. 필요할 때 비동기 로드 시작
	Manager.LoadPrimaryAsset(TargetId, TArray<FName>(), FStreamableDelegate::CreateUObject(this, &UItemSpawnSubsystem::OnDropActorDataLoaded));
}

void UItemSpawnSubsystem::OnDropActorDataLoaded()
{
	FPrimaryAssetId TargetId = FPrimaryAssetId("DropActorData", FName("DA_DropActor"));
	DropActorAsset = UAssetManager::Get().GetPrimaryAssetObject<UDropActorData>(TargetId);

	UE_LOG(LogTemp, Log, TEXT("Drop Actors Loaded Successfully! %i"), DropActorAsset != nullptr);
}
