// Fill out your copyright notice in the Description page of Project Settings.


#include "GGravitor.h"
#include "Components/CapsuleComponent.h"
#include "GravitorCup.h"
#include "DrawDebugHelpers.h"
#include "GBall.h"
#include "GGravityDestroyGameMode.h"
#include "GPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GItem.h"
#include "Components/SphereComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AGGravitor::AGGravitor()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionCapsule"));
	//CollisionCapsule->SetCapsuleHalfHeight(120.0);
	CollisionCapsule->InitCapsuleSize(60.f, 120.f);
	CollisionCapsule->OnComponentHit.AddDynamic(this, &AGGravitor::OnGravitorHit);

	RingMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RingMesh"));
	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));

	RootComponent = CollisionCapsule;
	BallMesh->SetupAttachment(CollisionCapsule);

	RingMesh->SetupAttachment(CollisionCapsule);

	ResetGravitor();

	bGameInputDisabled = false;

	// Movement
	LastFrameVelocity = FVector::ZeroVector;
	Accel = 1300000.0;
	GroundFriction = 2.2f;
	MaxSpeed = 1500.f;
	MaxSpeedGravityInactive = MaxSpeed;
	SingleGravityOwnAffectionFactor = 1.f;
	DoubleGravityOwnAffectionFactor = 2.f;
	DecelToMaxSpeedFriction = 4.f;

	// Gravity
	GravityPower = 16500.f;
	DoubleGravityFactor = 2.f;
	bIsGravityActive = false;
	OwnAffectionFactor = 1.0f;
	MinGravityRadius = 1200.f;
	GravityRadius = MinGravityRadius;
	MaxGravityRadius = 1500.f;
	GravityIncreaseSpeed = 2.0f;
	GravityDecreaseSpeed = 1.3f;

	// Spin
	SpinAccelPower = 20000.f;

	// Boost
	Boost = 0.5f;
	BoostImpulsePower = 20000.f;
	BoostPower = 4000.f;
	bCanBoost = true;
	bInBoostLoadArea = false;
	BoostDiminishSpeed = 0.5f;
	BoostFuelUpSpeed = 1.f;
	BoostReloadTime = 0.5f;

	// Hit
	Stunned = false;
	MinStunTime = 0.5f;
	MaxStunTime = 2.f;
	StunTime = MinStunTime;
	StunTimer = 0.f;
	MinImpulse = 40.f;
	MaxImpulse = 3000.f;
	MaxOnHitPossessionTransferAmount = 0.2f;

	// Tilt
	TargetTiltRotation = FQuat::Identity;
	TiltPredictionLength = 20;
	MaxTiltAngle = 10.f;

	// Item
	MaxCollectableItemNum = 1;
	ItemDetachOffset = FVector(0.f, 500.f, 0.f);

	Team = -1;

	bDrawDebug = false;

	LoadGravitorMechanicParamtersFromTable();
}

void AGGravitor::LoadGravitorMechanicParamtersFromTable()
{
}


void AGGravitor::ResetGravitor()
{
	Stunned = false;
	SetToStandardHeight();

	RingMesh->SetWorldRotation(FQuat::Identity);
	TargetTiltRotation = FQuat::Identity;
}

void AGGravitor::SetToStandardHeight()
{
	FVector Pos = GetActorLocation();
	Pos.Z = PLAYER_STANDARD_HEIGHT;
	SetActorLocation(Pos);
}

// Called when the game starts or when spawned
void AGGravitor::BeginPlay()
{
	Super::BeginPlay();
}


void AGGravitor::AddGravityForce(FVector Force, float Distance)
{
}

void AGGravitor::AddLeftVortexInput(float Value)
{
	VortexLeftInput = Value;
}

void AGGravitor::AddRightVortexInput(float Value)
{
	VortexRightInput = Value;
}

void AGGravitor::AddGravityInput(float Value)
{
}

void AGGravitor::ActivateBoost()
{
	BoostPressed = true;
}

void AGGravitor::DeactivateBoost()
{
	BoostPressed = false;
}

void AGGravitor::ActivateBoostImpulse()
{
	BoostImpulsePressed = true;
}

