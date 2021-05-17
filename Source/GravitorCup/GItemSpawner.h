// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GItemSpawner.generated.h"

UCLASS()
class GRAVITORCUP_API AGItemSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGItemSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="ItemSpawner")
	class USceneComponent* ItemSpawnTransform;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="ItemSpawner")
	TSubclassOf<class AGItem>  ItemClass;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="ItemSpawner")
	class AGItem* SpawnedItem;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="ItemSpawner")
	float AfterItemDestroyRespawnDelay;

	FTimerHandle AfterItemDestroyRespawnTimerHandle;

	void MarkForRespawn();

	void RespawnItem();

};
