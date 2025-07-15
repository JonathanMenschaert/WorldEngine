#include "TreeSpawner.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "TreeFunctionRegistry.h"
#include "Treelith.h"
#include "TreeSpawnerData.h"

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
	Trees.AddDefaulted(TreeSettings.Num());
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
		FinalizeTreeSkeleton(TreeSettings[i].TreeSpawnerData, Trees[i]);
	}
}

void ATreeSpawner::GenerateTreeMesh()
{
	//TODO add amount to reserve for performance reasons

	TArray<FVector2D> emptyUVs;

	Vertices.Empty();
	Triangles.Empty();
	UV0.Empty();
	UV1.Empty();
	VertexColors.Empty();
	TreeMesh->ClearAllMeshSections();

	int prevAmountUVValues{0};
	for (int i{}; i < TreeSettings.Num(); ++i)
	{
		GenerateNextBranchMesh(TreeSettings[i].TreeSpawnerData, i, Trees[i].Branches[0]);
		TArray<FVector2D> temp;
		temp.Init(FVector2D{ static_cast<float>(TreeSettings[i].TreeSpawnerData->TreeMaterialSlot) / static_cast<float>(EMaterialSlot::COUNT), 0.5f }, UV0.Num() - prevAmountUVValues);
		UV1.Append(temp);
		prevAmountUVValues = UV0.Num();
	}

	TArray<FVector> Normals{};
	TArray<FProcMeshTangent> Tangents{};
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UV0, Normals, Tangents);
	TreeMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV0, UV1, emptyUVs, emptyUVs, VertexColors, Tangents, true);
	TreeMesh->SetMaterial(0, BarkMaterial);


	//Reset seed to initial to ensure the leaves generate the same way if the tree mesh is regenerated for lower polycounts
	Seed.Reset();
	Vertices.Empty();
	Triangles.Empty();
	UV0.Empty();
	UV1.Empty();
	VertexColors.Empty();
	Normals.Empty();
	Tangents.Empty();

	prevAmountUVValues = 0;
	for (int i{}; i < TreeSettings.Num(); ++i)
	{
		GenerateEndBranchLeaves(TreeSettings[i].TreeSpawnerData, Trees[i]);
		TArray<FVector2D> temp;
		temp.Init(FVector2D{ static_cast<float>(TreeSettings[i].TreeSpawnerData->LeafMaterialSlot) / static_cast<float>(EMaterialSlot::COUNT), 0.5f }, UV0.Num() - prevAmountUVValues);
		UV1.Append(temp);
		prevAmountUVValues = UV0.Num();
	}

	if (Vertices.Num() > 0)
	{
		UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UV0, Normals, Tangents);
		TreeMesh->CreateMeshSection_LinearColor(1, Vertices, Triangles, Normals, UV0, UV1, emptyUVs, emptyUVs, VertexColors, Tangents, false);
		TreeMesh->SetMaterial(1, LeafMaterial);
	}
}

