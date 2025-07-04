// Fill out your copyright notice in the Description page of Project Settings.


#include "TreeFunctionRegistry.h"

TMap<ETreeRandomType, TFunction<void(FRandomStream&, TArray<FTreeBranchLeaf>&, int)>> UTreeFunctionRegistry::TreeRandomizationRegistry = TMap<ETreeRandomType, TFunction<void(FRandomStream&, TArray<FTreeBranchLeaf>&, int)>>{};

void UTreeFunctionRegistry::InitializeRegistries()
{
	InitializeTreeRandomizationRegistry();
}

TFunction<void(FRandomStream&, TArray<FTreeBranchLeaf>&, int)>& UTreeFunctionRegistry::GetTreeRandomizationFunction(ETreeRandomType treeType)
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
	TreeRandomizationRegistry.Add(ETreeRandomType::DEFAULT, [](FRandomStream& rand, TArray<FTreeBranchLeaf>& leaves, int numberOfLeaves)
		{
			leaves.Reserve(numberOfLeaves);
			for (int i{}; i < numberOfLeaves; ++i)
			{
				leaves.Add(FTreeBranchLeaf{ FVector{ rand.FRandRange(-200.f, 200.f), rand.FRandRange(-200.f, 200.f), rand.FRandRange(-150.f, 600.f) }});
			}
		}
	);
}
