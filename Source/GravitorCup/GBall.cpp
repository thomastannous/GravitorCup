// Fill out your copyright notice in the Description page of Project Settings.


#include "GBall.h"
#include "Components/SphereComponent.h"
#include "GameFramework/MovementComponent.h"
#include "DrawDebugHelpers.h"
#include "GGravityDestroyGameMode.h"
#include "GravitorCup.h"

;
// Sets default values
AGBall::AGBall()
{
    PrimaryActorTick.bCanEverTick = true;

    /* Components */

    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    RootComponent = CollisionSphere;

    BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));
    BallMesh->SetupAttachment(CollisionSphere);

    YawOrientator = CreateDefaultSubobject<USceneComponent>(TEXT("YawOrientator"));
    YawOrientator->SetupAttachment(BallMesh);

    InnerRing = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InnerRing"));

    OuterRing = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OuterRing"));

    InnerRing->SetupAttachment(YawOrientator);
    OuterRing->SetupAttachment(BallMesh);

    bDrawDebug = false;

    /* Movement */

    MaxSpeed = 2000.f;
    FrictionGround = 2000.f;
    FrictionLift = 200.f;
    Friction = FrictionGround;



    /* Gravity */
    MaxPerFrameGravity = 30000.f;

    /* Lift */

    ForceToLiftFactor = 0.002;
    MinLift = 0.f;
    Lift = MinLift;
    MaxLift = 1.f;
    LiftThreshold = 0.5f;
    ReduceLiftSpeed = 0.5f;
    Lifted = false;

    /* SingleTestMode */    
    bIsSingleTestMode = false;
    BallReturnForceFactor = 8.f;

    /* Spin */

    SpinVelocity = FVector::ZeroVector;
    SpinForce = 4000.f;
    SpinFrictionLifted = 0.f;
    SpinFrictionGround = 2.26f;
    SpinFriction = SpinFrictionGround;
    MaxSpinSpeed = 100.f;
    RingSpinSpeed = 23.f;

    PossessingTeam = -1;
    OldPossessingTeam = -1;
    ExpirationTime = 0.f;
    Expiration = ExpirationTime;
}

// Called when the game starts or when spawned
void AGBall::BeginPlay()
{
    Super::BeginPlay();

    // In case we didn't set them equal in a child blueprint 
    Expiration = 0.f;
}

void AGBall::AddGravityForce(FVector Force, float Distance)
{
    PendingGravityForces.Add(FGravityForceParams(Force, Distance));
}

void AGBall::ApplyBallReturnForce()
{
    float BallY = GetActorLocation().Y;
    if(BallY > 0.f)
    {
        PendingGravityForces.Add(FGravityForceParams(FVector(0.f,-BallY * BallReturnForceFactor, 0.f), BallY));
    }
}

// Called every frame
void AGBall::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bMovementDisabled)
    {
        CollisionSphere->SetPhysicsLinearVelocity(FVector::ZeroVector);
        PendingGravityForces.Empty();
        PendingSpinForces.Empty();
        return;
    }


    if (FMath::Abs(Expiration) > ExpirationTime)
    {
        UWorld* World = GetWorld();
        if (World)
        {
            AGGravityDestroyGameMode* GM = Cast<AGGravityDestroyGameMode>(GetWorld()->GetAuthGameMode());
            if (GM)
            {
                GM->RegisterBallExpired(this);
                OnBallExpired();
                Expiration = 0.f;
            }
        }
    }

    // Don't store if ball is in neutral area (-1) to detect from which to which side the ball transitioned
    if(PossessingTeam != -1)
    {
        OldPossessingTeam = PossessingTeam;
    }

    float WhichTeamSide = 0.f;
    if (FMath::Abs(GetActorLocation().Y) > 15.0)
    {
        WhichTeamSide = FMath::Sign(GetActorLocation().Y);
        Expiration += DeltaTime * WhichTeamSide;
    }
    
    if(FMath::IsNearlyZero(WhichTeamSide))
    {
        PossessingTeam = -1;        
    }
    else
    {
        PossessingTeam = WhichTeamSide < 0.f ? 0 : 1;
    }

    if(bIsSingleTestMode)
    {
        ApplyBallReturnForce();
    }

    if (OldPossessingTeam != PossessingTeam && OldPossessingTeam != -1)
    {
        OnBallChangePossession(PossessingTeam);
        //Expiration = ExpirationTime;
    }


    ConsumeGravityForces(DeltaTime);

    bool OldLifted = Lifted;
    Lifted = Lift > LiftThreshold;

    if (OldLifted != Lifted)
    {
        int a = 0;
        if (Lifted)
        {
            OnLifted();
        }
        else
        {
            OnGrounded();
        }
    }

    ConsumeSpinForces(DeltaTime);

    if (SpinVelocity.Size2D() > 0.f)
    {
        if (!Lifted)
        {
            AccelerateBySpin(DeltaTime);
        }
        ApplySpinFriction(DeltaTime);
    }

    FVector FrameVelocity = CollisionSphere->GetPhysicsLinearVelocity();
    bIsDeadly = FrameVelocity.Size() > DeadlyVelocityMagnitudeThreshold;
	LastFrameVelocity =  FrameVelocity;

    CollisionSphere->AddForce(CalcFrictionForce(FrameVelocity, DeltaTime));
    DecelToMaxVelocity(FrameVelocity, MaxSpeed);
    CollisionSphere->AddForce(CalcFrictionForce(CollisionSphere->GetPhysicsLinearVelocity(), DeltaTime));

    DecreaseLift(DeltaTime);

    if (bDrawDebug)
    {
        DrawDebug(DeltaTime);
    }

    SpinRings(DeltaTime);

    PendingGravityForces.Empty();
    PendingSpinForces.Empty();
}