void ATreeSpawner::GenerateTreeSkeleton(const FTreeSettings& currentSettings, FTreeSkeleton& currentTreeSkeleton)
{
	auto spawnerData{ currentSettings.TreeSpawnerData };
	currentTreeSkeleton.Branches.Reserve(spawnerData->BranchDestinationAmount);

	//set up root branch
	int branchRadiusOffsetIdx{ spawnerData->RootShapes.Num() > 0 ? Seed.RandRange(0, spawnerData->RootShapes.Num() - 1) : -1 };
	FTreeBranch root{0, -1, static_cast<float>(Seed.FRandRange(spawnerData->MinRootLength, spawnerData->MaxRootLength)), currentSettings.Position, FVector::UpVector, branchRadiusOffsetIdx};
	root.UvOffset = 0.f;
	root.IsRoot = true;
	currentTreeSkeleton.MinHeight = root.Position.Z;
	currentTreeSkeleton.MaxHeight = root.Position.Z;

	//Grab correct randomizer function from the registry and update root
	auto& randomizeBranchLeaf = UTreeFunctionRegistry::GetTreeRandomizationFunction(spawnerData->RandomType);
	randomizeBranchLeaf(Seed, BranchDestinations, spawnerData->RandomBranchBoundaries, currentSettings.Position, spawnerData->BranchDestinationAmount);
	root.NextDir = root.BranchDir;

	//Generate trunk
	auto& currentBranches{ currentTreeSkeleton.Branches };
	currentBranches.Add(root);

	int currentBranchIdx{ currentBranches.Num() - 1 };
	int maxTrunkBranches{ spawnerData->MaxTrunkBranches };
	bool foundLeaf{ false };

	//Continue to grow until a leaf comes within distance
	while (!foundLeaf && maxTrunkBranches > 0)
	{
		UE_LOG(LogTreelith, Log, TEXT("Trunk Growing..."));
		for (FTreeBranchDestination& branchDest : BranchDestinations)
		{
			float d = FVector::Distance(currentBranches[currentBranchIdx].Position, branchDest.Position);
			if (d < spawnerData->MaxLeafDistance)
			{
				foundLeaf = true;
			}			
		}
		if (!foundLeaf)
		{
			auto& branch{ currentTreeSkeleton.Branches[currentBranchIdx] };
			branchRadiusOffsetIdx = spawnerData->BranchShapes.Num() > 0 ? Seed.RandRange(0, spawnerData->BranchShapes.Num() - 1) : -1;
			FTreeBranch nextBranch{ branch.Next(Seed.FRandRange(spawnerData->MinBranchLength, spawnerData->MaxBranchLength), currentBranches.Num(), Seed.RandRange(0, spawnerData->BranchShapes.Num() - 1)) };
			nextBranch.IsRoot = true;
			currentBranchIdx = nextBranch.CurrentIdx;
			currentTreeSkeleton.MinHeight = (nextBranch.Position.Z - root.Position.Z) / 2.f;

			currentTreeSkeleton.Branches.Emplace(nextBranch);

			--maxTrunkBranches;
		}
	}

	//Start growing branches
	GrowTreeSkeleton(spawnerData, currentTreeSkeleton, spawnerData->GrowIterations);
}

void ATreeSpawner::GrowTreeSkeleton(const UTreeSpawnerData* currentSettings, FTreeSkeleton& currentTreeSkeleton, int maxIterations)
{
	if (maxIterations <= 0)
	{
		UE_LOG(LogTreelith, Log, TEXT("Tree Complete"));
		return;
	}

	UE_LOG(LogTreelith, Log, TEXT("Branch Growing..."));
	int closestBranchIdx{-1};

	//Check what leaves are in distance of the branches
	for (auto& branchDest : BranchDestinations)
	{
		int maxDistance{ INT32_MAX };
		for (auto& branch : currentTreeSkeleton.Branches)
		{
			float d = FVector::Distance(branchDest.Position, branch.Position);
			if (d < currentSettings->MinLeafDistance)
			{
				branchDest.IsReached = true;
				closestBranchIdx = branch.CurrentIdx;
			}
			else if (d > currentSettings->MaxLeafDistance)
			{
				continue;
			}
			else if (closestBranchIdx < 0 || d < maxDistance)
			{
				closestBranchIdx = branch.CurrentIdx;
				maxDistance = d;
			}
		}
		
		//If a branch is found, add the normalized direction to the leaf.
		if (closestBranchIdx >= 0)
		{
			FTreeBranch& currentBranch{ currentTreeSkeleton.Branches[closestBranchIdx] };
			FVector newDir{ branchDest.Position - currentBranch.Position };
			newDir.Normalize();
			currentBranch.AddDirection(newDir);
		}
	}

	//Remove all leaves that have a branch near, indicated by the IsReached variable
	BranchDestinations.RemoveAll([&](FTreeBranchDestination& leaf) {return leaf.IsReached; });

	//Loop over branches and create a child branch to the branches that have a leaf near them
	for (int i{ currentTreeSkeleton.Branches.Num() - 1 }; i >= 0; --i)
	{
		auto& branch{ currentTreeSkeleton.Branches[i] };
		if (branch.ShouldCreateNext)
		{
			int branchRadiusOffsetIdx{ currentSettings->BranchShapes.Num() > 0 ? Seed.RandRange(0, currentSettings->BranchShapes.Num() - 1) : -1 };
			FTreeBranch nextBranch = branch.Next(Seed.FRandRange(currentSettings->MinBranchLength, currentSettings->MaxBranchLength), currentTreeSkeleton.Branches.Num(), branchRadiusOffsetIdx);
			currentTreeSkeleton.Branches.Emplace(nextBranch);
			UpdateTreeHeightMinMax(currentTreeSkeleton, nextBranch.Position.Z);
		}
	}

	GrowTreeSkeleton(currentSettings, currentTreeSkeleton, maxIterations - 1);
}

