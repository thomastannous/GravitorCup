// Fill out your copyright notice in the Description page of Project Settings.


#include "GArenaCamera.h"
#include "GravitorCup.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AGArenaCamera::AGArenaCamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bIsEnabled = false;

	// Setup camera defaults
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	RootComponent = CameraComponent;
	CameraComponent->FieldOfView = 60.0f;
	CameraComponent->bConstrainAspectRatio = true;
	CameraComponent->AspectRatio = 1.777778f;
	CameraComponent->PostProcessBlendWeight = 1.0f;
	CameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	PositionLimitTop = 3000.0;
	PositionLimitBottom = -PositionLimitTop;
	PositionLimitRight = 3000.0;
	PositionLimitLeft = -PositionLimitRight;
	
	MinDistance = 200.0;
	MaxDistance = 8000.0;

	MinCamZ = 4500.0;
	MaxCamZ = 9000.0;
	CurrentCamZ = MinCamZ;
	MinCamY = -4800;
	MaxCamY = -5500;
	CurrentCamY = MinCamY;
	
	SmoothSpeed = 3.0f;
	
	AspectRatio = CameraComponent->AspectRatio;
	
}

// Called when the game starts or when spawned
void AGArenaCamera::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGArenaCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(bIsEnabled)
	{
		AdjustToObjects(DeltaTime);
	}
}

void AGArenaCamera::SetTrackedObjects(TArray<AActor*> NewTrackedObjects)
{
	if(NewTrackedObjects.Num() > 0)
	{
		TrackedObjects = NewTrackedObjects;
	}
}

void AGArenaCamera::AddTrackedObject(AActor* TrackedObject)
{
	TrackedObjects.Add(TrackedObject);
}

void AGArenaCamera::AdjustToObjects(float DeltaTime)
{
	const int32 TrackedObjectsSize = TrackedObjects.Num(); 	
	if(TrackedObjectsSize == 0)
	{
		return;
	}
	FVector AveragePosition = FVector::ZeroVector;
	for(AActor* Actor : TrackedObjects)
	{
		AveragePosition += Actor->GetActorLocation();	
	}
	AveragePosition.Z = PLAYER_STANDARD_HEIGHT;
	AveragePosition *= 1.0/TrackedObjectsSize;

	float LongestDistance = GetLongestDistanceBetweenAllTracked();
	float DesiredZ = FMath::GetMappedRangeValueClamped(FVector2D(MinDistance,MaxDistance), FVector2D(MinCamZ, MaxCamZ), LongestDistance);
	float DesiredY = FMath::GetMappedRangeValueClamped(FVector2D(MinDistance,MaxDistance), FVector2D(MinCamY, MaxCamY), LongestDistance);

	// TODO: Maybe interpolate this in future
	CurrentCamZ = FMath::Lerp(CurrentCamZ, DesiredZ, DeltaTime*SmoothSpeed);
	CurrentCamY = FMath::Lerp(CurrentCamY, DesiredY, DeltaTime*SmoothSpeed);
	
	SetActorLocation(AveragePosition+FVector(CurrentCamY, 0.0,CurrentCamZ));
	const FRotator Rot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), AveragePosition);
	SetActorRotation(Rot);
	
	UE_LOG(LogGCamera, VeryVerbose,TEXT("LongestDistance: %f"), LongestDistance);
	UE_LOG(LogGCamera, VeryVerbose,TEXT("AveragePosition: %f, %f, %f"), AveragePosition.X, AveragePosition.Y, AveragePosition.Z );
}

float AGArenaCamera::GetLongestDistanceBetweenAllTracked()
{
	float CurrentMaxDistance = -1.f;
	for(AActor* Actor1 : TrackedObjects)
	{
		for(AActor* Actor2: TrackedObjects)
		{
			if(Actor1 != Actor2)
			{
				const FVector Distance = Actor1->GetActorLocation() - Actor2->GetActorLocation();
				// Check for each dimension, cause 
				CurrentMaxDistance = FMath::Max(CurrentMaxDistance, FMath::Max(Distance.X*AspectRatio * 1.2f, Distance.Y));
			}
		}
	}
	if (!FMath::IsNearlyEqual(CurrentMaxDistance,1.f))
	{
		return CurrentMaxDistance;
	}
	UE_LOG(LogGCamera, Warning, TEXT("GCamera: LongestDistance is -1.f, probably because no players are tracked"));
	return 0.0;
}


