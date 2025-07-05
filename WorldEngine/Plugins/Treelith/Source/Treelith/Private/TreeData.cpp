// Fill out your copyright notice in the Description page of Project Settings.


#include "TreeData.h"



FTreeBranch::FTreeBranch()
{
}

FTreeBranch::FTreeBranch(int currentIdx, int parentIdx, float currentBranchLength, const FVector& position, const FVector& direction)
	: CurrentIdx{currentIdx}
	, ParentIdx{ parentIdx }
	, Position{ position }
	, BranchDir{ direction }
	, NextDir{ direction }
	, BranchLength{currentBranchLength}
{
}

void FTreeBranch::Next(TArray<FTreeBranch>& branchList, float nextBranchLength, int nextIdx)
{
	NextDir.Normalize();
	float dotAngle{ static_cast<float>(FVector::DotProduct(BranchDir, NextDir)) };
	if (dotAngle < 0.f)
	{
		NextDir = -NextDir;
	}	

	FVector nextPos = Position + NextDir * nextBranchLength;

	FTreeBranch nextBranch{nextIdx, CurrentIdx, nextBranchLength, nextPos, NextDir};

	branchList.Emplace(nextBranch);
	ChildIdxs.Add(nextIdx);

	NextDir = BranchDir;
	ShouldCreateNext = false;
}

void FTreeBranch::AddDirection(const FVector& newDir)
{
	NextDir += newDir;
	ShouldCreateNext = true;
}