void ATreeSpawner::FinalizeTreeSkeleton(const UTreeSpawnerData* currentSettings, FTreeSkeleton& currentTreeSkeleton)
{
	//Loop over all branches to add the correct sizing to the trunk and branches.
	for (FTreeBranch& branch : currentTreeSkeleton.Branches)
	{
		branch.BranchSize += currentSettings->MinBranchRadius;

		//Branches with no child branches are added to a separate array to add the leaf foliage to in the mesh generation stage
		if (branch.ChildIdxs.Num() == 0)
		{
			IncrementBranchSizeAndPropagate(currentTreeSkeleton, branch, currentSettings->BranchAddRadius);
		}

		if (branch.ChildIdxs.Num() <= currentSettings->MaxChildPerLeafBranch && (!branch.IsRoot || !currentSettings->IgnoreRootBranches) && branch.CurrentIdx > currentSettings->IgnoreAmountBranchesFromBottom)
		{
			currentTreeSkeleton.LeafBranches.Add(branch.CurrentIdx);
		}
	}
}

void ATreeSpawner::UpdateTreeHeightMinMax(FTreeSkeleton& currentTreeSkeleton, float zPos)
{
	if (zPos < currentTreeSkeleton.MinHeight)
	{
		currentTreeSkeleton.MinHeight = zPos;
	}

	if (zPos > currentTreeSkeleton.MaxHeight)
	{
		currentTreeSkeleton.MaxHeight = zPos;
	}
}

void ATreeSpawner::IncrementBranchSizeAndPropagate(FTreeSkeleton& currentTreeSkeleton, FTreeBranch& currentBranch, float size)
{
	currentBranch.BranchSize += size;
	if (currentBranch.ParentIdx >= 0)
	{
		//Propagate the size down to the trunk, this way the tree looks better
		IncrementBranchSizeAndPropagate(currentTreeSkeleton, currentTreeSkeleton.Branches[currentBranch.ParentIdx], size);
	}
}

