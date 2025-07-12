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

void FTreeBranch::Next(TArray<FTreeBranch>& branchList, float nextBranchLength, int nextIdx, int branchShapeIdx)
{
	NextDir.Normalize();
	float dotAngle{ static_cast<float>(FVector::DotProduct(BranchDir, NextDir)) };
	if (dotAngle < 0.f)
	{
		NextDir = -NextDir;
	}	

	FVector nextPos = Position + NextDir * BranchLength;

	FTreeBranch nextBranch{nextIdx, CurrentIdx, nextBranchLength, nextPos, NextDir, branchShapeIdx};
	branchList.Emplace(nextBranch);
	ChildIdxs.Add(nextIdx);

	NextDir = BranchDir;
	ShouldCreateNext = false;
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

float FTreeSkeleton::GetNormalizedHeight(float zPos)
{
	return FMath::Clamp((zPos - MinHeight) / (MaxHeight - MinHeight), 0.f, FLT_MAX);
}
