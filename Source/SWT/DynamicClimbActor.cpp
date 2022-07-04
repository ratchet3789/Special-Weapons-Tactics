// Fill out your copyright notice in the Description page of Project Settings.


#include "DynamicClimbActor.h"

// Sets default values
ADynamicClimbActor::ADynamicClimbActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	Spline->SetupAttachment(RootComponent);
}