void ATreeSpawner::GenerateNextBranchMesh(const UTreeSpawnerData* currentSettings, int currentTreeIdx, FTreeBranch& currentBranch, int attachOffset)
{
	int nextOffset{ Vertices.Num() };

	if (currentBranch.ChildIdxs.Num() > 0)
	{

		//One child branch is chosen to continue the trunk, the childbranch with the closest branchdir to the currentbranch is picked to avoid too sudden twists in the tree
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
		closestBranch.CalculateUVOffset(currentSettings->UVLength, currentBranch.UvOffset, currentBranch.BranchLength);

		//Before the vertex ring is generated, both the size of the tree and the branchdirection are averaged to ensure:
		//- the branch doesn't look squished when connected to the next branch
		//- the tree doesn't have sudden increases or decreases in circumference
		//Exception to this is the root of the tree, as those factors are decided at the start of the algorithm
		float avgSize{ currentBranch.BranchSize };
		FVector avgBranchDir{currentBranch.BranchDir};

		if (currentBranch.ParentIdx >= 0)
		{
			avgBranchDir += closestBranch.BranchDir;
			avgBranchDir.Normalize();

			avgSize += Trees[currentTreeIdx].Branches[currentBranch.ParentIdx].BranchSize;
			avgSize /= 2.f;
		}

		GenerateNextBranchRing(currentSettings, currentTreeIdx, currentBranch, avgBranchDir, avgSize, attachOffset, nextOffset);

		for (int currentChildIdx : currentBranch.ChildIdxs)
		{
			FTreeBranch& childBranch{ Trees[currentTreeIdx].Branches[currentChildIdx] };

			//childbranches that don't continue the trunk are given a separate vertex ring to connect to 
			if (childBranch.ParentVertexStart < 0)
			{
				childBranch.ParentVertexStart = Vertices.Num();
				childBranch.CalculateUVOffset(currentSettings->UVLength, 0.f, childBranch.BranchLength);
				GenerateNextBranchRing(currentSettings, currentTreeIdx, currentBranch, childBranch.BranchDir, childBranch.BranchSize, -1, childBranch.ParentVertexStart, true);
			}

			GenerateNextBranchMesh(currentSettings, currentTreeIdx, childBranch, childBranch.ParentVertexStart);
		}
	}
	else
	{
		//if a branch has no child, a final ring is made, which is then closed off by a cap.
		currentBranch.CalculateUVOffset(currentSettings->UVLength, currentBranch.UvOffset, currentBranch.BranchLength);
		GenerateNextBranchRing(currentSettings, currentTreeIdx, currentBranch, currentBranch.BranchDir, currentBranch.BranchSize / 2.f, attachOffset, nextOffset);
		GenerateBranchCap(currentSettings, currentTreeIdx, currentBranch.Position, nextOffset, true);
	}
}

void ATreeSpawner::GenerateNextBranchRing(const UTreeSpawnerData* currentSettings, int currentTreeIdx, const FTreeBranch& currentBranch, const FVector& upVector, float minRingRadius, int prevRingOffset, int currentRingOffset, bool ignoreUVOffset)
{
	int numSides{ currentSettings->NumSides };
	Vertices.AddDefaulted(numSides + 1);
	UV0.AddDefaulted(numSides + 1);
	VertexColors.AddDefaulted(numSides + 1);
	FQuat branchRotator{FQuat::Identity};

	//If the branchdirection and the world upvector are different enough, a quaternion is prepared to correctly rotate the vertex ring
	if (FMath::Abs(upVector.Z) < 0.9999f)
	{
		FVector rotationAxis = FVector::CrossProduct(FVector::UpVector, upVector).GetSafeNormal();
		float angleOffset = FQuat::FindBetweenNormals(FVector::UpVector, upVector).GetAngle();

		branchRotator = FQuat{ rotationAxis, angleOffset };
	}

	//Retrieve the branch shapes from the spawner data, if possible
	float radiusOffsetMultiplier{ 0.f };
	UCurveFloat* radiusOffsetCurve{ nullptr };
	if (currentBranch.BranchShapeIdx >= 0)
	{
		if (currentBranch.ParentIdx >= 0)
		{
			radiusOffsetCurve = currentSettings->BranchShapes[currentBranch.BranchShapeIdx];
			radiusOffsetMultiplier = currentSettings->BranchShapeMultiplier;
		}
		else
		{
			radiusOffsetCurve = currentSettings->RootShapes[currentBranch.BranchShapeIdx];
			radiusOffsetMultiplier = currentSettings->RootShapeMultiplier;
		}
	}

	float normalizedVertexColor{ Trees[currentTreeIdx].GetNormalizedHeight(currentBranch.Position.Z) };
	float currentUVOffset{ !ignoreUVOffset ? currentBranch.UvOffset : 0.f };

	for (int i{}; i < numSides; ++i)
	{
		float angle{ TWO_PI * i / numSides };

		FQuat vertexRotator = FQuat{ FVector::UpVector, angle };
		float additionalDistance = radiusOffsetCurve ? radiusOffsetCurve->GetFloatValue(angle) : 0.f;

		FVector nextVert = vertexRotator.RotateVector(FVector::RightVector * (minRingRadius + additionalDistance * radiusOffsetMultiplier));
		nextVert = branchRotator.RotateVector(nextVert) + currentBranch.Position;
		Vertices[currentRingOffset + i] = nextVert;
		UV0[currentRingOffset + i] = FVector2D{ 1.f / numSides * i, currentUVOffset };
		VertexColors[currentRingOffset + i] = FLinearColor{normalizedVertexColor, normalizedVertexColor, normalizedVertexColor};
	}
	Vertices[currentRingOffset + numSides] = Vertices[currentRingOffset];
	UV0[currentRingOffset + numSides] = FVector2D{ 1.f, currentBranch.UvOffset };
	VertexColors[currentRingOffset + numSides] = VertexColors[currentRingOffset];

	//If there is no mesh to generate, return
	if (prevRingOffset < 0) return;

	for (int i{}; i < numSides; ++i)
	{
		int next = i + 1;
		int vert0 = prevRingOffset + i;
		int vert1 = prevRingOffset + next;
		int vert2 = currentRingOffset + i;
		int vert3 = currentRingOffset + next;

		Triangles.Add(vert0); Triangles.Add(vert2); Triangles.Add(vert1);
		Triangles.Add(vert1); Triangles.Add(vert2); Triangles.Add(vert3);
	}
}

