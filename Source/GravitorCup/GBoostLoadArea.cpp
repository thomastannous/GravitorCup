// Fill out your copyright notice in the Description page of Project Settings.


#include "GBoostLoadArea.h"
#include "Components/SphereComponent.h"
#include "GGravitor.h"

// Sets default values
AGBoostLoadArea::AGBoostLoadArea()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Area = CreateDefaultSubobject<USphereComponent>(TEXT("Area"));
	Area->InitSphereRadius(500.f);

	Area->OnComponentBeginOverlap.AddDynamic(this, &AGBoostLoadArea::OnAreaBeginOverlap);
	Area->OnComponentEndOverlap.AddDynamic(this, &AGBoostLoadArea::OnAreaEndOverlap);
}

// Called when the game starts or when spawned
void AGBoostLoadArea::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGBoostLoadArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AGBoostLoadArea::OnAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AGGravitor *Gravitor = Cast<AGGravitor>(OtherActor);
	if(Gravitor)
	{
		Gravitor->bInBoostLoadArea = true;		
	}
}

void AGBoostLoadArea::OnAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AGGravitor *Gravitor = Cast<AGGravitor>(OtherActor);
	if(Gravitor)
	{
		Gravitor->bInBoostLoadArea = false;		
	}
}