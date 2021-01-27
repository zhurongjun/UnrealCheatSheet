// Fill out your copyright notice in the Description page of Project Settings.


#include "CrashAngel.h"


// Sets default values
ACrashAngel::ACrashAngel()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACrashAngel::BeginPlay()
{
	Super::BeginPlay();

	AActor* CrashHelper = nullptr;
	CrashHelper->ReceiveDestroyed();
}

// Called every frame
void ACrashAngel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

