// Fill out your copyright notice in the Description page of Project Settings.


#include "GItemSpawner.h"
#include "GItem.h"

// Sets default values
AGItemSpawner::AGItemSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemSpawnTransform = CreateDefaultSubobject<USceneComponent>(TEXT("ItemSpawnTransform"));
	RootComponent = ItemSpawnTransform;
	
}

// Called when the game starts or when spawned
void AGItemSpawner::BeginPlay()
{
	Super::BeginPlay();

	RespawnItem();
}

// Called every frame
void AGItemSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGItemSpawner::MarkForRespawn()
{
	UWorld *World = GetWorld();
	if(World)
	{
		GetWorldTimerManager().SetTimer(AfterItemDestroyRespawnTimerHandle, this,  &AGItemSpawner::RespawnItem, AfterItemDestroyRespawnDelay,false);
	}
}

void AGItemSpawner::RespawnItem()
{
	UWorld *World = GetWorld();
	
	if(World)
	{
		AGItem *Item = World->SpawnActor<AGItem>(ItemClass, ItemSpawnTransform->GetComponentToWorld());
		if(Item)
		{
			SpawnedItem = Item;
			Item->ItemSpawner = this;
		}
	}
}

