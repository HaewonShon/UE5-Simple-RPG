// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackHitModule.h"

UAttackHitModule::UAttackHitModule()
{
	ChannelToHit = ECollisionChannel::ECC_GameTraceChannel1;
	TargetClass = APawn::StaticClass();
}
