#pragma once
#include "CoreMinimal.h"
#include "GlobalShader.h"
#include "ShaderParameterUtils.h"

// !!!Tip: don't forget add shader path in FMiscToolsModule::StartupModule()
// !!!Tip: LoadingPhase must PostConfigInit

// here is the cpp part of a compute shader 
class FSimpleCs : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FSimpleCs, Global)
public:
	FSimpleCs() {}
	FSimpleCs(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		:FGlobalShader(Initializer)
	{
		// bind parameter in shader 
		RWOut.Bind(Initializer.ParameterMap,TEXT("RWOut"));
	}

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

	void SetParameters(
        FRHICommandList& RHICmdList,
        FTexture2DRHIRef &InOutputSurfaceValue,
        FUnorderedAccessViewRHIRef& UAV
    )
	{
		FRHIComputeShader* ShaderRHI = RHICmdList.GetBoundComputeShader();

		// barrier for UAV 
		RHICmdList.TransitionResource(
			EResourceTransitionAccess::ERWBarrier,
			EResourceTransitionPipeline::EComputeToCompute, UAV);
		RWOut.SetTexture(RHICmdList, ShaderRHI, InOutputSurfaceValue, UAV);
	}

	void UnsetParameters(FRHICommandList& RHICmdList, FUnorderedAccessViewRHIRef& UAV)
	{
		// barrier for copy  
		RHICmdList.TransitionResource(
			EResourceTransitionAccess::EReadable,
			EResourceTransitionPipeline::EComputeToCompute, UAV);
		RWOut.UnsetUAV(RHICmdList, RHICmdList.GetBoundComputeShader());
	}

private:
	// used to bind cs out put texture  
	LAYOUT_FIELD(FRWShaderParameter, RWOut);
};

// here bind this class to .usf, the path is virtual path witch seated in ShaderTools.cpp 
IMPLEMENT_SHADER_TYPE(, FSimpleCs, TEXT("/Plugin/MiscTools/Private/SimpleCs.usf"), TEXT("Main"), SF_Compute);

