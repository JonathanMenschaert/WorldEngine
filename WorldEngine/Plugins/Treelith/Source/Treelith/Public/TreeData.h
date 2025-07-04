// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TreeData.generated.h"

UENUM(BlueprintType, Blueprintable)
enum class ETreeType : uint8
{
	DEFAULT,
	OAK
};

UENUM(BlueprintType, Blueprintable)
enum class ELeafType : uint8
{
	DEFAULT,
	OAK
};

UENUM(BlueprintType, Blueprintable)
enum class ETreeRandomType : uint8
{
	DEFAULT,
	OAK
};

USTRUCT(BlueprintType, Blueprintable)
struct TREELITH_API FTreeSettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	ETreeType TreeType{ ETreeType::DEFAULT };

	ETreeRandomType RandomType{ ETreeRandomType::DEFAULT };

	UPROPERTY(BlueprintReadWrite)
	int BranchDestinationAmount{ 20 };

	UPROPERTY(BlueprintReadWrite)
	float MinBranchLength{ 0.f };

	UPROPERTY(BlueprintReadWrite)
	float MaxBranchLength{ 100.f };

	UPROPERTY(BlueprintReadWrite)
	int NumSides{ 4 };

	UPROPERTY(BlueprintReadWrite)
	int GrowIterations{ 8 };

	UPROPERTY(BlueprintReadWrite)
	ELeafType LeafType{ ELeafType::DEFAULT };

	UPROPERTY(BLueprintReadWrite)
	float MinLeafDistance{ 150 };

	UPROPERTY(BLueprintReadWrite)
	float MaxLeafDistance{ 250 };

	UPROPERTY(BlueprintReadWrite)
	int NumLeavesPerBranch { 10 };

	//Relative to actor location
	UPROPERTY(BlueprintReadWrite)
	FVector Position{0.f, 0.f, 0.f};
};

USTRUCT(BlueprintType, Blueprintable)
struct TREELITH_API FTreeBranchLeaf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FVector Position{};
	bool IsReached{ false };
};


USTRUCT(BlueprintType, Blueprintable)
struct TREELITH_API FTreeBranch
{
	GENERATED_BODY()

	FVector Position{};
	FVector BranchDir{ 0.f, 0.f, 1.f };

	FVector NextDir{};

	int ParentIdx{ -1 };
	TArray<int> ChildIdxs{};

	float BranchSize{ 6.f };
};
