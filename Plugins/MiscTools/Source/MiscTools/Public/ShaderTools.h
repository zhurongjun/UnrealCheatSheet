// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ShaderTools.generated.h"

/**
 * here is some tools for use shader, some function is just a example or template 
 */
UCLASS()
class MISCTOOLS_API UShaderTools : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// here just a example for use shader 
	UFUNCTION(BlueprintCallable)
	static void DrawSimpleCs(UTextureRenderTarget2D* InRenderTarget);

	// draw static noise
	UFUNCTION(BlueprintCallable)
	static void DrawStaticNoise(UTextureRenderTarget2D* InRenderTarget,float InSeed);
};
