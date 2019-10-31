// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"
#include "Components/SplineComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AMovingPlatform::AMovingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Platform Mesh"));
	SetRootComponent(PlatformMesh);

	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	Spline->SetupAttachment(PlatformMesh);

	if (ensure(Spline))
	{
		for (int i = 0; i < Spline->GetNumberOfSplinePoints(); i++)
		{
			//Spawn a new one with a name based on the current waypoint in this loop
			UStaticMeshComponent* NewMesh = CreateDefaultSubobject<UStaticMeshComponent>(*FString("Waypoint Mesh " + FString::FromInt(i)));

			NewMesh->SetWorldTransform(Spline->GetTransformAtSplinePoint(i, ESplineCoordinateSpace::World));
			NewMesh->SetupAttachment(PlatformMesh);

			//Add it to the array of waypoint meshes
			WaypointMeshArray.Add(NewMesh);
		}
	}
}

// Called when the game starts or when spawned
void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMovingPlatform::OnConstruction(const FTransform& Transform)
{
	if (ensure(Spline))
	{
		for (int i = 0; i < Spline->GetNumberOfSplinePoints(); i++)
		{
				//If there isnt a mesh for this spline point
				if (WaypointMeshArray.Num() == i - 1)
				{

				}
		}
	}


	//Loop through Each Waypoint And chech that it's stacmesh is equal to the main meshes
	if (ensure(PlatformMesh))
	{
		for (int i = 0; i < WaypointMeshArray.Num(); i++)
		{
			if (CustomWaypointMesh != nullptr)
			{
				if (WaypointMeshArray[i]->GetStaticMesh() != CustomWaypointMesh)
					WaypointMeshArray[i]->SetStaticMesh(CustomWaypointMesh);
			}
			else if (WaypointMeshArray[i]->GetStaticMesh() != PlatformMesh->GetStaticMesh())
				WaypointMeshArray[i]->SetStaticMesh(PlatformMesh->GetStaticMesh());

			if (WaypointMeshArray[i]->GetComponentLocation() != Spline->GetTransformAtSplinePoint(i, ESplineCoordinateSpace::World).GetLocation())
			{
				WaypointMeshArray[i]->SetWorldLocation(Spline->GetTransformAtSplinePoint(i, ESplineCoordinateSpace::World).GetLocation());
			}
		}
	}
}

