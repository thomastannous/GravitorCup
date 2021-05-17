// Fill out your copyright notice in the Description page of Project Settings.


#include "GItem.h"
#include "GItemSpawner.h"
#include "GGravitor.h"
#include "GravitorCup.h"

// Sets default values
AGItem::AGItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionCapsule"));
	RootComponent = CollisionCapsule;

	CollisionCapsule->OnComponentHit.AddDynamic(this, &AGItem::OnItemHit);
	CollisionCapsule->OnComponentBeginOverlap.AddDynamic(this, &AGItem::OnItemOverlap);
	

	TillActivationTime = 1.f;
	TillActivationTimer = TillActivationTime;

	bIsInstant = true;

	State = ItemState::IS_Idle;
}

// Called when the game starts or when spawned
void AGItem::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(State == ItemState::IS_Armed)
	{
		TillActivationTimer -= DeltaTime;
		if(TillActivationTimer < 0.f)
		{
			ActivateItem();
		}
	}

	if(State == ItemState::IS_Activated)
	{
		EffectDurationTimer -= DeltaTime;
		if(EffectDurationTimer < 0.f)
		{
			DeactivateItem();
		}
	}
	
	if(State == ItemState::IS_PendingDestroy)
	{
		AfterEffectTime -= DeltaTime;
		if(AfterEffectTime < 0.f)
		{
			Destroy();
		}
	}

}

void AGItem::OnItemHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	
	if(State == ItemState::IS_Armed && bCanBeActivatedByCollision)
	{
		ActivateItem();		
	}
	
	if(State == ItemState::IS_Idle && bIsCollectable)
	{
		AGGravitor *Gravitor = Cast<AGGravitor>(OtherActor);
		if(Gravitor)
		{
			State = ItemState::IS_Collected;	
			Collected(Gravitor);
		}
	}
	else
	{
		if(State == ItemState::IS_Idle)
		{
			Arm();
		}
	}
	
}

void AGItem::OnItemOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(State == ItemState::IS_Idle && bIsCollectable)
	{
		AGGravitor *Gravitor = Cast<AGGravitor>(OtherActor);
		if(Gravitor)
		{
			UE_LOG(LogG, Verbose, TEXT("Gravitor %s Overlapped with Item %s"), *Gravitor->GetName(), *GetNameSafe(this))
			State = ItemState::IS_Collected;	
			Collected(Gravitor);
		}
	}
}

void AGItem::Arm()
{
	State = ItemState::IS_Armed;	
	TillActivationTimer = TillActivationTime;
}

void AGItem::Use(FVector DetachOffset)
{
	const FDetachmentTransformRules DetachmentTransformRules(FDetachmentTransformRules::KeepWorldTransform);
	DetachFromActor(DetachmentTransformRules);
	
	SetActorLocation(GetActorLocation()+DetachOffset, false, nullptr, ETeleportType::ResetPhysics);
	if(GravitorOwner)
	{
		GravitorOwner->AttractableObjects.Add(this);
	}
	
	CollisionCapsule->SetSimulatePhysics(true);
	CollisionCapsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Block);
	
	Arm();
}

void AGItem::Collected(AGGravitor* GOwner)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::KeepRelative, false);
	TransformRules.LocationRule = EAttachmentRule::SnapToTarget;
	TransformRules.RotationRule = EAttachmentRule::KeepRelative;
	TransformRules.ScaleRule = EAttachmentRule::KeepRelative;
	AttachToActor(GOwner, TransformRules);
	
	GOwner->CollectItem(this);
	GravitorOwner = GOwner;
}

void AGItem::ActivateItem()
{
	if(bIsInstant)
	{
		AfterEffectTimer = AfterEffectTime;
		DeactivateItem();
	}
	else
	{
		State = ItemState::IS_Activated;
		EffectDurationTimer = EffectDuration;
	}
	BP_ActivateItem();
}

void AGItem::DeactivateItem()
{
	State = ItemState::IS_PendingDestroy;
	GravitorOwner->AttractableObjects.Remove(this);
	CollisionCapsule->SetSimulatePhysics(false);
	CollisionCapsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
	ItemSpawner->MarkForRespawn();
	BP_DeactivateItem();
}