void ATreeSpawner::GenerateBranchCap(const UTreeSpawnerData* currentSettings, int currentTreeIdx, const FVector& position, int capStartOffset, bool copyRing)
{
	int numSides{ currentSettings->NumSides };

	// Due to the component not supporting multiple uv values per vertex, 
	// the vertices used for the last ring are copied and used for correct uv calculations
	if (copyRing)
	{
		int originalSize{ Vertices.Num() };
		TArray<FVector> vertexTemp;
		vertexTemp.Append(&Vertices[originalSize - numSides], numSides);
		Vertices.Append(vertexTemp);

		TArray<FLinearColor> vertexColorTemp;
		vertexColorTemp.Append(&VertexColors[originalSize - numSides], numSides);
		VertexColors.Append(vertexColorTemp);
		capStartOffset += numSides;
	}	



	int capVert = Vertices.Add(position);
	UV0.AddDefaulted(numSides + 1);
	FLinearColor vertexColor{ VertexColors.Last() };
	VertexColors.Add(vertexColor);

	for (int i{}; i < numSides; ++i)
	{
		int vert0 = capStartOffset + i;
		int vert1 = capStartOffset + ((i + 1) % numSides);

		Triangles.Add(vert1); Triangles.Add(vert0); Triangles.Add(capVert);
	}
}

