// Fill out your copyright notice in the Description page of Project Settings.


#include "TreeFunctionRegistry.h"
#include "TreeSpawnerFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Treelith.h"


TMap<ETreeRandomType, TFunction<void(FRandomStream&, TArray<FTreeBranchDestination>&, const FBoundary3&, const FVector&, int)>> UTreeFunctionRegistry::TreeRandomizationRegistry{};


void UTreeFunctionRegistry::InitializeRegistries()
{
	InitializeTreeRandomizationRegistry();
}

TFunction<void(FRandomStream&, TArray<FTreeBranchDestination>&, const FBoundary3&, const FVector&, int)>& UTreeFunctionRegistry::GetTreeRandomizationFunction(ETreeRandomType treeType)
{
	auto function{ TreeRandomizationRegistry.Find(treeType) };
	if (!function)
	{
		function = TreeRandomizationRegistry.Find(ETreeRandomType::DEFAULT);
	}
	return *function;
}

void UTreeFunctionRegistry::UninitalizeRegistries()
{
	TreeRandomizationRegistry.Empty();
}

void UTreeFunctionRegistry::InitializeTreeRandomizationRegistry()
{
	TreeRandomizationRegistry.Add(ETreeRandomType::DEFAULT, [](FRandomStream& rand, TArray<FTreeBranchDestination>& leaves, const FBoundary3& randBoundaries, const FVector& position, int numberOfLeaves)
		{
			leaves.Empty(numberOfLeaves);
			for (int i{}; i < numberOfLeaves; ++i)
			{
				FVector leafPos{ FVector{ rand.FRandRange(randBoundaries.MinMaxX.X, randBoundaries.MinMaxX.Y), rand.FRandRange(randBoundaries.MinMaxY.X, randBoundaries.MinMaxY.Y), rand.FRandRange(randBoundaries.MinMaxZ.X, randBoundaries.MinMaxZ.Y)} + position };
				leaves.Add(FTreeBranchDestination{leafPos});
			}
		}
	);
}
