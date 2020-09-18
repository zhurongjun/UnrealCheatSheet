#pragma once
#include "CoreMinimal.h"
#include "GlobalShader.h"
#include "ShaderParameterUtils.h"
#include "ShaderParameterStruct.h"

class FStaticNoiseCs : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FStaticNoiseCs, Global)
	SHADER_USE_PARAMETER_STRUCT(FStaticNoiseCs, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER(float, Seed)
		SHADER_PARAMETER_UAV(RWTexture2D<float4>, RWOut)
	END_SHADER_PARAMETER_STRUCT()
public:
	static bool ShouldCompilePermutation(
        const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	static void ModifyCompilationEnvironment(
        const FGlobalShaderPermutationParameters& Parameters,
        FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters,OutEnvironment);
	}
};

IMPLEMENT_SHADER_TYPE(, FStaticNoiseCs, TEXT("/Plugin/MiscTools/Private/StaticNoiseCs.usf"), TEXT("Main"), SF_Compute);