void AGGravitor::DrawDebugThings(float DeltaTime)
{
	UWorld* World = GetWorld();
	if (World)
	{
		DrawDebugCircle(World, CollisionCapsule->GetComponentLocation(), GravityRadius, 64, FColor::Red, true,
		                DeltaTime,
		                SDPG_Foreground, 4.f, FVector(0.f, 1.f, 0.f), FVector(1.f, 0.f, 0.f), false);
	}
}

void AGGravitor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	bool bOldIsGravityActive = bIsGravityActive;

	if (bGameInputDisabled)
	{
		CollisionCapsule->SetPhysicsLinearVelocity(FVector::ZeroVector);
		BoostPressed = false;
		BoostImpulsePressed = false;
		VortexLeftInput = 0.f;
		VortexRightInput = 0.f;

		return;
	}

	if (!FMath::IsNearlyZero(VortexLeftInput) || !FMath::IsNearlyZero(VortexRightInput))
	{
		bIsGravityActive = true;
		OwnAffectionFactor = SingleGravityOwnAffectionFactor;
		//MaxSpeed = MaxSpeedGravityActive;
	}
	else
	{
		bIsGravityActive = false;
		//MaxSpeed = MaxSpeedGravityInactive;
	}
	if (!FMath::IsNearlyZero(VortexLeftInput) && !FMath::IsNearlyZero(VortexRightInput))
	{
		OwnAffectionFactor = DoubleGravityOwnAffectionFactor;
		//MaxSpeed = MaxSpeedGravityDoubleActive;
	}

	if (bOldIsGravityActive != bIsGravityActive)
	{
		if (bIsGravityActive)
		{
			OnGravityActivate();
		}
		else
		{
			OnGravityDeactivate();
		}
	}

	// Movement Related
	CollisionCapsule->AddForce(CalcFrictionForce(CollisionCapsule->GetPhysicsLinearVelocity(), GroundFriction,
	                                             DeltaTime));
	DecelToMaxVelocity(CollisionCapsule->GetPhysicsLinearVelocity(), MaxSpeed);

	FVector OldVelocity = CollisionCapsule->GetPhysicsLinearVelocity();


	FVector InputVector = GetPendingMovementInputVector().GetSafeNormal2D();

	FVector NewVelocity = OldVelocity + Accel * InputVector * DeltaTime;

	if (OldVelocity.Size() < MaxSpeed && NewVelocity.Size() > MaxSpeed)
	{
		CollisionCapsule->AddForce(Accel * InputVector);
	}
	else
	{
		CollisionCapsule->AddForce(Accel * InputVector);
	}

	bIsAttractingBall = false;

	if (Stunned)
	{
		HandleStun(DeltaTime);
	}
	else
	{
		if (bIsGravityActive)
		{
			GravityHold = FMath::Min(1.f, GravityHold + DeltaTime * GravityIncreaseSpeed);
			GravityRadius = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 1.f),
			                                                  FVector2D(MinGravityRadius, MaxGravityRadius),
			                                                  GravityHold);
			ApplySpinAndGravityToAttractableObjects(DeltaTime);
		}
	}

	if (Stunned || !bIsGravityActive)
	{
		GravityHold = FMath::Max(0.f, GravityHold - DeltaTime * GravityDecreaseSpeed);
		GravityRadius = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 1.f),
		                                                  FVector2D(MinGravityRadius, MaxGravityRadius), GravityHold);
	}

	HandleBoost(DeltaTime, InputVector);

	TiltGravitorToVelocityPrediction(DeltaTime, LastFrameVelocity, OldVelocity);

	LastFrameVelocity = OldVelocity;

	ClearInput();

	if (bDrawDebug)
	{
		DrawDebugThings(DeltaTime);
	}
}

void AGGravitor::ClearInput()
{
	CurrentSpinInput = FVector2D::ZeroVector;
	ConsumeMovementInputVector();
	VortexLeftInput = 0.f;
	VortexRightInput = 0.f;
}

