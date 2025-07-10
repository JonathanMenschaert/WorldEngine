// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TreeData.h"
#include "TreeSpawnerData.generated.h"


USTRUCT(BlueprintType, Blueprintable)
struct TREELITH_API FLeafSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	ELeafType LeafType{ ELeafType::OAK };

	UPROPERTY(EditAnywhere)
	float MinLeafDistance{ 150 };

	UPROPERTY(EditAnywhere)
	float MaxLeafDistance{ 250 };

	UPROPERTY(EditAnywhere)
	int NumLeavesPerBranch{ 10 };

	UPROPERTY(EditAnywhere)
	int MaxChildPerLeafBranch{ 0 };

	UPROPERTY(EditAnywhere)
	int IgnoreAmountBranchesFromBottom{ 2 };

	UPROPERTY(EditAnywhere)
	FVector2D LeafCardHalfDimensions{ 100.f, 100.f };

	UPROPERTY(EditAnywhere)
	FVector2D LeafCardDivisions{ 3.f, 3.f };

	UPROPERTY(EditAnywhere)
	FVector2D LeafCardZeroPoint{ 0.f, 0.f };

	UPROPERTY(EditAnywhere)
	FVector2D MinMaxPitchRotation{ -PI, PI };

	UPROPERTY(EditAnywhere)
	FVector2D MinMaxRollhRotation{ -PI, PI };
};

UCLASS(BlueprintType, Blueprintable)
class TREELITH_API UTreeSpawnerData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	ETreeType TreeType{ ETreeType::DEFAULT };

	UPROPERTY(EditAnywhere)
	ETreeRandomType RandomType{ ETreeRandomType::DEFAULT };

	UPROPERTY(EditAnywhere)
	int BranchDestinationAmount{ 20 };

	UPROPERTY(EditAnywhere)
	float MinBranchLength{ 50.f };

	UPROPERTY(EditAnywhere)
	float MaxBranchLength{ 100.f };

	UPROPERTY(EditAnywhere)
	int NumSides{ 4 };

	UPROPERTY(EditAnywhere)
	int GrowIterations{ 8 };

	UPROPERTY(EditAnywhere)
	float MinBranchRadius{ 6.f };
	 
	UPROPERTY(EditAnywhere)
	float UVLength{ 100.f };

	UPROPERTY(EditAnywhere)
	FLeafSettings LeafSettings{};

	UPROPERTY(EditAnywhere)
	TArray<UCurveFloat*> BranchShapes;

	UPROPERTY(EditAnywhere)
	float BranchShapeMultiplier{ 6.f };

	UPROPERTY(EditAnywhere)
	TArray<UCurveFloat*> RootShapes;

	UPROPERTY(EditAnywhere)
	float RootShapeMultiplier{ 6.f };
	
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
};

