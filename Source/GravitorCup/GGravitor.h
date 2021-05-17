// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAttractableInterface.h"
#include "GameFramework/Pawn.h"
#include "Engine/DataTable.h"
#include "GGravitor.generated.h"

class UGGravitorMovementComponent;



UCLASS()
class GRAVITORCUP_API AGGravitor : public APawn, public IGAttractableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGGravitor();

	UFUNCTION(BlueprintCallable, CallInEditor, Category="Gravitor | General")
	void LoadGravitorMechanicParamtersFromTable();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gravitor | General")
	FDataTableRowHandle GravitorParameterRowHandle;

	void ResetGravitor();
	void SetToStandardHeight();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gravitor | General")
	int32 Team;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gravitor | General")
	float MaxOnHitPossessionTransferAmount;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:


	UPROPERTY(BlueprintReadWrite, Category="Gravitor")
	bool bGameInputDisabled;

	// Placeholder
	UPROPERTY(BlueprintReadWrite, Category="Placeholder")
	FLinearColor GravitorColor;

	UPROPERTY(BlueprintReadOnly, Category="Gravitor | Movement")
	FVector LastFrameVelocity;
	
	// Movement
	UPROPERTY(BlueprintReadWrite, EditAnywhere,  Category="Gravitor | Movement")
	float Accel;
	UPROPERTY(BlueprintReadWrite, EditAnywhere,  Category="Gravitor | Movement")
	float GroundFriction;
	UPROPERTY(BlueprintReadWrite, EditAnywhere,  Category="Gravitor | Movement")
	float MaxSpeed;
	UPROPERTY(BlueprintReadWrite, EditAnywhere,  Category="Gravitor | Movement")
	float SingleGravityOwnAffectionFactor;
	UPROPERTY(BlueprintReadWrite, EditAnywhere,  Category="Gravitor | Movement")
	float DoubleGravityOwnAffectionFactor;
	UPROPERTY(BlueprintReadWrite, EditAnywhere,  Category="Gravitor | Movement")
	float MaxSpeedGravityInactive;
	UPROPERTY(BlueprintReadWrite, EditAnywhere,  Category="Gravitor | Movement")
	float DecelToMaxSpeedFriction;
	

	// Gravity	
	
	UPROPERTY(BlueprintReadOnly,  Category="Gravitor | Gravity ")
	float BallDistance;
	UPROPERTY(BlueprintReadOnly,  Category="Gravitor | Gravity ")
	bool bIsAttractingBall;
	UPROPERTY(BlueprintReadOnly, Category="Gravitor")
	bool bIsOwningABall;

	
	UPROPERTY(BlueprintReadWrite, EditAnywhere,  Category="Gravitor | Gravity")
	float GravityPower;
	UPROPERTY(BlueprintReadWrite, EditAnywhere,  Category="Gravitor | Gravity")
	float DoubleGravityFactor;	
	UPROPERTY(BlueprintReadOnly,  Category="Gravitor | Gravity")
	float VortexRightInput;
	UPROPERTY(BlueprintReadOnly,  Category="Gravitor | Gravity")
	float VortexLeftInput;
	UPROPERTY(BlueprintReadWrite, EditAnywhere,  Category="Gravitor | Gravity")
	float OwnAffectionFactor;
	UPROPERTY(BlueprintReadWrite, EditAnywhere,  Category="Gravitor | Gravity")
	float GravityRadius;
	UPROPERTY(BlueprintReadWrite, EditAnywhere,  Category="Gravitor | Gravity")
	float MinGravityRadius;
	UPROPERTY(BlueprintReadWrite, EditAnywhere,  Category="Gravitor | Gravity")
	float MaxGravityRadius;
	/* Ranges from 0 to 1 */	
	UPROPERTY(BlueprintReadOnly,  Category="Gravitor | Gravity")
	float GravityHold;
	UPROPERTY(BlueprintReadWrite, EditAnywhere,  Category="Gravitor | Gravity")
	float GravityIncreaseSpeed;
	UPROPERTY(BlueprintReadWrite, EditAnywhere,  Category="Gravitor | Gravity")
	float GravityDecreaseSpeed;
	
	
	UPROPERTY(BlueprintReadOnly, Category="Gravitor | Gravity")
	bool bIsGravityActive;


	// Spin
	UPROPERTY(BlueprintReadWrite, EditAnywhere,  Category="Gravitor | Spin")
	float SpinAccelPower;

	UPROPERTY(BlueprintReadOnly,  Category="Gravitor | Spin")
	FVector2D CurrentSpinInput;

	// Boost
	bool BoostImpulsePressed;
	bool BoostPressed;

	UPROPERTY(BlueprintReadOnly, Category="Gravitor | Boost")
	bool bCanBoost;
	
	UPROPERTY(BlueprintReadWrite, Category="Gravitor | Boost")
	bool bInBoostLoadArea;
	
	UPROPERTY(EditAnywhere, Category="Gravitor | Boost")
	float  BoostFuelUpSpeed;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere,  Category="Gravitor | Boost")
	float Boost;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere,  Category="Gravitor | Boost")
	float BoostDiminishSpeed;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere,  Category="Gravitor | Boost")
	float BoostImpulsePower;

	UPROPERTY(BlueprintReadWrite, EditAnywhere,  Category="Gravitor | Boost")
	float BoostReloadTimer;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere,  Category="Gravitor | Boost")
	float BoostReloadTime;

	UPROPERTY(BlueprintReadWrite, EditAnywhere,  Category="Gravitor | Boost")
	float BoostPower;

	// Hit
	UPROPERTY(BlueprintReadOnly, Category="Gravitor | Hit")
	bool Stunned;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gravitor | Hit")
	float StunTime;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gravitor | Hit")
	float MinStunTime;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gravitor | Hit")
	float MaxStunTime;
	UPROPERTY(BlueprintReadOnly, Category="Gravitor | Hit")
	float StunTimer;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gravitor | Hit")
	float MinImpulse;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gravitor | Hit")
	float MaxImpulse;

	// Tilt to Acceleration Prediction 
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gravitor | Tilt")
	float VelocityToTiltFactor;	
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gravitor | Tilt")
	float TiltLerpSpeed;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gravitor | Tilt")
	int32 RadialDeltaHistoryLength;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gravitor | Tilt")
	int32 TiltPredictionLength;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gravitor | Tilt")
	float MaxTiltAngle;
	
	UPROPERTY(BlueprintReadOnly, Category="Gravitor | Tilt")
	FQuat TargetTiltRotation;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gravitor | Item")
	int32 MaxCollectableItemNum;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gravitor | Item")
	FVector ItemDetachOffset;

	TArray<float> RadialDeltaHistory;
	
	TArray<AActor*> AttractableObjects;

	virtual void AddGravityForce(FVector Force, float Distance) override;

	virtual void AddLeftVortexInput(float Value);
	virtual void AddRightVortexInput(float Value);

	void ClearInput();
	
	virtual void AddGravityInput(float Value);
	
	virtual void ActivateBoost();
	virtual void DeactivateBoost();
	virtual void ActivateBoostImpulse();

	UPROPERTY(BlueprintReadWrite, EditAnywhere,  Category="Gravitor | Debug")
	bool bDrawDebug;

	void DrawDebugThings(float DeltaTime);
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void HandleBoost(float DeltaTime, FVector InputVector);

	virtual void ApplySpinAndGravityToAttractableObjects(float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent, Category="Gravitor")
	void OnGravityActivate();
	
	UFUNCTION(BlueprintImplementableEvent, Category="Gravitor")
	void OnGravityDeactivate();

	UFUNCTION(BlueprintCallable, Category="Gravitor")
	void CollectItem(class AGItem *Item);
	
	UFUNCTION(Category="Gravitor")
	void UseNextCollectedItem();

	UPROPERTY(BlueprintReadOnly, Category="Gravitor")
	TArray<AGItem*> CollectedItems;


	UFUNCTION()
	void OnGravitorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	virtual void HandleStun(float DeltaTime);
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	FVector CalcFrictionForce(FVector CurrentVelocity, float Friction, float DeltaTime);
	void DecelToMaxVelocity(FVector CurrentVelocity, float MaxSpeed);

	void AddSpinInput(FVector2D Input);

	void TiltGravitorToVelocityPrediction(float DeltaTime, FVector OldVelocity, FVector NewVelocity);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gravitor)
	USkeletalMeshComponent *RingMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gravitor)
	UStaticMeshComponent *BallMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gravitor)
	class UCapsuleComponent *CollisionCapsule;
};

