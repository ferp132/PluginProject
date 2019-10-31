// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingPlatform.generated.h"

class USplineComponent;

UCLASS()
class PLUGINPROJECT_API AMovingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMovingPlatform();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		int CurrentPoint = 0;

	UPROPERTY(VisibleAnywhere, Category = "Waypoints")
		TArray<UStaticMeshComponent*> WaypointMeshArray;

public:	

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* PlatformMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		USplineComponent* Spline = nullptr;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoints")
		UStaticMesh* CustomWaypointMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoints")
		UMaterial* CustomWaypointMaterial = nullptr;

};
