// Fill out your copyright notice in the Description page of Project Settings.

#include "ShaderTools.h"
#include "GlobalShader.h"
#include "RenderGraphUtils.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Shader/FSimpleCs.h"

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
			CreateInfo.ClearValueBinding == FClearValueBinding::Green;
			FTexture2DRHIRef GSurfaceTexture2D = RHICreateTexture2D(
				RenderTargetTexture->GetSizeX(),
				RenderTargetTexture->GetSizeY(),
				PF_FloatRGBA,
				1, 1,
				TexCreate_ShaderResource | TexCreate_UAV,
				CreateInfo);

			// create UAV 
			FUnorderedAccessViewRHIRef GUAV = RHICreateUnorderedAccessView(GSurfaceTexture2D);
			RHICmdList.ClearUAVFloat(GUAV, FVector4(0, 1, 0, 1));

			// dispatch compute shader
			FSimpleCs::FParameters Param;
			Param.RWOut = GUAV;
			FComputeShaderUtils::Dispatch(RHICmdList, ComputeShader, Param,
										  FIntVector(GroupSizeX, GroupSizeY, 1));

			// copy to render target 
			FRHICopyTextureInfo CopyInfo;
			RHICmdList.CopyTexture(GSurfaceTexture2D, RenderTargetTexture, CopyInfo);
		}
	);
	
}
