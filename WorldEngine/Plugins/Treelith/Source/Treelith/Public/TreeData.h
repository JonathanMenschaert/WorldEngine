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

	float BranchLength{ 100.f };

	bool ShouldCreateNext{ false };

	int ParentVertexStart{ -1 };

	int BranchShapeIdx{ -1 };

	FTreeBranch();

	FTreeBranch(int currentIdx, int parentIdx, float currentBranchLength, const FVector& position, const FVector& direction, int branchShapeIdx);

	void Next(TArray<FTreeBranch>& branchList, float currentBranchLength, int nextIdx, int branchShapeIdx);

	void AddDirection(const FVector& newDir);
};

USTRUCT(BlueprintType, Blueprintable)
struct TREELITH_API FTreeSkeleton
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FTreeBranch> Branches;

	UPROPERTY(EditAnywhere)
	TArray<FTreeBranchLeaf> Leaves;

	UPROPERTY(EditAnywhere)
	TArray<int> EndBranches;
	
};

