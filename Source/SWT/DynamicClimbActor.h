// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "DynamicClimbActor.generated.h"

UCLASS()
class SWT_API ADynamicClimbActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADynamicClimbActor();

	USplineComponent* Spline;

};
