// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TreeData.h"
#include "Math/RandomStream.h"
#include "TreeFunctionRegistry.generated.h"

/**
 * 
 */
UCLASS()
class TREELITH_API UTreeFunctionRegistry : public UObject
{
	GENERATED_BODY()

public: 
	static void InitializeRegistries();
	static TFunction<void(FRandomStream&, TArray<FTreeBranchDestination>&, const FVector&, FVector&, int)>& GetTreeRandomizationFunction(ETreeRandomType treeType);

	static void UninitalizeRegistries();

private:

	static TMap<ETreeRandomType, TFunction<void(FRandomStream&, TArray<FTreeBranchDestination>&, const FVector&, FVector&, int)>> TreeRandomizationRegistry;

	static void InitializeTreeRandomizationRegistry();
};
