#pragma once
#include "CoreMinimal.h"
#include "DllExample.generated.h"

UCLASS(BlueprintType)
class UDllCalculator : public UObject
{
	GENERATED_BODY()
	using PCalcFun = int(*)(int,int);
public:
	UFUNCTION(BlueprintCallable)
	bool Load(const FString& InPath)
	{
		if (!FPaths::FileExists(InPath))
		{
			UE_LOG(LogTemp,Error,TEXT("Load Dll failed, Reason: Dll in Path \"%s\" is not exist!!!"),*InPath);
			return false;
		}

		// load dll 
		m_DllHandle = FPlatformProcess::GetDllHandle(*InPath);
		if (m_DllHandle == nullptr)
		{
			UE_LOG(LogTemp,Error,TEXT("Load Dll failed, Reason: Unknown error, Path: %s"),*InPath);
			return false;
		}

		// load function
		m_Add = (PCalcFun)FPlatformProcess::GetDllExport(m_DllHandle,TEXT("Add"));
		if (m_Add == nullptr)
		{
			UE_LOG(LogTemp,Error,TEXT("Load Dll function Add failed, Reason: Unknown error, Path: %s"),*InPath);
			return false;
		}
		m_Sub = (PCalcFun)FPlatformProcess::GetDllExport(m_DllHandle,TEXT("Sub"));
		if (m_Sub == nullptr)
		{
			UE_LOG(LogTemp,Error,TEXT("Load Dll function Sub failed, Reason: Unknown error, Path: %s"),*InPath);
			return false;
		}
		m_Mul = (PCalcFun)FPlatformProcess::GetDllExport(m_DllHandle,TEXT("Mul"));
		if (m_Mul == nullptr)
		{
			UE_LOG(LogTemp,Error,TEXT("Load Dll function Mul failed, Reason: Unknown error, Path: %s"),*InPath);
			return false;
		}
		m_Div = (PCalcFun)FPlatformProcess::GetDllExport(m_DllHandle,TEXT("Div"));
		if (m_Div == nullptr)
		{
			UE_LOG(LogTemp,Error,TEXT("Load Dll function Div failed, Reason: Unknown error, Path: %s"),*InPath);
			return false;
		}

		// save path 
		m_DllPath = InPath;
		
		return true;
	}

	UFUNCTION(BlueprintCallable)
	void Free()
	{
		if (m_DllHandle == nullptr) return;

		FPlatformProcess::FreeDllHandle(m_DllHandle);
		
		m_DllHandle = nullptr;
		m_Add = m_Sub = m_Mul = m_Div = nullptr;
	}

	UFUNCTION(BlueprintCallable)
	void Reload()
	{
		Free();
		Load(m_DllPath);
	}
	
	UFUNCTION(BlueprintCallable)
	bool IsValid() const { return m_DllHandle != nullptr; }

	UFUNCTION(BlueprintCallable)
	int Add(int a, int b) const { return (*m_Add)(a,b); }
	UFUNCTION(BlueprintCallable)
	int Sub(int a, int b) const { return (*m_Sub)(a,b); }
	UFUNCTION(BlueprintCallable)
	int Mul(int a, int b) const { return (*m_Mul)(a,b); }
	UFUNCTION(BlueprintCallable)
	int Div(int a, int b) const { return (*m_Div)(a,b); }
	
private:
	FString		m_DllPath;
	void*		m_DllHandle;

	PCalcFun	m_Add;	// +
	PCalcFun	m_Sub;	// -
	PCalcFun	m_Mul;	// *
	PCalcFun	m_Div;	// /
};