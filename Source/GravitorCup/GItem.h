// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"



#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "GItem.generated.h"

UENUM(BlueprintType)
enum class ItemState : uint8
{
	IS_Idle,
	IS_Collected,
	IS_Armed,
	IS_Activated,
	IS_PendingDestroy
}; 

UCLASS()
class GRAVITORCUP_API AGItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GItem")
	UCapsuleComponent *CollisionCapsule;
	
	UPROPERTY(BlueprintReadOnly, Category="GItem")
	class AGItemSpawner *ItemSpawner;
	
	UPROPERTY(BlueprintReadOnly, Category="GItem")
	class AGGravitor *GravitorOwner;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GItem")
	ItemState State;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GItem")
	float TillActivationTime;
	
	UPROPERTY(BlueprintReadOnly, Category="GItem")
	float TillActivationTimer;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GItem")
	bool bIsInstant;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GItem")
	bool bIsCollectable;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GItem")
	bool bCanBeActivatedByCollision;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GItem")
	float EffectDuration;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GItem")
	float EffectDurationTimer;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GItem")
	float Charge;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GItem")
	float AfterEffectTime;
	
	UPROPERTY(BlueprintReadOnly, Category="GItem")
	float AfterEffectTimer;

	
	
	UFUNCTION()
	void OnItemHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
	void OnItemOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION(BlueprintCallable)
	void Arm();

	void Use(FVector DetachOffset);
	
	virtual void Collected(class AGGravitor *GOwner);
	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnCollected")
	void BP_OnCollected();


	virtual void ActivateItem();
	UFUNCTION(BlueprintImplementableEvent, DisplayName="ActivateItem")
	void BP_ActivateItem();
	
	virtual void DeactivateItem();
	UFUNCTION(BlueprintImplementableEvent, DisplayName="DeactivateItem")
	void BP_DeactivateItem();
};
