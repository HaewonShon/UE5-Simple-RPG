// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/UI/Common/SessionWidget.h"

void USessionWidget::CloseWidget()
{
	OnWidgetClosed.Broadcast();
}