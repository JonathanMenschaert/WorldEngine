// Fill out your copyright notice in the Description page of Project Settings.


#include "TreeFunctionRegistry.h"

TMap<ETreeBranchDestinationType, TFunction<void(FRandomStream&, TArray<FTreeBranchDestination>&, int)>> UTreeFunctionRegistry::TreeRandomizationRegistry = TMap<ETreeBranchDestinationType, TFunction<void(FRandomStream&, TArray<FTreeBranchDestination>&, int)>>{};

void UTreeFunctionRegistry::InitializeRegistries()
{
	InitializeTreeRandomizationRegistry();
}

TFunction<void(FRandomStream&, TArray<FTreeBranchDestination>&, int)>& UTreeFunctionRegistry::GetTreeRandomizationFunction(ETreeBranchDestinationType treeType)
{
	auto function{ TreeRandomizationRegistry.Find(treeType) };
	if (!function)
	{
		function = TreeRandomizationRegistry.Find(ETreeBranchDestinationType::DEFAULT);
	}
	return *function;
}

void UTreeFunctionRegistry::UninitalizeRegistries()
{
	TreeRandomizationRegistry.Empty();
}

void UTreeFunctionRegistry::InitializeTreeRandomizationRegistry()
{
	TreeRandomizationRegistry.Add(ETreeBranchDestinationType::DEFAULT, [](FRandomStream& rand, TArray<FTreeBranchDestination>& leaves, int numberOfLeaves)
		{
			leaves.Reserve(numberOfLeaves);
			for (int i{}; i < numberOfLeaves; ++i)
			{
				leaves.Add(FTreeBranchDestination{ FVector{ rand.FRandRange(-200.f, 200.f), rand.FRandRange(-200.f, 200.f), rand.FRandRange(-150.f, 600.f) }});
			}
		}
	);
}