void AGGravitor::HandleBoost(float DeltaTime, FVector InputVector)
{
	BoostReloadTimer -= DeltaTime;
	if (BoostReloadTimer < 0.f)
	{
		bCanBoost = true;
		BoostReloadTimer = BoostReloadTime;
	}

	if (BoostImpulsePressed && bCanBoost)
	{
		CollisionCapsule->AddImpulse(InputVector * BoostImpulsePower);
		BoostReloadTimer = BoostReloadTime;
		bCanBoost = false;
		Boost = 0.f;
	}

	BoostImpulsePressed = false;
}

void AGGravitor::ApplySpinAndGravityToAttractableObjects(float DeltaTime)
{
	bIsOwningABall = false;
	for (AActor* Actor : AttractableObjects)
	{
		FVector Pos = Actor->GetActorLocation();
		FVector DeltaPosition = Pos - CollisionCapsule->GetComponentLocation();
		float Distance = DeltaPosition.Size();
		BallDistance = Distance;
		if (Distance < GravityRadius)
		{
			bIsAttractingBall = true;

			float GravityInput = 0.f;
			bool bShouldGravitorBeAffected = true;
			bool bShouldBallBeAffected = true;
			const bool bLeftVortexPressed = !FMath::IsNearlyZero(VortexLeftInput);
			const bool bRightVortexPressed = !FMath::IsNearlyZero(VortexRightInput);

			if (bLeftVortexPressed && bRightVortexPressed)
			{
				GravityInput = FMath::Min(VortexLeftInput + VortexRightInput, DoubleGravityFactor);
			}
			else
			{
				GravityInput = FMath::Min(VortexLeftInput + VortexRightInput, 1.f);

				if (bLeftVortexPressed && !bRightVortexPressed)
				{
					bShouldBallBeAffected = false;
					bShouldGravitorBeAffected = true;
				}
				else if (!bLeftVortexPressed && bRightVortexPressed)
				{
					bShouldBallBeAffected = true;
					bShouldGravitorBeAffected = false;
				}
			}


			float VortexSpin = VortexRightInput - VortexLeftInput;
			// -1 is full left spin, 1 is full right spin, 0 is no spin
			VortexSpin = VortexSpin > 0.f ? 1.f : VortexSpin;
			VortexSpin = VortexSpin < 0.f ? -1.f : VortexSpin;

			const float FinalPower = GravityInput * GravityPower;
			//CalcPowerDecayByDistance(Distance, GravityPower, GravityDistanceDecayFactor) * GravityInput;

			FVector Force = -DeltaPosition.GetSafeNormal2D() * FinalPower;

			/* Handle Item */
			AGItem* Item = Cast<AGItem>(Actor);
			if (Item)
			{
				Item->CollisionCapsule->AddForce(Force);
				Item->TillActivationTimer = Item->TillActivationTime;
			}

			/* Handle Ball */
			AGBall* Ball = Cast<AGBall>(Actor);
			if (Ball)
			{
				if (Ball->PossessingTeam != Team)
					continue;

				bIsOwningABall = true;

				//DrawDebugLine(GetWorld(), BallPos, BallPos+RightVortexSpinVec*VortexImpulsePower, FColor::Red, true, DeltaTime, SDPG_Foreground, 4.f);


				if (bShouldBallBeAffected)
				{
					Ball->AddGravityForce(Force, Distance);
				}
				// if(bShouldGravitorBeAffected)
				// {
				// 	if(!bShouldBallBeAffected)
				// 	{
				// 		Force *= 2.f;
				// 	}
				// 	
				CollisionCapsule->AddForce(-OwnAffectionFactor * Force);
				// }

				if (!FMath::IsNearlyZero(CurrentSpinInput.SizeSquared()))
				{
					CurrentSpinInput.Normalize();
					Ball->PendingSpinForces.Add(FVector(CurrentSpinInput, 0.f) * SpinAccelPower * DeltaTime);
				}
			}
		}
	}
}

// Called to bind functionality to input
void AGGravitor::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

FVector AGGravitor::CalcFrictionForce(FVector CurrentVelocity, float Friction, float DeltaTime)
{
	FVector OldVelocity = CurrentVelocity;
	FVector FrictionForce = (Friction * CurrentVelocity);
	CurrentVelocity -= FrictionForce * DeltaTime;

	if ((CurrentVelocity | OldVelocity) < 0.f)
	{
		FrictionForce = FVector::ZeroVector;
	}

	return -FrictionForce;
}

