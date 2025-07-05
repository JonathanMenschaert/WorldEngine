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

void ATreeSpawner::InitializeSpawner(int seed, const TArray<FTreeSettings>& treeSettings)
{
	TreeSettings = treeSettings;
	Trees.AddDefaulted(treeSettings.Num());
	Seed.Initialize(seed);
}

// Called when the game starts or when spawned
void ATreeSpawner::BeginPlay()
{
	Super::BeginPlay();	
}

void ATreeSpawner::GenerateTreeSkeleton()
{	
	for (int i{}; i < TreeSettings.Num(); ++i)
	{
		GenerateTreeSkeleton(TreeSettings[i], Trees[i]);
		FinalizeTreeSkeleton(Trees[i]);
	}
}

void ATreeSpawner::GenerateTreeMesh()
{
	//TODO add amount to reserve for performance reasons
	Vertices.Empty();
	Triangles.Empty();
	UV0.Empty();
	TreeMesh->ClearAllMeshSections();

	for (int i{}; i < TreeSettings.Num(); ++i)
	{
		GenerateNextBranchMesh(TreeSettings[i], i, Trees[i].Branches[0]);
	}

	TArray<FVector> Normals{};
	TArray<FProcMeshTangent> Tangents{};
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UV0, Normals, Tangents);
	TreeMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV0, TArray<FLinearColor>{}, Tangents, true);
	TreeMesh->SetMaterial(0, BarkMaterial);
}

