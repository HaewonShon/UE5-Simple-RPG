// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageTextWidget.h"
#include "Components/TextBlock.h"

void UDamageTextWidget::InitializeText(float Damage, bool bIsCrit)
{
	DamageText->SetText(FText::FromString(FString::FromInt(FMath::RoundToInt(Damage))));

	if (bIsCrit)
	{
		FSlateFontInfo FontInfo = DamageText->GetFont();
		FontInfo.Size *= 1.1f;
		DamageText->SetColorAndOpacity(CritHitColor);
		DamageText->SetFont(FontInfo);
	}
	else
	{
		DamageText->SetColorAndOpacity(NormalHitColor);
	}
}

void UDamageTextWidget::SetOpacity(float Opacity)
{
	SetRenderOpacity(Opacity);

	//if (DamageText)
	//{
	//	FColor Color = DamageText->GetColorAndOpacity().GetSpecifiedColor().ToFColor(false);
	//	Color.A = Opacity;
	//	DamageText->SetColorAndOpacity(Color);
	//}
}