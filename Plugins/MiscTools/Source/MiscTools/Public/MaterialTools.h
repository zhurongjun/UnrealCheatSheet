// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MaterialTools.generated.h"

/**
 * Tools for material 
 */
UCLASS()
class MISCTOOLS_API UMaterialTools : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static void GetMaterialParams(UMaterial* InMaterial);

	
	
};
