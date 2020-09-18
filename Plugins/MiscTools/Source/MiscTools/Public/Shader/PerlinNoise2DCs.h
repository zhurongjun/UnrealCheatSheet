#pragma once
#include "CoreMinimal.h"
#include "GlobalShader.h"
#include "ShaderParameterUtils.h"
#include "ShaderParameterStruct.h"

class FPerlinNoise2DCs : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FPerlinNoise2DCs, Global)
        SHADER_USE_PARAMETER_STRUCT(FPerlinNoise2DCs, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
        SHADER_PARAMETER_UAV(RWTexture2D<float4>, RWOut)
        SHADER_PARAMETER_SRV(Texture2D<float4>, RGrid)
        SHADER_PARAMETER(FIntPoint, TexSize)
        SHADER_PARAMETER(int, GridSize)
        SHADER_PARAMETER(int, MinIterateSize)
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

IMPLEMENT_SHADER_TYPE(, FPerlinNoise2DCs, TEXT("/Plugin/MiscTools/Private/PerlinNoise2DCs.usf"), TEXT("Main"), SF_Compute);
