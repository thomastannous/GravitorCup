// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GArenaCamera.generated.h"

UCLASS()
class GRAVITORCUP_API AGArenaCamera : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGArenaCamera();

	bool bIsEnabled;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="ArenaCamera | Limiting")
	float PositionLimitLeft;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="ArenaCamera | Limiting")
	float PositionLimitRight;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="ArenaCamera | Limiting")
	float PositionLimitTop;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="ArenaCamera | Limiting")
	float PositionLimitBottom;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="ArenaCamera | Zoom")
	float MinDistance;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="ArenaCamera | Zoom")
	float MaxDistance;

	UPROPERTY(BlueprintReadOnly, Category="ArenaCamera | Zoom")
	float CurrentCamZ;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="ArenaCamera | Zoom")
	float MinCamZ;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="ArenaCamera | Zoom")
	float MaxCamZ;
	
	UPROPERTY(BlueprintReadOnly, Category="ArenaCamera | Zoom")
	float CurrentCamY;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="ArenaCamera | Zoom")
	float MinCamY;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="ArenaCamera | Zoom")
	float MaxCamY;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="ArenaCamera | Zoom")
	float SmoothSpeed;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="ArenaCamera | Zoom")
	float AspectRatio;	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Camera boom positioning the camera above the character */
   	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ArenaCamera", meta = (AllowPrivateAccess = "true"))
   	//class USpringArmComponent* CameraBoom;
    
   	/** The camera component for this camera */
   	UPROPERTY(Category="ArenaCamera", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
   	class UCameraComponent* CameraComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetTrackedObjects(TArray<AActor*> NewTrackedObjects);

	void AddTrackedObject(AActor *TrackedObject);

	void AdjustToObjects(float DeltaTime);

	float GetLongestDistanceBetweenAllTracked();

	UPROPERTY()
	TArray<AActor*> TrackedObjects;
};
