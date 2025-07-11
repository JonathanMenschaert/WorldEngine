// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TreeSpawnerFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class TREELITH_API UTreeSpawnerFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static FVector GetRandomVectorNormalized(FRandomStream& seed);
	
};
