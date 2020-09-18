// Fill out your copyright notice in the Description page of Project Settings.

#include "ShaderTools.h"
#include "GlobalShader.h"
#include "RenderGraphUtils.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Shader/FSimpleCs.h"
#include "Shader/GoldNoiseCs.h"
#include "Shader/PerlinNoise2DCs.h"

template<typename ShaderType, uint32 GroupSize = 32>
void DispatchComputeShader(
	FTextureRenderTargetResource* InRT ,
	ERHIFeatureLevel::Type InFeatureLevel ,
	FRHICommandListImmediate& RHICmdList ,
	typename ShaderType::FParameters InParam = ShaderType::FParameters())
{
	check(IsInRenderingThread());

	// get RHI texture 
	FTexture2DRHIRef RenderTargetTexture = InRT->GetRenderTargetTexture();

	// compute group num 
	uint32 GroupSizeX = FMath::DivideAndRoundUp((uint32)RenderTargetTexture->GetSizeX(), GroupSize);
	uint32 GroupSizeY = FMath::DivideAndRoundUp((uint32)RenderTargetTexture->GetSizeY(), GroupSize);

	// get shader 
	TShaderMapRef<ShaderType> ComputeShader(GetGlobalShaderMap(InFeatureLevel));
	RHICmdList.SetComputeShader(ComputeShader.GetComputeShader());

	// create a temp resource
	FRHIResourceCreateInfo CreateInfo;
	CreateInfo.ClearValueBinding = FClearValueBinding::Green;
	FTexture2DRHIRef GSurfaceTexture2D = RHICreateTexture2D(
        RenderTargetTexture->GetSizeX(),
        RenderTargetTexture->GetSizeY(),
        RenderTargetTexture->GetFormat(),
        1, 1,
        TexCreate_ShaderResource | TexCreate_UAV,
        CreateInfo);

	// create UAV 
	FUnorderedAccessViewRHIRef GUAV = RHICreateUnorderedAccessView(GSurfaceTexture2D);

	// dispatch compute shader
	RHICmdList.TransitionResource(
		EResourceTransitionAccess::ERWBarrier,
		EResourceTransitionPipeline::EComputeToCompute,GUAV);

	InParam.RWOut = GUAV;
	FComputeShaderUtils::Dispatch(RHICmdList, ComputeShader, InParam,
                                  FIntVector(GroupSizeX, GroupSizeY, 1));

	RHICmdList.TransitionResource(
		EResourceTransitionAccess::EReadable,
		EResourceTransitionPipeline::EComputeToCompute,
		GUAV
	);
	
	// copy to render target 
	FRHICopyTextureInfo CopyInfo;
	RHICmdList.CopyTexture(GSurfaceTexture2D, RenderTargetTexture, CopyInfo);
}

void UShaderTools::DrawSimpleCs(UTextureRenderTarget2D* InRenderTarget)
{
	check(IsInGameThread());

	if (InRenderTarget == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: InRenderTarget is null!!!"), TEXT(__FUNCTION__));
		return;
	}

	FTextureRenderTargetResource* TextureRenderTargetResource = InRenderTarget->GameThread_GetRenderTargetResource();
	ERHIFeatureLevel::Type FeatureLevel = GWorld->Scene->GetFeatureLevel();

	ENQUEUE_RENDER_COMMAND(MyCmd)
	(
		[TextureRenderTargetResource, FeatureLevel](FRHICommandListImmediate& RHICmdList)
		{
			DispatchComputeShader<FSimpleCs>(
				TextureRenderTargetResource ,
				FeatureLevel ,
				RHICmdList);
		}
	);
}

void UShaderTools::DrawGoldNoise(UTextureRenderTarget2D* InRenderTarget, float InSeed)
{
	check(IsInGameThread());

	if (InRenderTarget == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: InRenderTarget is null!!!"), TEXT(__FUNCTION__));
		return;
	}

	FTextureRenderTargetResource* TextureRenderTargetResource = InRenderTarget->GameThread_GetRenderTargetResource();
	ERHIFeatureLevel::Type FeatureLevel = GWorld->Scene->GetFeatureLevel();

	ENQUEUE_RENDER_COMMAND(MyCmd)
    (
        [=](FRHICommandListImmediate& RHICmdList)
        {
        	FGoldNoiseCs::FParameters Params;
        	Params.Seed = InSeed;
            DispatchComputeShader<FGoldNoiseCs, 8>(
                TextureRenderTargetResource ,
                FeatureLevel ,
                RHICmdList ,
                Params);
        }
    );
}

void UShaderTools::DrawPerlinNoise(UTextureRenderTarget2D* InRenderTarget, UTextureRenderTarget2D* InGrid, int MaxGrid,int MinGrid)
{
	check(IsInGameThread());

	if (InRenderTarget == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: InRenderTarget is null!!!"), TEXT(__FUNCTION__));
		return;
	}

	FTextureRenderTargetResource* TextureRenderTargetResource = InRenderTarget->GameThread_GetRenderTargetResource();
	FTextureRenderTargetResource* GridRes = InGrid->GameThread_GetRenderTargetResource();
	ERHIFeatureLevel::Type FeatureLevel = GWorld->Scene->GetFeatureLevel();

	ENQUEUE_RENDER_COMMAND(MyCmd)
    (
        [=](FRHICommandListImmediate& RHICmdList)
        {
            FPerlinNoise2DCs::FParameters Params;
			Params.RGrid = RHICreateShaderResourceView(GridRes->GetRenderTargetTexture(), 0);
        	Params.GridSize = MaxGrid;
        	Params.TexSize = TextureRenderTargetResource->GetSizeXY();
        	Params.MinIterateSize = MinGrid;
            DispatchComputeShader<FPerlinNoise2DCs, 8>(
                TextureRenderTargetResource ,
                FeatureLevel ,
                RHICmdList ,
                Params);
        }
    );
}
