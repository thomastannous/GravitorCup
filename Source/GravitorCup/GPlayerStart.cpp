// Fill out your copyright notice in the Description page of Project Settings.


#include "GPlayerStart.h"
#include "Components/CapsuleComponent.h"

AGPlayerStart::AGPlayerStart(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    GetCapsuleComponent()->InitCapsuleSize(60.f,30.f);
}