void AGBall::ConsumeGravityForces(float DeltaTime)
{
    FVector GravitySum(0.f);
    float DistanceSum = 0.f;
    for (const FGravityForceParams GravityForceParam : PendingGravityForces)
    {
        DistanceSum += GravityForceParam.Distance;
    }
    for (const FGravityForceParams GravityForceParam : PendingGravityForces)
    {
        GravitySum += GravityForceParam.GravityForce * (GravityForceParam.Distance / DistanceSum);
    }

    //GravitySum = GravitySum.GetSafeNormal2D()*FMath::Min(GravitySum.Size(), MaxPerFrameGravity);
    float GravityLength = GravitySum.Size();
    Lift += GravityLength * DeltaTime * ForceToLiftFactor;
    if (Lift > MaxLift)
    {
        Lift = MaxLift;
    }
    CollisionSphere->AddForce(GravitySum);
}

void AGBall::DecelToMaxVelocity(FVector CurrentVelocity, float CurMaxSpeed)
{
    if (CurrentVelocity.SizeSquared2D() > CurMaxSpeed * CurMaxSpeed)
    {
        FVector FrictionForce = -(CurrentVelocity * DecelToMaxSpeedFriction);
        CollisionSphere->AddForce(FrictionForce);
    }
}

void AGBall::ConsumeSpinForces(float DeltaTime)
{
    bIsLoadingSpin = false;

    if (Lifted)
    {
        for (FVector PendingSpinForce : PendingSpinForces)
        {
            bIsLoadingSpin = true;
            SpinVelocity += PendingSpinForce * DeltaTime;
        }
    }
    SpinVelocity = SpinVelocity.GetClampedToMaxSize(MaxSpinSpeed);
}

void AGBall::DecreaseLift(float DeltaTime)
{
    Lift -= DeltaTime * ReduceLiftSpeed;

    if (Lift < 0.f)
    {
        Lift = 0.f;
    }
}


void AGBall::AccelerateBySpin(float DeltaTime)
{
    float SpinSpeed = SpinVelocity.Size();
    if (SpinSpeed > 2.0f)
    {
        float FinalSpinPower = SpinForce;

        // If in future change in direction of velocity is desired rather than just applying the spin velocity to velocity, then use this below
        // 
        //* FMath::Abs(SpinVelocity.GetSafeNormal() | CollisionSphere->GetPhysicsLinearVelocity().RotateAngleAxis(90.f, FVector::UpVector)); //+ 1000.f * (FMath::Log2(SpinSpeed-1.f));
        //UE_LOG(LogTemp, Warning, TEXT("%f"), FinalSpinPower);
        //FVector Vel = CollisionSphere->GetPhysicsLinearVelocity();
        //FVector VelRightOrtho = Vel.RotateAngleAxis(90.f, FVector::UpVector);
        //FVector SpinOnVeloRight = SpinVelocity.ProjectOnTo(VelRightOrtho);
        //FVector SpinOnVelo = SpinVelocity; // --.ProjectOnTo(Vel);

        CollisionSphere->AddForce(SpinVelocity.GetSafeNormal2D() * FinalSpinPower);
    }
}

void AGBall::ApplySpinFriction(float DeltaTime)
{
    SpinFriction = Lifted ? SpinFrictionLifted : SpinFrictionGround;

    if (bIsLoadingSpin)
        return;

    FVector OldSpinVelocity = SpinVelocity;
    SpinVelocity -= (SpinVelocity * SpinFriction) * DeltaTime;

    if ((OldSpinVelocity | SpinVelocity) < 0.f)
    {
        SpinVelocity = FVector::ZeroVector;
    }
}

void AGBall::SpinRings(float DeltaTime)
{
    if (FMath::IsNearlyZero(SpinVelocity.SizeSquared()))
    {
        ResetRings();
    }
    else
    {
        float FinalYaw = SpinVelocity.ToOrientationRotator().Yaw;
        YawOrientator->SetRelativeRotation(FQuat(FRotator(0.f, FinalYaw, 0.f)));

        FRotator DeltaRot = FRotator(DeltaTime * SpinVelocity.Size() * -RingSpinSpeed, 0.f, 0.f);
        InnerRing->AddLocalRotation(DeltaRot);
    }
}

void AGBall::ResetRings()
{
    InnerRing->SetRelativeRotation(FRotator::ZeroRotator);
    OuterRing->SetRelativeRotation(FRotator::ZeroRotator);
}

void AGBall::OnBallExpired_Implementation()
{
}

void AGBall::DrawDebug(float DeltaTime)
{
}

void AGBall::Setup()
{
}

void AGBall::ResetBall()
{
    CollisionSphere->SetWorldLocationAndRotation(FVector(0.f, 0.f,PLAYER_STANDARD_HEIGHT), FRotator::ZeroRotator);
    CollisionSphere->SetPhysicsLinearVelocity(FVector::ZeroVector);
    SpinVelocity = FVector::ZeroVector;
    Lift = 0.f;
    PossessingTeam = -1;
    Expiration = 0.f;
    ResetRings();
    OnResetBall();
}

void AGBall::StopBall()
{
    CollisionSphere->SetPhysicsLinearVelocity(FVector::ZeroVector);
}

FVector AGBall::CalcFrictionForce(FVector CurrentVelocity, float DeltaTime)
{
    Friction = Lifted ? FrictionLift : FrictionGround;
    FVector OldVelocity = CurrentVelocity;
    FVector FrictionForce = (Friction * CurrentVelocity);
    CurrentVelocity -= FrictionForce * DeltaTime;

    if ((CurrentVelocity | OldVelocity) < 0.f)
    {
        FrictionForce = FVector::ZeroVector;
    }

    return -FrictionForce;
}
