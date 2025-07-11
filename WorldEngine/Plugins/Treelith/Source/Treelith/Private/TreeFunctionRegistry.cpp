// Fill out your copyright notice in the Description page of Project Settings.


#include "TreeFunctionRegistry.h"
#include "TreeSpawnerFunctionLibrary.h"


TMap<ETreeRandomType, TFunction<void(FRandomStream&, TArray<FTreeBranchDestination>&, const FVector&, FVector&, int)>> UTreeFunctionRegistry::TreeRandomizationRegistry{};
TMap<ELeafType, TFunction<FQuat(FRandomStream&, const FVector2D&, const FVector2D&, const FVector2D&, const FVector&)>> UTreeFunctionRegistry::LeafRandomizationRegistry{};


void UTreeFunctionRegistry::InitializeRegistries()
{
	InitializeTreeRandomizationRegistry();
	InitializeLeafRandomizationRegistry();
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

TFunction<FQuat(FRandomStream&, const FVector2D&, const FVector2D&, const FVector2D&, const FVector&)>& UTreeFunctionRegistry::GetLeafRandomizationFunction(ELeafType leafType)
{
	auto function{ LeafRandomizationRegistry.Find(leafType) };
	if (!function)
	{
		function = LeafRandomizationRegistry.Find(ELeafType::NONE);
	}
	return *function;
}

void UTreeFunctionRegistry::UninitalizeRegistries()
{
	TreeRandomizationRegistry.Empty();
	LeafRandomizationRegistry.Empty();
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

void UTreeFunctionRegistry::InitializeLeafRandomizationRegistry()
{
	LeafRandomizationRegistry.Add(ELeafType::NONE, [](FRandomStream& seed, const FVector2D& minMaxYaw, const FVector2D& minMaxPitch, const FVector2D& minMaxRoll, const FVector& branchUpVector)
		{
			return FQuat{};
		}
	);
	LeafRandomizationRegistry.Add(ELeafType::OAK, [](FRandomStream& seed, const FVector2D& minMaxYaw, const FVector2D& minMaxPitch, const FVector2D& minMaxRoll, const FVector& branchUpVector)
		{

			FVector branchRight{ FVector::CrossProduct(FVector::UpVector, branchUpVector) };
			branchRight.Normalize();
			//FVector randUpVector{ UTreeSpawnerFunctionLibrary::GetRandomVectorNormalized(seed) };
			

			float rollRotValue{ static_cast<float>(seed.FRandRange(minMaxRoll.X, minMaxRoll.Y)) };			
			float yawDiffValue{ static_cast<float>(FQuat::FindBetweenNormals(branchRight, FVector::RightVector).GetAngle())};
			float yawRotValue{ static_cast<float>(seed.FRandRange(minMaxYaw.X, minMaxYaw.Y)) };
			float pitchRotValue{ static_cast<float>(seed.FRandRange(minMaxPitch.X, minMaxPitch.Y)) };

			FQuat rotationQuat{ FVector::UpVector, yawRotValue };
			FVector rotationAxis{ rotationQuat.RotateVector(branchRight)};

			FQuat leafRollRotator{ FVector::ForwardVector, rollRotValue };
			FQuat leafYawRotator{ FVector::UpVector, yawRotValue};
			FQuat leafPitchRotator{ rotationAxis, yawRotValue };

			//FVector rollRight{ FVector::CrossProduct(FVector::UpVector, rollVector) };

			//FVector randPitchVector{ Seed.FRandRange(-1.f, 1.f), Seed.FRandRange(-1.f, 1.f), Seed.FRandRange(-1.f, 1.f) };
			//randPitchVector.Normalize();

			////Move minmax...rotation to the leafcard data asset
			//float anglePitchBetween{ static_cast<float>(FQuat::FindBetweenNormals(FVector::UpVector, randPitchVector).GetAngle()) };
			//anglePitchBetween = FMath::Clamp(anglePitchBetween, leafSettings.MinMaxPitchRotation.X, leafSettings.MinMaxPitchRotation.Y);
			//FVector pitchRotAxis{ FVector::CrossProduct(FVector::UpVector, randPitchVector) };
			//FQuat pitchRotator{ pitchRotAxis, anglePitchBetween };

			//FVector randRollVector{ Seed.FRandRange(-1.f, 1.f), Seed.FRandRange(-1.f, 1.f), Seed.FRandRange(-1.f, 1.f) };
			//randRollVector.Normalize();

			//float angleRollBetween{ static_cast<float>(FQuat::FindBetweenNormals(FVector::UpVector, randRollVector).GetAngle()) };
			//angleRollBetween = FMath::Clamp(angleRollBetween, leafSettings.MinMaxRollRotation.X, leafSettings.MinMaxRollRotation.Y);
			////FVector rollRotAxis{ FVector::CrossProduct(FVector::UpVector, randRollVector) };
			//FQuat rollRotator{ FVector::ForwardVector, angleRollBetween };


			return leafYawRotator;
		}
	);
}
