#include "TreeSpawner.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "TreeFunctionRegistry.h"
#include "Async/Async.h"

// Sets default values
ATreeSpawner::ATreeSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TreeMesh = CreateDefaultSubobject<UProceduralMeshComponent>("ProceduralTreeMesh");
	TreeMesh->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void ATreeSpawner::BeginPlay()
{
	Super::BeginPlay();	
}

void ATreeSpawner::GenerateAsync(const TArray<FTreeSettings>& treeSettings)
{
	TreeSettings = treeSettings;
	Seed.Initialize(0);
	
	AsyncTask(ENamedThreads::AnyHiPriThreadNormalTask, [this]()
		{
			for (const FTreeSettings& currentSettings : TreeSettings)
			{
				GenerateTreeSkeleton(currentSettings);
			}
		}
	);
}

void ATreeSpawner::GenerateTreeSkeleton(const FTreeSettings& currentSettings)
{

	auto& randomizeBranchLeaf = UTreeFunctionRegistry::GetTreeRandomizationFunction(currentSettings.RandomType);
	randomizeBranchLeaf(Seed, Leaves, currentSettings.BranchDestinationAmount);
}



