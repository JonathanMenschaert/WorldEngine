// Fill out your copyright notice in the Description page of Project Settings.


#include "TreeSpawnerFunctionLibrary.h"

FVector UTreeSpawnerFunctionLibrary::GetRandomVectorNormalized(FRandomStream& seed)
{
	FVector randVector{ seed.FRandRange(-1.f, 1.f), seed.FRandRange(-1.f, 1.f), seed.FRandRange(-1.f, 1.f) };
	randVector.Normalize();
	return randVector;
}