void AGGravitor::DecelToMaxVelocity(FVector CurrentVelocity, float CurMaxSpeed)
{
	if (CurrentVelocity.SizeSquared2D() > CurMaxSpeed * CurMaxSpeed)
	{
		FVector FrictionForce = -(CurrentVelocity * DecelToMaxSpeedFriction);
		CollisionCapsule->AddForce(FrictionForce);
	}
}

void AGGravitor::AddSpinInput(FVector2D Input)
{
	CurrentSpinInput += Input;
}

void AGGravitor::TiltGravitorToVelocityPrediction(float DeltaTime, FVector OldVelocity, FVector NewVelocity)
{
	// *** Prediction Attempt begin
	//float RadialAngleDelta = OldVelocity.CosineAngle2D(NewVelocity);
	//RadialDeltaHistory.Add(RadialAngleDelta);
	// const int32 HistoryLength = RadialDeltaHistory.Num();
	//
	// float Sum = 0.f;
	// for (float RadialDelta : RadialDeltaHistory)
	// {
	//     Sum += RadialDelta;
	// }
	//
	// float RadialAverage = UKismetMathLibrary::SafeDivide(Sum, HistoryLength);
	//
	// FVector FinalVel = NewVelocity;
	// for (int i = 0; i < TiltPredictionLength; ++i)
	// {
	//     FinalVel = FinalVel.RotateAngleAxis(RadialAverage, FVector::UpVector);
	// }
	//FRotator Rot = FRotator(-FinalVel.X, 0.f, FinalVel.Y) * VelocityToTiltFactor;
	//FQuat TiltRot(FVector(-TiltTargetVelocity.X, TiltTargetVelocity.Y,0.f) ,*DeltaTime);
	// *** Prediciton Attempt end 

	FVector BallRelativeLoc = BallMesh->GetComponentLocation() - CollisionCapsule->GetComponentLocation();
	FVector Rotated = FVector(BallRelativeLoc.Y, -BallRelativeLoc.X, 0.f);
	FQuat TiltRot(
		Rotated, FMath::DegreesToRadians(FMath::Clamp(Rotated.Size() * VelocityToTiltFactor, 0.f, MaxTiltAngle)));

	TargetTiltRotation = FMath::QInterpTo(TargetTiltRotation, TiltRot, DeltaTime, TiltLerpSpeed);
	FQuat YawAdjustment(FVector::UpVector, FMath::DegreesToRadians(CollisionCapsule->GetComponentRotation().Yaw));
	TiltRot = TargetTiltRotation * YawAdjustment;

	RingMesh->SetWorldRotation(TiltRot);
}

void AGGravitor::CollectItem(AGItem* Item)
{
	if (Item)
	{
		CollectedItems.Add(Item);
	}
}

void AGGravitor::UseNextCollectedItem()
{
	if (CollectedItems.Num() > 0)
	{
		AGItem* Item = CollectedItems.Pop(true);
		if (Item)
		{
			Item->Use(ItemDetachOffset);
		}
	}
}

void AGGravitor::OnGravitorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogGravitor, Verbose, TEXT("NormalImpulse Magnitude: %f"), NormalImpulse.Size())
	StunTime = FMath::GetMappedRangeValueClamped(FVector2D(MinImpulse, MaxImpulse), FVector2D(MinStunTime, MaxStunTime),
	                                             NormalImpulse.Size());


	AGBall* Ball = Cast<AGBall>(OtherActor);
	if (Ball && Ball->bIsDeadly)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			AGGravityDestroyGameMode* GM = Cast<AGGravityDestroyGameMode>(GetWorld()->GetAuthGameMode());
			if (GM)
			{
				GM->RegisterDestroy(Team, Ball);
			}
		}
	}

	if (StunTime > StunTimer)
	{
		StunTimer = StunTime;
		Stunned = true;
	}
}

void AGGravitor::HandleStun(float DeltaTime)
{
	StunTimer -= DeltaTime;
	if (StunTimer < 0.f)
	{
		Stunned = false;
	}
}
