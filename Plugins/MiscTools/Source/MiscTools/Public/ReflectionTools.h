// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ReflectionTools.generated.h"

/**
 * some tools of reflection
 * help functions:
 * 		UObjectGlobals.h
 */
UCLASS()
class MISCTOOLS_API UReflectionTools : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// get derived classes, cpp and loaded blueprint 
	UFUNCTION(BlueprintCallable)
	static void GetDerivedClasses(UClass* InClass, TArray<UClass*>& OutClasses, bool bRecursive = true);

	// find classes by name, cpp and loaded blueprint 
	UFUNCTION(BlueprintCallable)
	static UClass* FindClassByName(const FString& InName,bool bExactClass = false);

	// find enum by name, cpp and loaded blueprint 
	UFUNCTION(BlueprintCallable)
	static UEnum* FindEnumByName(const FString& InName, bool bExactClass = false);

	// find struct by name, cpp and loaded blueprint 
	UFUNCTION(BlueprintCallable)
	static UScriptStruct* FindStructByName(const FString& InName, bool bExactClass = false);
};
