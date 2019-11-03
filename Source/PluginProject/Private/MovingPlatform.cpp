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
}

// Called when the game starts or when spawned
void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMovingPlatform::CheckWaypointMesh(int ArrayIndex)
{
	if (!ensure(PlatformMesh)) return;

	UStaticMesh* WaypointMesh = WaypointMeshArray[ArrayIndex]->GetStaticMesh();
	UMaterial* WaypointMaterial = WaypointMeshArray[ArrayIndex]->GetMaterial(0)->GetMaterial();


	UMaterial* MainMaterial = PlatformMesh->GetMaterial(0)->GetMaterial();
	UStaticMesh* MainMesh = PlatformMesh->GetStaticMesh();

	//If the user has set a custom mesh for all the waypoints
	if (CustomWaypointMesh != nullptr)
	{
		if (WaypointMesh != CustomWaypointMesh)
			WaypointMeshArray[ArrayIndex]->SetStaticMesh(CustomWaypointMesh);
	}
	else if (WaypointMesh != MainMesh)
		WaypointMeshArray[ArrayIndex]->SetStaticMesh(MainMesh);

	if (ensure(CustomWaypointMaterial))
	{
		if (WaypointMaterial != CustomWaypointMaterial)
		{
			WaypointMeshArray[ArrayIndex]->SetMaterial(0, CustomWaypointMaterial);
		}
	}
	else
	{
		if (WaypointMaterial != MainMaterial)
		{
			WaypointMeshArray[ArrayIndex]->SetMaterial(0, MainMaterial);
		}
	}

}

void AMovingPlatform::CheckWaypointLocation(int ArrayIndex)
{
	FVector SplinepointLocation = Spline->GetLocationAtSplinePoint(ArrayIndex, ESplineCoordinateSpace::World);
	FVector MeshLocation = WaypointMeshArray[ArrayIndex]->GetComponentLocation();

	if (MeshLocation != SplinepointLocation)
	{
		WaypointMeshArray[ArrayIndex]->SetWorldLocation(SplinepointLocation);
	}
}

void AMovingPlatform::CheckWaypointRotation(int ArrayIndex)
{
	FRotator SplinepointRotation = Spline->GetRotationAtSplinePoint(ArrayIndex, ESplineCoordinateSpace::World);
	FRotator MeshRotation = WaypointMeshArray[ArrayIndex]->GetComponentRotation();

	if (MeshRotation != SplinepointRotation)
	{
		WaypointMeshArray[ArrayIndex]->SetWorldRotation(SplinepointRotation);
	}
}

void AMovingPlatform::CheckWaypointTransform(int ArrayIndex)
{
	if (bUseWaypointLocation) CheckWaypointLocation(ArrayIndex);
	if (bUseWaypointRotation) CheckWaypointRotation(ArrayIndex);
}

void AMovingPlatform::UpdateWaypointArray()
{
	int AmountSplinepoints = Spline->GetNumberOfSplinePoints();
	int AmountWaypoints = WaypointMeshArray.Num();

	if (AmountSplinepoints > AmountWaypoints)
	{
		for (int i = 0; i < AmountSplinepoints - AmountWaypoints; i++)
		{
			CreateNewWaypoint();
		}
	}
	else if (AmountWaypoints > AmountSplinepoints)
	{
		for (int i = 0; i < AmountWaypoints - AmountSplinepoints; i++)
		{
			DestroyWaypoint(i);
		}
	}
}

void AMovingPlatform::CreateNewWaypoint()
{
	// create the static mesh component
	UStaticMeshComponent* mesh = NewObject<UStaticMeshComponent>(this);
	mesh->AttachTo(RootComponent);
	mesh->RegisterComponent();

	UMaterial* MainMaterial = PlatformMesh->GetMaterial(0)->GetMaterial();
	UStaticMesh* MainMesh = PlatformMesh->GetStaticMesh();

	mesh->SetStaticMesh(MainMesh);
	mesh->SetMaterial(0, MainMaterial);

	if (ensure(mesh))
	{
		WaypointMeshArray.Add(mesh);
		UE_LOG(LogTemp, Warning, TEXT("New Waypoint Created"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("New Waypoint Creation Failed"));
	}
}

void AMovingPlatform::DestroyWaypoint(int ArrayIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("Waypoint Destroyed"));
	UStaticMeshComponent* MeshToRemove = WaypointMeshArray[ArrayIndex];
	WaypointMeshArray.RemoveAt(ArrayIndex);
	MeshToRemove->DestroyComponent();
}

void AMovingPlatform::UpdatePlatformTransform()
{
	UpdatePlatformLocation();
	if (bUseWaypointRotation) UpdatePlatformRotation();
}

void AMovingPlatform::UpdatePlatformLocation()
{
	FVector NextLocation = Spline->GetLocationAtDistanceAlongSpline(DistanceTravelled, ESplineCoordinateSpace::World);

	PlatformMesh->SetWorldLocation(NextLocation);
}

void AMovingPlatform::UpdatePlatformRotation()
{
	FRotator NextRotation = Spline->GetRotationAtDistanceAlongSpline(DistanceTravelled, ESplineCoordinateSpace::World);

	PlatformMesh->SetWorldRotation(NextRotation);
}

void AMovingPlatform::OnConstruction(const FTransform& Transform)
{
	UE_LOG(LogTemp, Warning, TEXT("%d"), WaypointMeshArray.Num());
	if (ensure(Spline))
	{
		UpdateWaypointArray();

		if (ensure(PlatformMesh))
		{
			//Loop through Each waypointmesh
			for (int i = 0; i < WaypointMeshArray.Num(); i++)
			{
				CheckWaypointMesh(i);
				CheckWaypointTransform(i);
			}
		}
	}
}


// Called every frame
void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DistanceTravelled += DeltaTime * PlatformSpeed;

	if(DistanceTravelled >= Spline->GetDistanceAlongSplineAtSplinePoint(CurrentPoint))
	{
		CurrentPoint++;

		if (CurrentPoint > Spline->GetNumberOfSplinePoints())
		{
			CurrentPoint = 0;
			DistanceTravelled = 0;
		}
	}
	else
	{
		UpdatePlatformTransform();
	}




}
