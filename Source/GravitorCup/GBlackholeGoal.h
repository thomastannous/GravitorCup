// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "GBlackholeGoal.generated.h"

UCLASS()
class GRAVITORCUP_API AGBlackholeGoal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGBlackholeGoal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Blackhole")
	class USphereComponent *GoalArea;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gravity")
	float GravityRadius;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gravity")
	float GravityPower;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Team")
	int32 DefensingTeam;
	
	void ApplyGravityToAttractableObjects();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TArray<class AGBall*> AttractableObjects;

	UFUNCTION()
	void OnGoalAreaOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

};
