// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformSystem.h"
#include "Components/SplineComponent.h"
#include "Components/StaticMeshComponent.h"


//For Widget
#include "Editor/UMGEditor/Public/WidgetBlueprint.h"
#include "Editor/LevelEditor/Public/LevelEditor.h"
#include "Runtime/Core/Public/Modules/ModuleManager.h"
#include "Editor/Blutility/Public/IBlutilityModule.h"
#include "Editor/Blutility/Classes/EditorUtilityWidgetBlueprint.h"
#include "Templates/SharedPointer.h"

#define DEBUG_MSG(x, ...) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT(x), __VA_ARGS__));}

// Sets default values
APlatformSystem::APlatformSystem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SetRootComponent(Spline);

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Platform Mesh"));
	PlatformMesh->SetupAttachment(Spline);

	WaypointMeshArray.Empty();

	CurrentPoint = StartPoint;
}

// Called when the game starts or when spawned
void APlatformSystem::BeginPlay()
{
	Super::BeginPlay();

	CurrentPoint = StartPoint;

	for (int i = 0; i < WaypointMeshArray.Num(); i++)
	{
		if (ensure(WaypointMeshArray[i]))
		{
			WaypointMeshArray[i]->SetVisibility(false);
			WaypointMeshArray[i]->SetHiddenInGame(true);
			WaypointMeshArray[i]->SetActive(false);
			WaypointMeshArray[i]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void APlatformSystem::CheckWaypointMesh(int ArrayIndex)
{
	if (ensure(WaypointMeshArray[ArrayIndex]))
	{
		UStaticMesh* WaypointMesh = WaypointMeshArray[ArrayIndex]->GetStaticMesh();
		UMaterialInterface* WaypointMaterial = WaypointMeshArray[ArrayIndex]->GetMaterial(0);

		if (ensure(PlatformMesh))
		{
			UMaterialInterface* MainMaterial = PlatformMesh->GetMaterial(0);
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
	}

}

void APlatformSystem::CheckWaypointLocation(int ArrayIndex)
{
	if (ensure(Spline))
	{
		if (ensure(WaypointMeshArray[ArrayIndex]))
		{
			FVector SplinepointLocation = Spline->GetLocationAtSplinePoint(ArrayIndex, ESplineCoordinateSpace::World);
			FVector MeshLocation = WaypointMeshArray[ArrayIndex]->GetComponentLocation();

			if (MeshLocation != SplinepointLocation)
			{
				WaypointMeshArray[ArrayIndex]->SetWorldLocation(SplinepointLocation);
			}
		}
	}
}

void APlatformSystem::CheckWaypointRotation(int ArrayIndex)
{
	if (ensure(Spline))
	{
		if (ensure(WaypointMeshArray[ArrayIndex]))
		{
			FRotator SplinepointRotation = Spline->GetRotationAtSplinePoint(ArrayIndex, ESplineCoordinateSpace::World);
			FRotator MeshRotation = WaypointMeshArray[ArrayIndex]->GetComponentRotation();

			if (MeshRotation != SplinepointRotation)
			{
				WaypointMeshArray[ArrayIndex]->SetWorldRotation(SplinepointRotation);
			}
		}
	}
}

void APlatformSystem::CheckWaypointTransform(int ArrayIndex)
{
	if (bUseWaypointLocation) CheckWaypointLocation(ArrayIndex);
	if (bUseWaypointRotation) CheckWaypointRotation(ArrayIndex);
	else
	{
		if (ensure(WaypointMeshArray[ArrayIndex])) WaypointMeshArray[ArrayIndex]->SetWorldRotation(PlatformMesh->GetComponentRotation());
	}
	if (ensure(WaypointMeshArray[ArrayIndex])) WaypointMeshArray[ArrayIndex]->SetWorldScale3D(PlatformMesh->GetComponentScale());


}

void APlatformSystem::UpdateWaypointArray()
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

void APlatformSystem::CreateNewWaypoint()
{
	// create the static mesh component
	UStaticMeshComponent* mesh = NewObject<UStaticMeshComponent>(this);
	mesh->RegisterComponent();

	UMaterialInterface* MainMaterial = PlatformMesh->GetMaterial(0);
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

void APlatformSystem::DestroyWaypoint(int ArrayIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("Waypoint Destroyed"));
	UStaticMeshComponent* MeshToRemove = WaypointMeshArray[ArrayIndex];
	WaypointMeshArray.RemoveAt(ArrayIndex);
	MeshToRemove->DestroyComponent();
}

void APlatformSystem::UpdatePlatformTransform()
{
	UpdatePlatformLocation();
	if (bUseWaypointRotation) UpdatePlatformRotation();
}

void APlatformSystem::UpdatePlatformLocation()
{
	FVector NextLocation = Spline->GetLocationAtDistanceAlongSpline(DistanceTravelled, ESplineCoordinateSpace::World);

	PlatformMesh->SetWorldLocation(NextLocation);
}

void APlatformSystem::UpdatePlatformRotation()
{
	FRotator NextRotation = Spline->GetRotationAtDistanceAlongSpline(DistanceTravelled, ESplineCoordinateSpace::World);

	PlatformMesh->SetWorldRotation(NextRotation);
}

void APlatformSystem::ValidateStartPoint()
{
	int min = 0;
	int max = Spline->GetNumberOfSplinePoints();

	if (StartPoint < min) StartPoint = min;
	else if (StartPoint > max) StartPoint = max;
}

void APlatformSystem::UpdatePlatformStartPosition()
{
	int max = WaypointMeshArray.Num() - 1;
	bool CurrentWaypointWithinBounds = CurrentPoint <= max && CurrentPoint >= 0;
	bool StartWaypointWithinBounds = StartPoint <= max && StartPoint >= 0;

	if (CurrentWaypointWithinBounds)
	{
		if (ensure(WaypointMeshArray[CurrentPoint]))
		{
			if (StartWaypointWithinBounds)
			{
				if (ensure(WaypointMeshArray[StartPoint]))
				{
					if (CurrentPoint != StartPoint)
					{
						WaypointMeshArray[CurrentPoint]->SetVisibility(true);
						WaypointMeshArray[StartPoint]->SetVisibility(false);
						CurrentPoint = StartPoint;
					}
				}
			}

			if (ensure(PlatformMesh))
			{
				PlatformMesh->SetWorldLocation(WaypointMeshArray[CurrentPoint]->GetComponentLocation());
				PlatformMesh->SetWorldRotation(WaypointMeshArray[CurrentPoint]->GetComponentRotation());
			}

			if (WaypointMeshArray[CurrentPoint]->IsVisible())
			{
				WaypointMeshArray[CurrentPoint]->SetVisibility(false);
			}
		}
	}
}

void APlatformSystem::OnConstruction(const FTransform& Transform)
{
	ValidateStartPoint();
	UpdatePlatformStartPosition();

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

void APlatformSystem::StartWidget(UWidgetBlueprint* Blueprint)
{
	if (Blueprint->GeneratedClass->IsChildOf(UEditorUtilityWidget::StaticClass()))
	{
		DEBUG_MSG("1");
		const UEditorUtilityWidget* CDO = Cast<UEditorUtilityWidget>(Blueprint->GeneratedClass->GetDefaultObject());
		if (CDO->ShouldAutoRunDefaultAction())
		{
			DEBUG_MSG("2");
			// This is an instant-run blueprint, just execute it
			UEditorUtilityWidget* Instance = NewObject<UEditorUtilityWidget>(GetTransientPackage(), Blueprint->GeneratedClass);
			Instance->ExecuteDefaultAction();
		}
		else
		{
			FName RegistrationName = FName(*(Blueprint->GetPathName() + TEXT("_ActiveTab")));
			FText DisplayName = FText::FromString(Blueprint->GetName());
			FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(FName(TEXT("LevelEditor")));
			TSharedPtr<FTabManager> LevelEditorTabManager = LevelEditorModule.GetLevelEditorTabManager();
			TSharedRef<FTabManager> NewDockTab = LevelEditorTabManager.ToSharedRef();
			DEBUG_MSG("3");
		}
	}
}

// Called every frame
void APlatformSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DistanceTravelled += DeltaTime * PlatformSpeed;

	if (DistanceTravelled >= Spline->GetDistanceAlongSplineAtSplinePoint(CurrentPoint))
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


