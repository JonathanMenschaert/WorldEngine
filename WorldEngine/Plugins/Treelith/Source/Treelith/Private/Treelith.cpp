// Copyright Epic Games, Inc. All Rights Reserved.

#include "Treelith.h"
#include "TreeFunctionRegistry.h"

#define LOCTEXT_NAMESPACE "FTreelithModule"

DEFINE_LOG_CATEGORY(LogTreelith)

void FTreelithModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	/*registry = NewObject<UTreeFunctionRegistry>();
	registry->InitializeRegistries();*/

	UTreeFunctionRegistry::InitializeRegistries();
}

void FTreelithModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTreelithModule, Treelith)