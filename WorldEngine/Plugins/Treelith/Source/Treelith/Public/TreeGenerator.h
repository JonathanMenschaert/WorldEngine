// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TreeGenerator.generated.h"


UENUM(BlueprintType, Blueprintable)
enum UTreeType : uint8
{

	OAK
};

USTRUCT(BlueprintType, Blueprintable)
struct TREELITH_API FTreeData
{
	GENERATED_BODY()



	UPROPERTY(BlueprintReadWrite)
	int NumSides{ 4 };
	

};

USTRUCT(BlueprintType, Blueprintable)
struct TREELITH_API FTreeGeneratorData
{
	GENERATED_BODY()
};

class UProceduralMeshComponent;

UCLASS(BlueprintType, Blueprintable)
class TREELITH_API ATreeGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATreeGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:

	UProceduralMeshComponent* TreeMesh;
};
