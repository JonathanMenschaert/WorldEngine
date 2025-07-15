// Fill out your copyright notice in the Description page of Project Settings.


#include "TreeData.h"



FTreeBranch::FTreeBranch()
{
}


FTreeBranch::FTreeBranch(int currentIdx, int parentIdx, float currentBranchLength, const FVector& position, const FVector& direction, int branchShapeIdx)
	: Position{ position }
	, BranchDir{ direction }
	, NextDir{ direction }
	, CurrentIdx{currentIdx}
	, ParentIdx{ parentIdx }
	, BranchLength{currentBranchLength}
	, BranchShapeIdx{branchShapeIdx}
{
}

FORCEINLINE FTreeBranch FTreeBranch::Next(float nextBranchLength, int nextIdx, int branchShapeIdx)
{
	NextDir.Normalize();
	float dotAngle{ static_cast<float>(FVector::DotProduct(BranchDir, NextDir)) };
	if (dotAngle < 0.f)
	{
		NextDir = -NextDir;
	}	

	FVector nextPos = Position + NextDir * BranchLength;

	FTreeBranch nextBranch{nextIdx, CurrentIdx, nextBranchLength, nextPos, NextDir, branchShapeIdx};
	ChildIdxs.Add(nextIdx);

	NextDir = BranchDir;
	ShouldCreateNext = false;

	return nextBranch;
}

void FTreeBranch::CalculateUVOffset(float uvLength, float uvOffset, float parentBranchLength)
{
	if (parentBranchLength > 0.f)
	{
		UvOffset = uvOffset + (parentBranchLength / uvLength);
	}
}

void FTreeBranch::AddDirection(const FVector& newDir)
{
	NextDir += newDir;
	ShouldCreateNext = true;
}

FORCEINLINE float FTreeSkeleton::GetNormalizedHeight(float zPos)
{
	return FMath::Clamp((zPos - MinHeight) / (MaxHeight - MinHeight), 0.f, FLT_MAX);
}

FORCEINLINE float FTreeSkeleton::GetNormalizedBranchLevel(float level)
{
	return level / MaxBranchLevel;
}
