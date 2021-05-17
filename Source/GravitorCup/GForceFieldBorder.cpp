// Fill out your copyright notice in the Description page of Project Settings.


#include "GForceFieldBorder.h"
#include "Components/SplineComponent.h"
#include "GBall.h"
#include "GravitorCup.h"


// Sets default values
AGForceFieldBorder::AGForceFieldBorder()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BorderSpline = CreateDefaultSubobject<USplineComponent>(TEXT("BorderSpline"));

	RootComponent = BorderSpline;

	ForcePower = 1000.f;
	OutWorldDirection = FVector(1.f,0.f,0.f);
}

// Called when the game starts or when spawned
void AGForceFieldBorder::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGForceFieldBorder::ApplyForceToInnerObjects(float DeltaTime)
{
	for(AActor* Obj : ForceObjects)
	{
		UPrimitiveComponent *PhysComp = Cast<UPrimitiveComponent>(Obj->GetRootComponent()); 
		if(PhysComp)
		{
			FVector ObjLoc = Obj->GetActorLocation();
			float InKey = BorderSpline->FindInputKeyClosestToWorldLocation(ObjLoc);
			FVector SplineLoc = BorderSpline->GetLocationAtSplineInputKey(InKey, ESplineCoordinateSpace::World);
			FVector SplineTangent = BorderSpline->GetTangentAtSplineInputKey(InKey, ESplineCoordinateSpace::World);
			const FVector OutNormal = SplineTangent.RotateAngleAxis(-90.f, FVector(0.f,0.f,1.f));
			
			//DrawEasyLine(GetWorld(), SplineLoc, SplineLoc+OutNormal, DeltaTime);
			FVector ObjToSpline = SplineLoc - ObjLoc;
			 if ((ObjToSpline | OutNormal) < 0.f) //&& (ObjToSpline | OutWorldDirection) < 0.f)
			 {
				PhysComp->AddForce(ObjToSpline * ForcePower);
			 }
		}
	}
}

// Called every frame
void AGForceFieldBorder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ApplyForceToInnerObjects(DeltaTime);
}