void ATreeSpawner::GenerateTreeSkeleton(const FTreeSettings& currentSettings, FTreeSkeleton& currentTreeSkeleton)
{

	currentTreeSkeleton.Branches.Reserve(currentSettings.BranchDestinationAmount);
	auto& randomizeBranchLeaf = UTreeFunctionRegistry::GetTreeRandomizationFunction(currentSettings.RandomType);

	FTreeBranch root{};
	randomizeBranchLeaf(Seed, currentTreeSkeleton.Leaves, root.BranchDir, currentSettings.BranchDestinationAmount);

	root.Position = currentSettings.Position;
	root.NextDir = root.BranchDir;
	root.ParentIdx = -1;


	auto& currentBranches{ currentTreeSkeleton.Branches };
	currentBranches.Add(root);

	int currentBranchIdx{ currentBranches.Num() - 1 };
	bool foundLeaf{ false };

	while (!foundLeaf)
	{
		UE_LOG(LogTreelith, Log, TEXT("Trunk Growing..."));
		for (FTreeBranchLeaf& leaf : currentTreeSkeleton.Leaves)
		{
			float d = FVector::Distance(currentBranches[currentBranchIdx].Position, leaf.Position);
			if (d < currentSettings.MaxLeafDistance)
			{
				foundLeaf = true;
			}			
		}
		if (!foundLeaf)
		{
			currentBranches[currentBranchIdx].Next(currentBranches, Seed.FRandRange(currentSettings.MinBranchLength, currentSettings.MaxBranchLength), currentBranches.Num());
			currentBranchIdx = currentBranches.Num() - 1;
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

void ATreeSpawner::FinalizeTreeSkeleton(FTreeSkeleton& currentTreeSkeleton)
{
	for (FTreeBranch& branch : currentTreeSkeleton.Branches)
	{
		if (branch.ChildIdxs.Num() == 0)
		{
			currentTreeSkeleton.EndBranches.Add(branch.CurrentIdx);
			IncrementBranchSizeAndPropagate(currentTreeSkeleton, branch);
		}
	}
}

void ATreeSpawner::IncrementBranchSizeAndPropagate(FTreeSkeleton& currentTreeSkeleton, FTreeBranch& currentBranch, int size)
{
	currentBranch.BranchSize += size;
	if (currentBranch.ParentIdx >= 0)
	{
		IncrementBranchSizeAndPropagate(currentTreeSkeleton, currentTreeSkeleton.Branches[currentBranch.ParentIdx], size);
	}
}

void ATreeSpawner::GenerateNextBranchMesh(const FTreeSettings& currentSettings, int currentTreeIdx, const FTreeBranch& currentBranch, int attachOffset)
{
	int nextOffset{ Vertices.Num() };

	if (currentBranch.ChildIdxs.Num() > 0)
	{
		int closestBranchIdx{ -1 };
		float closestBranchVal{ 100000.f };
		for (int currentChildIdx : currentBranch.ChildIdxs)
		{
			FTreeBranch& childBranch{ Trees[currentTreeIdx].Branches[currentChildIdx] };			
			childBranch.ParentVertexStart = -1;

			float branchVal = FMath::Acos(FVector::DotProduct(childBranch.BranchDir, currentBranch.BranchDir));

			if (branchVal < closestBranchVal)
			{
				closestBranchVal = branchVal;
				closestBranchIdx = currentChildIdx;
			}
		}

		FTreeBranch& closestBranch{ Trees[currentTreeIdx].Branches[closestBranchIdx] };
		closestBranch.ParentVertexStart = nextOffset;

		float avgSize{ currentBranch.BranchSize };
		FVector avgBranchDir{currentBranch.BranchDir};

		if (currentBranch.ParentIdx >= 0)
		{
			avgBranchDir += closestBranch.BranchDir;
			avgBranchDir.Normalize();

			avgSize += Trees[currentTreeIdx].Branches[currentBranch.ParentIdx].BranchSize;
			avgSize /= 2.f;
		}

		GenerateNextBranchRing(currentSettings, currentBranch, avgBranchDir, avgSize, attachOffset, nextOffset);

		for (int currentChildIdx : currentBranch.ChildIdxs)
		{
			FTreeBranch& childBranch{ Trees[currentTreeIdx].Branches[currentChildIdx] };

			if (childBranch.ParentVertexStart < 0)
			{
				childBranch.ParentVertexStart = Vertices.Num();
				GenerateNextBranchRing(currentSettings, currentBranch, childBranch.BranchDir, childBranch.BranchSize, -1, childBranch.ParentVertexStart);
			}

			GenerateNextBranchMesh(currentSettings, currentTreeIdx, childBranch, childBranch.ParentVertexStart);
		}
	}
	else
	{
		GenerateNextBranchRing(currentSettings, currentBranch, currentBranch.BranchDir, currentBranch.BranchSize / 2.f, attachOffset, nextOffset);
		GenerateBranchCap(currentSettings, currentBranch.Position, nextOffset, true);
	}
}

void ATreeSpawner::GenerateNextBranchRing(const FTreeSettings& currentSettings, const FTreeBranch& currentBranch, const FVector& upVector, float minRingRadius, int prevRingOffset, int currentRingOffset)
{
	Vertices.AddDefaulted(currentSettings.NumSides);

	FQuat branchRotator{FQuat::Identity};

	if (FMath::Abs(upVector.Z) < 0.9999f)
	{
		FVector rotationAxis = FVector::CrossProduct(FVector::UpVector, upVector).GetSafeNormal();
		float angleOffset	= FQuat::FindBetweenNormals(FVector::UpVector, upVector).GetAngle();

		branchRotator = FQuat{ rotationAxis, angleOffset };
	}

	int numSides{ currentSettings.NumSides };

	for (int i{}; i < numSides; ++i)
	{
		float angle{ TWO_PI * i / numSides };

		FQuat vertexRotator = FQuat{ FVector::UpVector, angle };
		FVector nextVert = vertexRotator.RotateVector(FVector::RightVector * minRingRadius);
		nextVert = branchRotator.RotateVector(nextVert) + currentBranch.Position;
		Vertices[currentRingOffset + i] = nextVert;

		//outUVs[nextRingOffset + i] = FVector2D{ 1.f / numSides * i, 0.f };
	}

	if (prevRingOffset < 0) return;

	for (int i{}; i < numSides; ++i)
	{
		int next = ((i + 1) % numSides);
		int vert0 = prevRingOffset + i;
		int vert1 = prevRingOffset + next;
		int vert2 = currentRingOffset + i;
		int vert3 = currentRingOffset + next;

		//UE_LOG(LogTemp, Log, TEXT("Vertex values: %i - %i, %i - %i"), vert0, vert2, vert1, vert3);

		Triangles.Add(vert0); Triangles.Add(vert2); Triangles.Add(vert1);
		Triangles.Add(vert1); Triangles.Add(vert2); Triangles.Add(vert3);
	}
}

void ATreeSpawner::GenerateBranchCap(const FTreeSettings& currentSettings, const FVector& position, int capStartOffset, bool copyRing)
{
	int numSides{ currentSettings.NumSides };
	if (copyRing)
	{
		int originalSize{ Vertices.Num() };
		TArray<FVector> temp;
		temp.Append(&Vertices[originalSize - numSides], numSides);
		Vertices.Append(temp);
		capStartOffset += numSides;
	}	

	int capVert = Vertices.Add(position);

	for (int i{}; i < numSides; ++i)
	{
		int vert0 = capStartOffset + i;
		int vert1 = capStartOffset + ((i + 1) % numSides);

		Triangles.Add(vert1); Triangles.Add(vert0); Triangles.Add(capVert);
	}
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



