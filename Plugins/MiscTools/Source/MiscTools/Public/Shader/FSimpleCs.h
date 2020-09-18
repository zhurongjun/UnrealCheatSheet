#pragma once
#include "CoreMinimal.h"
#include "GlobalShader.h"
#include "ShaderParameterUtils.h"
#include "ShaderParameterStruct.h"

// !!!Tip: don't forget add shader path in FMiscToolsModule::StartupModule()
// !!!Tip: LoadingPhase must PostConfigInit

// here is the cpp part of a compute shader 
class FSimpleCs : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FSimpleCs, Global)
	SHADER_USE_PARAMETER_STRUCT(FSimpleCs, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
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

// here bind this class to .usf, the path is virtual path witch seated in ShaderTools.cpp 
IMPLEMENT_SHADER_TYPE(, FSimpleCs, TEXT("/Plugin/MiscTools/Private/SimpleCs.usf"), TEXT("Main"), SF_Compute);

