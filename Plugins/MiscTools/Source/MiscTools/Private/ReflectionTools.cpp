// Fill out your copyright notice in the Description page of Project Settings.
#include "ReflectionTools.h"
#include "UObject/Package.h"
#include "UObject/UObjectHash.h"

void UReflectionTools::GetDerivedClasses(UClass* InClass, TArray<UClass*>& OutClasses, bool bRecursive)
{
	::GetDerivedClasses(InClass, OutClasses, bRecursive);
}

UClass* UReflectionTools::FindClassByName(const FString& InName, bool bExactClass)
{
	return FindObject<UClass>(ANY_PACKAGE, *InName, bExactClass);
}

UEnum* UReflectionTools::FindEnumByName(const FString& InName, bool bExactClass)
{
	return FindObject<UEnum>(ANY_PACKAGE, *InName, bExactClass);
}

UScriptStruct* UReflectionTools::FindStructByName(const FString& InName, bool bExactClass)
{
	return FindObject<UScriptStruct>(ANY_PACKAGE, *InName, bExactClass);
}
