#include "TreeSpawner.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "TreeFunctionRegistry.h"
#include "Treelith.h"

#include "DrawDebugHelpers.h"

// Sets default values
ATreeSpawner::ATreeSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TreeMesh = CreateDefaultSubobject<UProceduralMeshComponent>("ProceduralTreeMesh");
	TreeMesh->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void ATreeSpawner::BeginPlay()
{
	Super::BeginPlay();	
}

void ATreeSpawner::GenerateTreeSkeletonAsync(const TArray<FTreeSettings>& treeSettings)
{
	TreeSettings = treeSettings;
	Trees.AddDefaulted(treeSettings.Num());
	Seed.Initialize(30);
	
	for (int i{}; i < treeSettings.Num(); ++i)
	{
		GenerateTreeSkeleton(treeSettings[i], Trees[i]);
	}

	Debug();
}

void ATreeSpawner::GenerateTreeSkeleton(const FTreeSettings& currentSettings, FTreeSkeleton& currentTreeSkeleton)
{

	currentTreeSkeleton.Branches.Reserve(currentSettings.BranchDestinationAmount);
	auto& randomizeBranchLeaf = UTreeFunctionRegistry::GetTreeRandomizationFunction(currentSettings.RandomType);

	FTreeBranch root{};
	randomizeBranchLeaf(Seed, currentTreeSkeleton.Leaves, root.BranchDir, currentSettings.BranchDestinationAmount);

	root.Position = currentSettings.Position;
	root.NextDir = root.BranchDir;
	currentTreeSkeleton.Branches.Add(root);

	auto currentBranch{ root };
	bool foundLeaf{ false };

	while (!foundLeaf)
	{
		UE_LOG(LogTreelith, Log, TEXT("Trunk Growing..."));
		for (FTreeBranchLeaf& leaf : currentTreeSkeleton.Leaves)
		{
			float d = FVector::Distance(currentBranch.Position, leaf.Position);
			if (d < currentSettings.MaxLeafDistance)
			{
				foundLeaf = true;
				break;
			}

			if (!foundLeaf)
			{
				currentBranch.Next(currentTreeSkeleton.Branches, Seed.FRandRange(currentSettings.MinBranchLength, currentSettings.MaxBranchLength), currentTreeSkeleton.Branches.Num());
				currentBranch = currentTreeSkeleton.Branches.Last();
			}
		}
	}

	GrowTreeSkeleton(currentSettings, currentTreeSkeleton, currentSettings.GrowIterations);
}

void ATreeSpawner::GrowTreeSkeleton(const FTreeSettings& currentSettings, FTreeSkeleton& currentTreeSkeleton, int maxIterations)
{
	if (maxIterations <= 0)
	{
		UE_LOG(LogTreelith, Log, TEXT("Tree Complete"));
		return;
	}

	UE_LOG(LogTreelith, Log, TEXT("Branch Growing..."));
	int closestBranchIdx{-1};

	for (auto& leaf : currentTreeSkeleton.Leaves)
	{
		int maxDistance{ INT32_MAX };
		for (auto& branch : currentTreeSkeleton.Branches)
		{
			float d = FVector::Distance(leaf.Position, branch.Position);
			if (d < currentSettings.MinLeafDistance)
			{
				leaf.IsReached = true;
				closestBranchIdx = branch.CurrentIdx;
			}
			else if (d > currentSettings.MaxLeafDistance)
			{
				continue;
			}
			else if (closestBranchIdx < 0 || d < maxDistance)
			{
				closestBranchIdx = branch.CurrentIdx;
				maxDistance = d;
			}
		}

		if (closestBranchIdx >= 0)
		{
			FTreeBranch& currentBranch{ currentTreeSkeleton.Branches[closestBranchIdx] };
			FVector newDir{ leaf.Position - currentBranch.Position };
			newDir.Normalize();
			currentBranch.AddDirection(newDir);
		}
	}

	currentTreeSkeleton.Leaves.RemoveAll([&](FTreeBranchLeaf& leaf) {return leaf.IsReached; });

	for (int i{ currentTreeSkeleton.Branches.Num() - 1 }; i >= 0; --i)
	{
		auto& branch{ currentTreeSkeleton.Branches[i] };
		if (branch.ShouldCreateNext)
		{
			branch.Next(currentTreeSkeleton.Branches, Seed.FRandRange(currentSettings.MinBranchLength, currentSettings.MaxBranchLength), currentTreeSkeleton.Branches.Num());
		}
	}

	GrowTreeSkeleton(currentSettings, currentTreeSkeleton, maxIterations - 1);
}

void ATreeSpawner::Debug()
{
	FlushPersistentDebugLines(GetWorld());
	for (const FTreeSkeleton& currentTree : Trees)
	{
		for (const FTreeBranch& branch : currentTree.Branches)
		{
			if (branch.ParentIdx >= 0)
			{
				DrawDebugLine(GetWorld(), branch.Position, currentTree.Branches[branch.ParentIdx].Position, FColor::Green, true, -1.f, 0U, 6.f);
			}
		}

		for (const FTreeBranchLeaf& leaf : currentTree.Leaves)
		{

			DrawDebugCircle(GetWorld(), leaf.Position, 5.f, 4, FColor::Red, true, -1.f, 0U, 2.f);

		}
	}
}



