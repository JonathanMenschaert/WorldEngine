// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TreeData.h"
#include "TreeSpawner.generated.h"

class UProceduralMeshComponent;
class UTreeSpawnerData;

UCLASS(BlueprintType, Blueprintable)
class TREELITH_API ATreeSpawner : public AActor
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TArray<FTreeSkeleton> Trees;
public:
	// Sets default values for this actor's properties
	ATreeSpawner();

	UFUNCTION(BlueprintCallable)
	void InitializeSpawner(int seed, const TArray<FTreeSettings>& treeSpawnerData);

	UFUNCTION(BlueprintCallable)
	void GenerateTreeSkeleton();

	UFUNCTION(BlueprintCallable)
	void GenerateTreeMesh();

	//Tree Skeleton Debug
	UFUNCTION(BlueprintCallable)
	void Debug();

private:

	//Tree Skeleton Generation
	UProceduralMeshComponent* TreeMesh;
	TArray<FTreeSettings> TreeSettings;
	UPROPERTY(EditAnywhere)
	TArray<FTreeBranchDestination> BranchDestinations;
	FRandomStream Seed;


	void GenerateTreeSkeleton(const FTreeSettings& currentSettings, FTreeSkeleton& currentTreeSkeleton);
	void GrowTreeSkeleton(const UTreeSpawnerData* currentSettings, FTreeSkeleton& currentTreeSkeleton, int maxIterations);
	void FinalizeTreeSkeleton(const UTreeSpawnerData* currentSettings, FTreeSkeleton& currentTreeSkeleton, int currentIdx, int currentLevel);

	void UpdateTreeHeightMinMax(FTreeSkeleton& currentTreeSkeleton, float zPos);
	void IncrementBranchSizeAndPropagate(FTreeSkeleton& currentTreeSkeleton, FTreeBranch& currentBranch, float size = 2.f);
	
	//Tree Mesh generation

	UPROPERTY(EditAnywhere)
	TArray<FVector> Vertices;

	UPROPERTY(EditAnywhere)
	TArray<int> Triangles;

	UPROPERTY(EditAnywhere)
	TArray<FVector2D> UV0;

	UPROPERTY(EditAnywhere)
	TArray<FVector2D> UV1;

	UPROPERTY(EditAnywhere)
	TArray<FLinearColor> VertexColors;


	UPROPERTY(EditAnywhere)
	UMaterialInterface* BarkMaterial;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* LeafMaterial;

	void GenerateNextBranchMesh(const UTreeSpawnerData* currentSettings, int currentTreeIdx, FTreeBranch& currentBranch,int attachOffset = -1);
	void GenerateNextBranchRing(const UTreeSpawnerData* currentSettings, int currentTreeIdx, const FTreeBranch& currentBranch, const FVector& upVector, int prevRingOffset, int currentRingOffset, bool ignoreUVOffset = false);
	void GenerateBranchCap(const UTreeSpawnerData* currentSettings, int currentTreeIdx, const FVector& position, int capStartOffset, bool copyRing);

	void GenerateEndBranchLeaves(const UTreeSpawnerData* currentSettings, const FTreeSkeleton& currentTreeSkeleton);
	void GenerateLeafCard(TArray<FVector>& outLeafCard, TArray<FVector2D>& outUVs, TArray<int>& outTriangles, const FVector2D& cardDimensions, const FVector2D& cardDivisions, const FVector& zeroPoint);
};
