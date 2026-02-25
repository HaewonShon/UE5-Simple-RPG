// Fill out your copyright notice in the Description page of Project Settings.


#include "UISubsystem.h"
#include "CommonWidgetData.h"
#include "Core/SimpleRPGAssetManager.h"
#include "Shared/UI/UISubsystem.h"
#include "Shared/Item/UI/QuantityConfirmationWidget.h"
#include "MessageBox.h"

void UUISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LoadWidgetData();
}

void UUISubsystem::InitializeRootWidget(APlayerController* PC, TSubclassOf<class URootWidget> RootWidgetClass)
{
	RootWidget = CreateWidget<URootWidget>(PC, RootWidgetClass.Get());
	RootWidget->AddToViewport();
}

UUserWidget* UUISubsystem::AddWidgetToLayer(EWidgetLayer Layer, TSubclassOf<class UUserWidget> WidgetClass, bool bFillScreen, bool bIsAliveAways)
{
	return RootWidget->AddWidgetToLayer(Layer, WidgetClass, bFillScreen, bIsAliveAways);
}

void UUISubsystem::ToggleInventory()
{
	RootWidget->ToggleInventory();
}

UQuantityConfirmationWidget* UUISubsystem::RequestCreateQuantityWidget()
{
	UQuantityConfirmationWidget* Widget = Cast<UQuantityConfirmationWidget>(AddWidgetToLayer(EWidgetLayer::System, 
		CommonWidgetDataAsset->QuantityConfirmationWidgetClass));

	return Widget;
}

void UUISubsystem::RequestDisplayMessageBox(const FText& Message)
{
	UMessageBox* Widget = Cast<UMessageBox>(AddWidgetToLayer(EWidgetLayer::System,
		CommonWidgetDataAsset->MessageBoxWidgetClass));
	Widget->SetMessage(Message);
}

void UUISubsystem::LoadWidgetData()
{
	UAssetManager& Manager = UAssetManager::Get();

	FPrimaryAssetId TargetId = FPrimaryAssetId("CommonWidgetData", FName("DA_CommonWidgetDataAsset"));
	FSoftObjectPath AssetPath = Manager.GetPrimaryAssetPath(TargetId);

	Manager.LoadPrimaryAsset(TargetId, TArray<FName>(), FStreamableDelegate::CreateUObject(this, &UUISubsystem::OnWidgetDataLoaded));
}

void UUISubsystem::OnWidgetDataLoaded()
{
	FPrimaryAssetId TargetId = FPrimaryAssetId("CommonWidgetData", FName("DA_CommonWidgetDataAsset"));
	CommonWidgetDataAsset = UAssetManager::Get().GetPrimaryAssetObject<UCommonWidgetData>(TargetId);

	if (!CommonWidgetDataAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("Loading Widget Data failed"));
	}
}