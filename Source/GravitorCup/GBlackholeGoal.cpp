// Fill out your copyright notice in the Description page of Project Settings.


#include "GBlackholeGoal.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GBall.h"
#include "GGravityBallGameMode.h"

// Sets default values
AGBlackholeGoal::AGBlackholeGoal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GravityRadius = 1000.f;
	GravityPower = 12000.f;

	GoalArea = CreateDefaultSubobject<USphereComponent>(TEXT("GoalArea"));
	RootComponent = GoalArea;
	GoalArea->InitSphereRadius(64.f);
}

// Called when the game starts or when spawned
void AGBlackholeGoal::BeginPlay()
{
	Super::BeginPlay();

	if(GoalArea)
	{
		GoalArea->OnComponentBeginOverlap.AddDynamic(this, &AGBlackholeGoal::OnGoalAreaOverlap);
	}
}

void AGBlackholeGoal::ApplyGravityToAttractableObjects()
{
	for(AGBall *Ball : AttractableObjects)
	{
		FVector BallLoc = Ball->GetActorLocation();
		FVector BallToBlackhole = GetActorLocation() - BallLoc;
		const float Distance = BallToBlackhole.Size();
		if (Distance < GravityRadius)
		{
			Ball->AddGravityForce(BallToBlackhole.GetSafeNormal2D() * GravityPower, Distance);
		}
	}
}

// Called every frame
void AGBlackholeGoal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ApplyGravityToAttractableObjects();
}

void AGBlackholeGoal::OnGoalAreaOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AGBall* Ball = Cast<AGBall>(OtherActor);
	if(Ball)
	{
		UWorld *World = GetWorld();
		if (World)
		{
			AGGravityBallGameMode *GM =  Cast<AGGravityBallGameMode>(GetWorld()->GetAuthGameMode());
			if(GM)
			{
				GM->RegisterGoal(DefensingTeam, Ball);
			}
		}
	}
}
