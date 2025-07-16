// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TreeData.generated.h"

UENUM(BlueprintType, Blueprintable)
enum class EMaterialSlot : uint8
{
	SLOT1 = 0,
	SLOT2 = 1,
	SLOT3 = 2,
	SLOT4 = 3,
	SLOT5 = 4,
	SLOT6 = 5,
	SLOT7 = 6,
	SLOT8 = 7,
	SLOT9 = 8,
	SLOT10 = 9,
	SLOT11 = 10,
	SLOT12 = 11,
	SLOT13 = 12,
	SLOT14 = 13,
	SLOT15 = 14,
	SLOT16 = 15,
	COUNT = 16
};

UENUM(BlueprintType, Blueprintable)
enum class ETreeRandomType : uint8
{
	DEFAULT,
	OAK
};


enum class EBranchType : uint8
{
	ROOT,
	TRUNK,
	BRANCH
};

USTRUCT(BlueprintType, Blueprintable)
struct TREELITH_API FBoundary3
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FVector2D MinMaxX{};

	UPROPERTY(EditAnywhere)
	FVector2D MinMaxY{};

	UPROPERTY(EditAnywhere)
	FVector2D MinMaxZ{};
};

USTRUCT(BlueprintType, Blueprintable)
struct TREELITH_API FTreeBranchDestination
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

	UPROPERTY(EditAnywhere)
	FVector Position{};

	UPROPERTY(EditAnywhere)
	FVector BranchDir{ 0.f, 0.f, 1.f };

	UPROPERTY(EditAnywhere)
	FVector NextDir{};

	UPROPERTY(EditAnywhere)
	int CurrentIdx{ 0 };

	UPROPERTY(EditAnywhere)
	int ParentIdx{ -1 };

	UPROPERTY(EditAnywhere)
	TArray<int> ChildIdxs{};

	UPROPERTY(EditAnywhere)
	float BranchSize{ 0.f };

	int BranchLevel{};

	float BranchLength{ 100.f };

	EBranchType BranchType{ EBranchType::ROOT };

	bool ShouldCreateNext{ false };

	int ParentVertexStart{ -1 };

	int BranchShapeIdx{ -1 };

	float UvOffset{ 0.f };

	FTreeBranch();

	FTreeBranch(int currentIdx, int parentIdx, float currentBranchLength, const FVector& position, const FVector& direction, int branchShapeIdx);

	FORCEINLINE FTreeBranch Next(float nextBranchLength, int nextIdx, int branchShapeIdx);

	void CalculateUVOffset(float uvLength, float uvOffset, float parentBranchLength);

	void AddDirection(const FVector& newDir);
};

USTRUCT(BlueprintType, Blueprintable)
struct TREELITH_API FTreeSkeleton
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FTreeBranch> Branches;

	UPROPERTY(EditAnywhere)
	TArray<int> LeafBranches;

	UPROPERTY(EditAnywhere)
	float MinHeight{ FLT_MIN };
	
	UPROPERTY(EditAnywhere)
	float MaxHeight{ FLT_MAX };

	float MaxBranchLevel{};

	FORCEINLINE float GetNormalizedHeight(float zPos) const;
	FORCEINLINE float GetNormalizedBranchLevel(float level) const;
};

