// Fill out your copyright notice in the Description page of Project Settings.

#include "ShaderTools.h"
#include "GlobalShader.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Shader/FSimpleCs.h"

void UShaderTools::DrawSimpleCs(UTextureRenderTarget2D* InRenderTarget)
{
	check(IsInGameThread());

	if (InRenderTarget == nullptr)
	{
		UE_LOG(LogTemp,Error,TEXT("%s: InRenderTarget is null!!!"),TEXT(__FUNCTION__));
		return;
	}

	FTextureRenderTargetResource* TextureRenderTargetResource = InRenderTarget->GameThread_GetRenderTargetResource();
	ERHIFeatureLevel::Type FeatureLevel = GWorld->Scene->GetFeatureLevel();

	ENQUEUE_RENDER_COMMAND(MyCmd)
    (
        [TextureRenderTargetResource, FeatureLevel](FRHICommandListImmediate& RHICmdList)
        {
        	check(IsInRenderingThread());

        	// get RHI texture 
		    FTexture2DRHIRef RenderTargetTexture = TextureRenderTargetResource->GetRenderTargetTexture();

        	// compute group num 
        	const uint32 GGroupSize = 32;
		    uint32 GroupSizeX = FMath::DivideAndRoundUp((uint32)RenderTargetTexture->GetSizeX(), GGroupSize);
		    uint32 GroupSizeY = FMath::DivideAndRoundUp((uint32)RenderTargetTexture->GetSizeY(), GGroupSize);

        	// get shader 
		    TShaderMapRef<FSimpleCs> ComputeShader(GetGlobalShaderMap(FeatureLevel));
		    RHICmdList.SetComputeShader(ComputeShader.GetComputeShader());

		    // create a temp resource
		    FRHIResourceCreateInfo CreateInfo;
		    FTexture2DRHIRef GSurfaceTexture2D = RHICreateTexture2D(
		    	RenderTargetTexture->GetSizeX(),
		    	RenderTargetTexture->GetSizeY(),
		    	PF_A32B32G32R32F,
		    	1, 1,
		    	TexCreate_ShaderResource | TexCreate_UAV,
		    	CreateInfo);

        	// create UAV  
		    FUnorderedAccessViewRHIRef GUAV = RHICreateUnorderedAccessView(GSurfaceTexture2D);

        	// dispatch compute shader 
		    ComputeShader->SetParameters(RHICmdList, RenderTargetTexture, GUAV);
		    DispatchComputeShader(RHICmdList, ComputeShader, GroupSizeX, GroupSizeY, 1);
		    ComputeShader->UnsetParameters(RHICmdList, GUAV);

        	// copy to render target 
		    FRHICopyTextureInfo CopyInfo;
		    RHICmdList.CopyTexture(GSurfaceTexture2D, RenderTargetTexture, CopyInfo);
        }
    );
}
