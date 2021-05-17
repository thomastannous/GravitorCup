// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAttractableInterface.h"
#include "GameFramework/Actor.h"
#include "GBall.generated.h"

USTRUCT(BlueprintType)
struct FGravityForceParams
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FVector GravityForce;

	UPROPERTY()
	float Distance;

	FGravityForceParams(): GravityForce(FVector::ZeroVector), Distance(0)
	{
	}

	FGravityForceParams(FVector InGravityForce, float InDistance)
	{
		GravityForce = InGravityForce;
		Distance = InDistance;
	}
};

UCLASS()
class GRAVITORCUP_API AGBall : public AActor, public IGAttractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGBall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void Setup();

	/* Sets the balls transform and other transient attributes to their original value.
	 * Mostly called before a round starts.
	 */
	virtual void ResetBall();
	UFUNCTION(BlueprintImplementableEvent, Category="Ball", DisplayName="ResetBall")
	void OnResetBall();
	void StopBall();

	/* Gravity forces that are pending to be applied in the next frame */
	UPROPERTY(BlueprintReadOnly, Category="Ball")
	TArray<FGravityForceParams> PendingGravityForces;

	/* Spin forces that are pending to be applied in the next frame */
	UPROPERTY(BlueprintReadOnly, Category="Ball")
	TArray<FVector> PendingSpinForces;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Ball")
	bool bDrawDebug;

	/* Interface function to make it attractable by other entities that can attract */
	virtual void AddGravityForce(FVector Force, float Distance) override;

	UPROPERTY(BlueprintReadWrite, Category="Ball")
	bool bMovementDisabled;

	/* Team that is in control over the ball */
	UPROPERTY(BlueprintReadWrite, Category="Ball | Possession")
	int32 PossessingTeam;
	
	/* Used in order to know when the PossessingTeam changed */	
	UPROPERTY(BlueprintReadWrite, Category="Ball | Possession")
	int32 OldPossessingTeam;

	/* How much time is left until the ball is expired */
	UPROPERTY(BlueprintReadOnly)
	float Expiration;

	/* The maximum time left until the ball expires */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Ball | Possession")
	float ExpirationTime;	

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Ball | Possession")
	bool bIsSingleTestMode;

	/* For SingleTestMode */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Ball | Possession")
	float BallReturnForceFactor;

	/*
	 * @param Team which team has now possession. 0=left team, 1=right team
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnBallChangePossession(int32 Team);

	/* For SingleTestMode */
	void ApplyBallReturnForce();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category=Ball)
	class UStaticMeshComponent *BallMesh;

	/* Yaw Orientator responsible for rotating the rings yaw wise. */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category=Ball)
	USceneComponent *YawOrientator;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category=Ball)
	class UStaticMeshComponent *InnerRing;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category=Ball)
	class UStaticMeshComponent *OuterRing;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category=Ball)
	class USphereComponent *CollisionSphere;

	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Ball | Movement")
	float MaxSpeed;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Ball | Movement")
	float DecelToMaxSpeedFriction;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Ball | Movement")
	float Friction;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Ball | Movement")
	float FrictionGround;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Ball | Movement")
	float FrictionLift;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Ball | Movement")
	FVector LastFrameVelocity;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Ball | Movement")
	float DeadlyVelocityMagnitudeThreshold;
	/* Whether this ball can destroy other gravitors */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Ball | Movement")
	bool bIsDeadly;
	
	/*
	 * When multiple gravitors apply gravity, gravity forces might be huge.
	 * This can be used to limit the max gravity per frame.
	 */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Ball | Gravity")
	float MaxPerFrameGravity;


	
		
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Ball | Lift")
	float ForceToLiftFactor;
	
	/*
	 * How much the ball is lifted in a continuous manner.
	 * When the ball is lifted less friction is applied, but spin forces cannot be applied
	 */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Ball | Lift")
	float Lift;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Ball | Lift")
	float MinLift;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Ball | Lift")
	float MaxLift;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Ball | Lift")
	float LiftThreshold;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Ball | Lift")
	float ReduceLiftSpeed;
	/* Whether this ball is lifted from ground in a discrete manner. Determines most of the lift dependent logic */
	UPROPERTY(BlueprintReadOnly, Category="Ball | Lift")
	bool Lifted;

	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Ball | Spin")
	FVector SpinVelocity;
	
	/* Controls how SpinVelocity translates to movement velocity */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Ball | Spin")
	float AddingToVelocityFactor;
	
	/* Constant force to apply when the ball still has spin left*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Ball | Spin")
	float SpinForce;
	/* Friction that is applied to SpinVelocity depends on whether ball is on ground or lifted */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Ball | Spin")
	float SpinFriction;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Ball | Spin")
	float SpinFrictionGround;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Ball | Spin")
	float SpinFrictionLifted;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Ball | Spin")
	float RingSpinSpeed;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Ball | Spin")
	float MaxSpinSpeed;
	UPROPERTY(BlueprintReadOnly, Category="Ball | Spin")
	bool bIsLoadingSpin;


	/* Calculates the friction to apply */
	FVector CalcFrictionForce(FVector CurrentVelocity, float DeltaTime);
	
	/* Consumes Gravity Forces that were applied by other entities (i.e. gravitors or the wall)*/
	virtual void ConsumeGravityForces(float DeltaTime);
	
	/* Decelerates the ball to the MaxSpeed using DecelToMaxSpeedFriction */
	void DecelToMaxVelocity(FVector CurrentVelocity, float CurMaxSpeed);
	
	/* Consumes Gravity Forces that were applied by other entities (i.e. gravitors or the wall)*/
	virtual void ConsumeSpinForces(float DeltaTime);

	virtual void DecreaseLift(float DeltaTime);

	/* Accelerates the ball when on ground */
	virtual void AccelerateBySpin(float DeltaTime);
	virtual void ApplySpinFriction(float DeltaTime);
	
	UFUNCTION(BlueprintImplementableEvent, Category="Ball")
	void OnLifted();

	/* Called when the Ball transitions from lift to ground*/
	UFUNCTION(BlueprintImplementableEvent, Category="Ball")
	void OnGrounded();

	void SpinRings(float DeltaTime);
	void ResetRings();

	UFUNCTION(BlueprintNativeEvent, Category="Ball")
	void OnBallExpired();
	void OnBallExpired_Implementation();
	
	

	virtual void DrawDebug(float DeltaTime);
};

