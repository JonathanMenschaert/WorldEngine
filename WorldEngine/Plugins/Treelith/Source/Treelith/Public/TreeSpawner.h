// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TreeData.h"
#include "TreeSpawner.generated.h"

class UProceduralMeshComponent;

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
	void InitializeSpawner(int seed, const TArray<FTreeSettings>& treeSettings);

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
	FRandomStream Seed;


	void GenerateTreeSkeleton(const FTreeSettings& currentSettings, FTreeSkeleton& currentTreeSkeleton);
	void GrowTreeSkeleton(const FTreeSettings& currentSettings, FTreeSkeleton& currentTreeSkeleton, int maxIterations);
	void FinalizeTreeSkeleton(FTreeSkeleton& currentTreeSkeleton);
	void IncrementBranchSizeAndPropagate(FTreeSkeleton& currentTreeSkeleton, FTreeBranch& currentBranch, int size = 2);
	
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
	UMaterialInterface* BarkMaterial;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* LeafMaterial;

	void GenerateNextBranchMesh(const FTreeSettings& currentSettings, int currentTreeIdx, const FTreeBranch& currentBranch,int attachOffset = -1);
	void GenerateNextBranchRing(const FTreeSettings& currentSettings, const FTreeBranch& currentBranch, const FVector& upVector, float minRingRadius, int prevRingOffset, int currentRingOffset);
	void GenerateBranchCap(const FTreeSettings& currentSettings, const FVector& position, int capStartOffset, bool copyRing);

};
