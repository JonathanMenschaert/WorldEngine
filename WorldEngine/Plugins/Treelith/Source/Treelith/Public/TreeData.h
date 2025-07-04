// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TreeData.generated.h"

UENUM(BlueprintType, Blueprintable)
enum class ETreeType : uint8
{
	DEFAULT,
	OAK
};

USTRUCT(BlueprintType, Blueprintable)
struct TREELITH_API FTreeSettings
{
	GENERATED_BODY()



	UPROPERTY(BlueprintReadWrite)
	int NumSides{ 4 };


};

USTRUCT()
struct TREELITH_API FTreeBranchDestination
{
	GENERATED_BODY()

	/*FTreeBranchDestination()
	{
	}

	FTreeBranchDestination(const FVector& position)
	{
		Position = position;
		IsReached = false;
	}*/

	FVector Position{};
	bool IsReached{ false };
};


USTRUCT()
struct TREELITH_API FTreeBranch
{
	GENERATED_BODY()

	bool IsReached{ false };
};
