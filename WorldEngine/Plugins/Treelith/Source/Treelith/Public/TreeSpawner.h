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
	void GenerateTreeSkeletonAsync(const TArray<FTreeSettings>& treeSettings);

private:

	UProceduralMeshComponent* TreeMesh;
	TArray<FTreeSettings> TreeSettings;
	FRandomStream Seed;

	//Tree Skeleton Generation

	void GenerateTreeSkeleton(const FTreeSettings& currentSettings, FTreeSkeleton& currentTreeSkeleton);
	void GrowTreeSkeleton(const FTreeSettings& currentSettings, FTreeSkeleton& currentTreeSkeleton, int maxIterations);
	
	//Tree Mesh generation


	void Debug();

};
