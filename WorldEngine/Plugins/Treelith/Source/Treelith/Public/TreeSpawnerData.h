// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TreeData.h"
#include "TreeSpawnerData.generated.h"


UCLASS(BlueprintType, Blueprintable)
class TREELITH_API ULeafCardTemplate : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TArray<FVector> Vertices;

	UPROPERTY(EditAnywhere)
	TArray<int> Triangles;

	UPROPERTY(EditAnywhere)
	TArray<FVector2D> UVs;

	UPROPERTY(EditAnywhere)
	TArray<FLinearColor> VertexColors;
};

UCLASS(BlueprintType, Blueprintable)
class TREELITH_API UTreeSpawnerData : public UDataAsset
{
	GENERATED_BODY()

public:

	//Branch Material Slot
	UPROPERTY(EditAnywhere, Category = "Tree|Material")
	EMaterialSlot TreeMaterialSlot{ EMaterialSlot::SLOT1 };

	//General Skeleton Generation
	UPROPERTY(EditAnywhere, Category = "Tree|Skeleton Generation|General")
	ETreeRandomType RandomType{ ETreeRandomType::DEFAULT };

	UPROPERTY(EditAnywhere, Category = "Tree|Skeleton Generation|General")
	int NumSides{ 4 };

	UPROPERTY(EditAnywhere, Category = "Tree|Skeleton Generation|General")
	int GrowIterations{ 8 };

	UPROPERTY(EditAnywhere, Category = "Tree|Skeleton Generation|General")
	float MinBranchRadius{ 6.f };

	//Root Skeleton Generation
	UPROPERTY(EditAnywhere, Category = "Tree|Skeleton Generation|Root")
	FBoundary3 RandomRootBoundaries{};

	UPROPERTY(EditAnywhere, Category = "Tree|Skeleton Generation|Root")
	float MinRootLength{ 20.f };

	UPROPERTY(EditAnywhere, Category = "Tree|Skeleton Generation|Root")
	float MaxRootLength{ 50.f };

	//Branch Skeleton Generation
	UPROPERTY(EditAnywhere, Category = "Tree|Skeleton Generation|Branches")
	FBoundary3 RandomBranchBoundaries{};

	UPROPERTY(EditAnywhere, Category = "Tree|Skeleton Generation|Branches")
	int BranchDestinationAmount{ 20 };

	UPROPERTY(EditAnywhere, Category = "Tree|Skeleton Generation|Branches")
	float MinBranchLength{ 50.f };

	UPROPERTY(EditAnywhere, Category = "Tree|Skeleton Generation|Branches")
	float MaxBranchLength{ 100.f };

	UPROPERTY(EditAnywhere, Category = "Tree|Skeleton Generation|Branches")
	float MinLeafDistance{ 150 };

	UPROPERTY(EditAnywhere, Category = "Tree|Skeleton Generation|Branches")
	float MaxLeafDistance{ 250 };
	 
	//Branch Mesh Generation
	UPROPERTY(EditAnywhere, Category = "Tree|Mesh Generation")
	float UVLength{ 100.f };

	UPROPERTY(EditAnywhere, Category = "Tree|Mesh Generation")
	TArray<UCurveFloat*> BranchShapes;

	UPROPERTY(EditAnywhere, Category = "Tree|Mesh Generation")
	float BranchShapeMultiplier{ 6.f };

	UPROPERTY(EditAnywhere, Category = "Tree|Mesh Generation")
	TArray<UCurveFloat*> RootShapes;

	UPROPERTY(EditAnywhere, Category = "Tree|Mesh Generation")
	float RootShapeMultiplier{ 6.f };
	

	//Leaf Material Slot
	UPROPERTY(EditAnywhere, Category="Leaves|Material")
	EMaterialSlot LeafMaterialSlot{ EMaterialSlot::SLOT1 };

	//Leaf Skeleton Generation
	UPROPERTY(EditAnywhere, Category = "Leaves|Skeleton Generation")
	int NumLeavesPerBranch{ 10 };

	UPROPERTY(EditAnywhere, Category = "Leaves|Skeleton Generation")
	int MaxChildPerLeafBranch{ 0 };

	UPROPERTY(EditAnywhere, Category = "Leaves|Skeleton Generation")
	int IgnoreAmountBranchesFromBottom{ 2 };

	UPROPERTY(EditAnywhere, Category = "Leaves|Mesh Generation")
	FVector LeafCardZeroPoint{ 0.f, 0.f, 0.f };

	UPROPERTY(EditAnywhere, Category = "Leaves|Mesh Generation")
	FBoundary3 LeafRotations{};
	
	UPROPERTY(EditAnywhere, Category = "Leaves|Mesh Generation")
	TArray<ULeafCardTemplate*> LeafCardTemplates;
};

USTRUCT(BlueprintType, Blueprintable)
struct TREELITH_API FTreeSettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	UTreeSpawnerData* TreeSpawnerData;

	//Relative to actor location
	UPROPERTY(BlueprintReadWrite)
	FVector Position{ 0.f, 0.f, 0.f };

	UPROPERTY(BlueprintReadWrite)
	FVector RootDirection{ 0.f, 0.f, 1.f };
};

