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
	UPROPERTY(EditAnywhere, Category = "Tree|Material", meta = (ToolTip = "Select the material slot this tree is going to use in the material instance"))
	EMaterialSlot TreeMaterialSlot{ EMaterialSlot::SLOT1 };

	//General Skeleton Generation
	UPROPERTY(EditAnywhere, Category = "Tree|Skeleton Generation|General", meta = (ToolTip = "Selects the randomization function to generate branch destination points"))
	ETreeRandomType RandomType{ ETreeRandomType::DEFAULT };

	UPROPERTY(EditAnywhere, Category = "Tree|Skeleton Generation|General", meta = (ToolTip = "Amount of sides the tree should have"))
	int NumSides{ 4 };

	UPROPERTY(EditAnywhere, Category = "Tree|Skeleton Generation|General", meta = (ToolTip = "Amount of times the growth algorithm will be executed on this tree"))
	int GrowIterations{ 8 };

	UPROPERTY(EditAnywhere, Category = "Tree|Skeleton Generation|Root", meta = (ToolTip = "Maximum amount of trunk branches"))
	int MaxTrunkBranches{ 2 };

	UPROPERTY(EditAnywhere, Category = "Tree|Skeleton Generation|General", meta = (ToolTip = "Curve between 0-1 that shows the shape of the tree"))
	UCurveFloat* TreeShapeCurve{};

	UPROPERTY(EditAnywhere, Category = "Tree|Skeleton Generation|General", meta = (ToolTip = "Shape Curve multiplier"))
	float TreeShapeCurveMultiplier{4.f};


	//Root Skeleton Generation
	UPROPERTY(EditAnywhere, Category = "Tree|Skeleton Generation|Root", meta = (ToolTip = "Boundaries in which the roots of this tree grow. Used in the randomization function"))
	FBoundary3 RandomRootBoundaries{};

	UPROPERTY(EditAnywhere, Category = "Tree|Skeleton Generation|Root", meta = (ToolTip = "Minimum length of the root branch"))
	float MinRootLength{ 20.f };

	UPROPERTY(EditAnywhere, Category = "Tree|Skeleton Generation|Root", meta = (ToolTip = "Maximum length of the root branch"))
	float MaxRootLength{ 50.f };


	UPROPERTY(EditAnywhere, Category = "Tree|Skeleton Generation|General", meta = (ToolTip = "Curve between 0-1 that shows the shape of the root"))
	UCurveFloat* RootShapeCurve{};

	UPROPERTY(EditAnywhere, Category = "Tree|Skeleton Generation|General", meta = (ToolTip = "Shape Curve multiplier"))
	float RootShapeCurveMultiplier{4.f};

	//Branch Skeleton Generation
	UPROPERTY(EditAnywhere, Category = "Tree|Skeleton Generation|Branches", meta = (ToolTip = "Boundaries in which the branches of this tree grow. Used in the randomization function"))
	FBoundary3 RandomBranchBoundaries{};

	UPROPERTY(EditAnywhere, Category = "Tree|Skeleton Generation|Branches", meta = (ToolTip = "Amount of branch destination points to generate"))
	int BranchDestinationAmount{ 20 };

	UPROPERTY(EditAnywhere, Category = "Tree|Skeleton Generation|Branches", meta = (ToolTip = "Minimum length of a branch"))
	float MinBranchLength{ 50.f };

	UPROPERTY(EditAnywhere, Category = "Tree|Skeleton Generation|Branches", meta = (ToolTip = "Maximum length of a branch"))
	float MaxBranchLength{ 100.f };

	UPROPERTY(EditAnywhere, Category = "Tree|Skeleton Generation|Branches", meta = (ToolTip = "Minimal detection distance to a destination point"))
	float MinLeafDistance{ 150 };

	UPROPERTY(EditAnywhere, Category = "Tree|Skeleton Generation|Branches", meta = (ToolTip = "Maximum detection distance to a destination point"))
	float MaxLeafDistance{ 250 };
	 
	//Branch Mesh Generation
	UPROPERTY(EditAnywhere, Category = "Tree|Mesh Generation", meta = (ToolTip = "Sets the length of a branch that maps to the 0-1 coordinates of the UV map"))
	float UVLength{ 100.f };

	UPROPERTY(EditAnywhere, Category = "Tree|Mesh Generation", meta = (ToolTip = "Array of curves that add irregular offsets to the branches to avoid uniform branch shapes"))
	TArray<UCurveFloat*> BranchShapes;

	UPROPERTY(EditAnywhere, Category = "Tree|Mesh Generation", meta = (ToolTip = "Multiplies the branch shape to increase or decrease the irregularity"))
	float BranchShapeMultiplier{ 6.f };

	UPROPERTY(EditAnywhere, Category = "Tree|Mesh Generation", meta = (ToolTip = "Array of curves that add irregular offsets to the root branches to avoid uniform root shapes"))
	TArray<UCurveFloat*> RootShapes;

	UPROPERTY(EditAnywhere, Category = "Tree|Mesh Generation", meta = (ToolTip = "Multiplies the root branch shape to increase or decrease the irregularity"))
	float RootShapeMultiplier{ 6.f };
	

	//Leaf Material Slot
	UPROPERTY(EditAnywhere, Category="Leaves|Material", meta = (ToolTip = "Select the material slot the leaves are going to use in the material instance"))
	EMaterialSlot LeafMaterialSlot{ EMaterialSlot::SLOT1 };

	//Leaf Skeleton Generation
	UPROPERTY(EditAnywhere, Category = "Leaves|Skeleton Generation", meta = (ToolTip = "Sets the number of leaf cards per branch"))
	int NumLeavesPerBranch{ 10 };

	UPROPERTY(EditAnywhere, Category = "Leaves|Skeleton Generation", meta = (ToolTip = "Sets the maximum children a branch is allowed to have before leaves grow on it"))
	int MaxChildPerLeafBranch{ 0 };

	UPROPERTY(EditAnywhere, Category = "Leaves|Skeleton Generation", meta = (ToolTip = "Ignore root branches for leaf generation"))
	bool IgnoreRootBranches{ true };

	UPROPERTY(EditAnywhere, Category = "Leaves|Skeleton Generation", meta = (ToolTip = "Number of branches counted from the root where no leaves will grow, even if other conditions match. Based on branch ID"))
	int IgnoreAmountBranchesFromBottom{ 2 };

	UPROPERTY(EditAnywhere, Category = "Leaves|Skeleton Generation", meta = (ToolTip = "Spread percentage over the leaf branch"))
	float LeavesSpreadPercentage{ 0.f };

	UPROPERTY(EditAnywhere, Category = "Leaves|Mesh Generation", meta = (ToolTip = "Offsets the leafcard by the Zeropoint coordinates"))
	FVector LeafCardZeroPoint{ 0.f, 0.f, 0.f };

	UPROPERTY(EditAnywhere, Category = "Leaves|Mesh Generation", meta = (ToolTip = "Set rotation limits for the leafcards"))
	FBoundary3 LeafRotations{};
	
	UPROPERTY(EditAnywhere, Category = "Leaves|Mesh Generation", meta = (ToolTip = "Sets the leafcards the algorithm can pick"))
	TArray<ULeafCardTemplate*> LeafCardTemplates;

	UPROPERTY(EditAnywhere, Category = "Leaves|Mesh Generation", meta = (ToolTip = "Curve between 0-1 that shows the shape of the leaves"))
	UCurveFloat* LeavesShapeCurve{};

	UPROPERTY(EditAnywhere, Category = "Leaves|Mesh Generation", meta = (ToolTip = "Size multiplier"))
	float LeavesShapeCurveMultiplier{ 4.f };
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

