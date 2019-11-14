// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EngineMinimal.h"
#include "Editor/Blutility/Classes/EditorUtilityWidget.h"
#include "PlatformSystem.generated.h"
class USplineComponent;

UCLASS()
class MOVINGPLATFORM_API APlatformSystem : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	APlatformSystem();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(BlueprintCallable)
		static void StartWidget(UWidgetBlueprint* Blueprint);

	TSharedRef<SDockTab> SpawnEditorUITab(const FSpawnTabArgs& SpawnTabArgst, UWidgetBlueprint* Blueprint);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		int CurrentPoint = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		int StartPoint = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		float DistanceTravelled = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		float PlatformSpeed = 1000.f;


	UPROPERTY(VisibleAnywhere, Category = "Waypoints")
		TArray<UStaticMeshComponent*> WaypointMeshArray;

	void CheckWaypointMesh(int ArrayIndex);
	void CheckWaypointLocation(int ArrayIndex);
	void CheckWaypointRotation(int ArrayIndex);
	void CheckWaypointTransform(int ArrayIndex);


	void UpdateWaypointArray();
	void CreateNewWaypoint();
	void DestroyWaypoint(int ArrayIndex);

	void UpdatePlatformTransform();
	void UpdatePlatformLocation();
	void UpdatePlatformRotation();

	void ValidateStartPoint();
	void UpdatePlatformStartPosition();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoints")
		bool bUseWaypointLocation = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoints")
		bool bUseWaypointRotation = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* PlatformMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		USplineComponent* Spline = nullptr;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoints")
		UStaticMesh* CustomWaypointMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoints")
		UMaterial* CustomWaypointMaterial = nullptr;

};