void ATreeSpawner::GenerateEndBranchLeaves(const UTreeSpawnerData* currentSettings, const FTreeSkeleton& currentTreeSkeleton)
{

	const TArray<ULeafCardTemplate*>& leafCardTemplates{ currentSettings->LeafCardTemplates };
	const FBoundary3& leafRotations{ currentSettings->LeafRotations };
	const TArray<int>& endBranches{ currentTreeSkeleton.LeafBranches };

	for (int i{}; i < endBranches.Num(); ++i)
	{
		const FTreeBranch& branch{ currentTreeSkeleton.Branches[endBranches[i]] };
		FVector branchRight{ FVector::CrossProduct(FVector::UpVector, branch.BranchDir) };
		branchRight.Normalize();

		for (int j{}; j < currentSettings->NumLeavesPerBranch; ++j)
		{
			const ULeafCardTemplate* leafCardTemplate{ leafCardTemplates[Seed.RandRange(0, leafCardTemplates.Num() - 1)] };
			float rollRotValue{ static_cast<float>(Seed.FRandRange(leafRotations.MinMaxX.X, leafRotations.MinMaxX.Y)) };

			//Calculate yaw values 
			FVector signVector{ FVector::CrossProduct(FVector::RightVector, branchRight).GetSafeNormal() };
			float dotValue{ static_cast<float>(FVector::DotProduct(FVector::UpVector, signVector)) };
			float signValue{ dotValue >= 0.f ? 1.f : -1.f };
			float yawDiffValue{ static_cast<float>(FMath::Acos(FVector::DotProduct(FVector::RightVector, branchRight))) };
			float yawRotValue{ static_cast<float>(Seed.FRandRange(leafRotations.MinMaxZ.X, leafRotations.MinMaxZ.Y)) };

			float pitchRotValue{ static_cast<float>(Seed.FRandRange(leafRotations.MinMaxY.X, leafRotations.MinMaxY.Y)) };

			FQuat rotationQuat{ FVector::UpVector, yawRotValue };
			FVector rotationAxis{ rotationQuat.RotateVector(branchRight) };

			FQuat leafRollRotator{ FVector::ForwardVector, rollRotValue };
			FQuat leafYawRotator{ FVector::UpVector, yawDiffValue * signValue + yawRotValue };
			FQuat leafPitchRotator{ rotationAxis, pitchRotValue };

			FQuat leafRotator{ leafYawRotator * leafRollRotator * leafPitchRotator };

			int vertOffset{ Vertices.Num() };

			for (const FVector& leafVertex : leafCardTemplate->Vertices)
			{			
				Vertices.Add(leafRotator.RotateVector(leafVertex + currentSettings->LeafCardZeroPoint) + branch.Position);
			}

			VertexColors.Append(leafCardTemplate->VertexColors);
			UV0.Append(leafCardTemplate->UVs);

			for (int idx{}; idx < leafCardTemplate->Triangles.Num(); ++idx)
			{
				Triangles.Add(leafCardTemplate->Triangles[idx] + vertOffset);
			}
		}
	}
}

void ATreeSpawner::GenerateLeafCard(TArray<FVector>& outLeafCard, TArray<FVector2D>& outUVs, TArray<int>& outTriangles, const FVector2D& cardDimensions, const FVector2D& cardDivisions, const FVector& zeroPoint)
{
	int sizeX{ static_cast<int>(cardDivisions.X)};
	int sizeY{ static_cast<int>(cardDivisions.Y)};
	int size{ (sizeX + 1) * (sizeY + 1)};
	outLeafCard.Reserve(size);
	outUVs.Reserve(size);

	FVector2D fullDimensions{ 2 * cardDimensions };
	FVector2D divisions{ fullDimensions / cardDivisions };
	FVector2D uvDivisions{ 1.f / sizeX, 1.f / sizeY };
	
	for (int i{}; i <= sizeX; ++i)
	{
		for (int j{}; j <= sizeY; ++j)
		{
			outLeafCard.Emplace(FVector{ i * divisions.X - cardDimensions.X + zeroPoint.X, j * divisions.Y - cardDimensions.Y + zeroPoint.Y, zeroPoint.Z });
			outUVs.Emplace(FVector2D{ i * uvDivisions.X, j * uvDivisions.Y });
		}
	}

	for (int i{}; i < sizeX; ++i)
	{
		for (int j{}; j < sizeY; ++j)
		{
			int vert0{ i * (sizeY + 1) + j };			
			int vert1{ vert0 + 1 };
			int vert2{ vert0 + sizeX + 1};			
			int vert3{ vert2 + 1 };
			

			outTriangles.Add(vert0); outTriangles.Add(vert1); outTriangles.Add(vert2);
			outTriangles.Add(vert1); outTriangles.Add(vert3); outTriangles.Add(vert2);
		}
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

		for (const FTreeBranchDestination& branchDest : BranchDestinations)
		{
			DrawDebugCircle(GetWorld(), branchDest.Position, 5.f, 4, FColor::Red, true, -1.f, 0U, 2.f);
		}
	}
}