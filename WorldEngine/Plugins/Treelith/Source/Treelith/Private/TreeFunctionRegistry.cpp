// Fill out your copyright notice in the Description page of Project Settings.


#include "TreeFunctionRegistry.h"

TMap<ETreeRandomType, TFunction<void(FRandomStream&, TArray<FTreeBranchDestination>&, const FVector&, FVector&, int)>> UTreeFunctionRegistry::TreeRandomizationRegistry{};


void UTreeFunctionRegistry::InitializeRegistries()
{
	InitializeTreeRandomizationRegistry();
}

TFunction<void(FRandomStream&, TArray<FTreeBranchDestination>&, const FVector&, FVector&, int)>& UTreeFunctionRegistry::GetTreeRandomizationFunction(ETreeRandomType treeType)
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
	TreeRandomizationRegistry.Add(ETreeRandomType::DEFAULT, [](FRandomStream& rand, TArray<FTreeBranchDestination>& leaves, const FVector& position, FVector& rootDirection, int numberOfLeaves)
		{
			leaves.Empty(numberOfLeaves);
			for (int i{}; i < numberOfLeaves; ++i)
			{
				FVector leafPos{ FVector{ rand.FRandRange(-200.f, 200.f), rand.FRandRange(-200.f, 200.f), rand.FRandRange(150.f, 600.f)} + position };
				leaves.Add(FTreeBranchDestination{leafPos});
				rootDirection += (leafPos - position);
			}
			rootDirection.Normalize();
		}
	);
}