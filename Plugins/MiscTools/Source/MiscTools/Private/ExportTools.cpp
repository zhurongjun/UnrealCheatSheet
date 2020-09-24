#include "ExportTools.h"

#include "CubemapUnwrapUtils.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereReflectionCaptureComponent.h"
#include "Engine/MapBuildDataRegistry.h"
#include "Engine/TextureCube.h"
#include "Rendering/SkeletalMeshModel.h"

namespace Fuko
{
	float LODScreenSizeToDistance(float InSize, float InBoundingSphereRadius, float InHalfFOV, float InScreenWidth,
		float InScreenHeight)
	{
		const FPerspectiveMatrix ProjMatrix(InHalfFOV, InScreenWidth, InScreenHeight, 1.0f);
		return ComputeBoundsDrawDistance(InSize,InBoundingSphereRadius,ProjMatrix);
	}

	TArray<MeshData> ExportStaticMesh(const UStaticMesh* InMesh)
	{
		TArray<MeshData> OutData;

		// Get lod num
		if(!InMesh) return OutData;
		int32 LODNum = InMesh->GetNumLODs();
		if(LODNum == 0) return OutData;
		OutData.SetNum(LODNum);

		// export all LOD data 
		for (int32 i = 0; i < LODNum; ++i)
		{
			const FStaticMeshLODResources& RenderMesh = InMesh->GetLODForExport(i);
			MeshData& OutDataNowLOD = OutData[i];

			// get LOD screen size
			OutDataNowLOD.LODScreenSize = InMesh->GetSourceModel(i).ScreenSize.Default;
			
			// Invalid mesh data
			if (RenderMesh.VertexBuffers.StaticMeshVertexBuffer.GetNumVertices() == 0) continue;
			if (RenderMesh.Sections.Num() == 0) continue;
			const int32 VertexCount = RenderMesh.VertexBuffers.StaticMeshVertexBuffer.GetNumVertices();

			// fill vertex position
			for (int32 j = 0; j < VertexCount; ++j)
			{
				const FVector& Position = RenderMesh.VertexBuffers.PositionVertexBuffer.VertexPosition(j);
				OutDataNowLOD.Position.Add(Position);
			}
			
			// fill indices array
			{
				FIndexArrayView RawIndices = RenderMesh.IndexBuffer.GetArrayView();
				for (int32 k = 0; k < RawIndices.Num(); ++k)
				{
					OutDataNowLOD.Indices.Add(RawIndices[k]);
				}
			}

			// fill normal and tangent
			for (int32 j = 0; j < VertexCount; ++j)
			{
				FVector Normal = (FVector)(RenderMesh.VertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(j));
				FVector Tangent = (FVector)(RenderMesh.VertexBuffers.StaticMeshVertexBuffer.VertexTangentX(j));

				OutDataNowLOD.Normal.Add(Normal);
				OutDataNowLOD.Tangent.Add(Tangent);
			}

			// fill uvs
			for (int32 j = 0; j < VertexCount; ++j)
			{
				int32 UVCount = FMath::Min((int32)RenderMesh.VertexBuffers.StaticMeshVertexBuffer.GetNumTexCoords(),4);
				for (int32 k = 0; k < UVCount; ++k)
				{
					FVector2D UV = RenderMesh.VertexBuffers.StaticMeshVertexBuffer.GetVertexUV(j,k);
					OutDataNowLOD.UVs[k].Add(UV);
				}
			}

			// fill colors 
			RenderMesh.VertexBuffers.ColorVertexBuffer.GetVertexColors(OutDataNowLOD.Color);

			// fill sections
			for(FStaticMeshSection Section : RenderMesh.Sections)
			{
				SectionData Data;
				Data.MaterialIndex = Section.MaterialIndex;
				Data.FirstIndex = Section.FirstIndex;
				Data.NumTriangles = Section.NumTriangles;
				
				OutDataNowLOD.Sections.Add(Data);
			}
		}

		return OutData;
	}

	TArray<MeshData> ExportSkeletalMesh(const USkeletalMesh* InMesh)
	{
		TArray<MeshData> OutData;

		// Get LOD num
		if (InMesh == nullptr) return OutData;
		const FSkeletalMeshModel* SkelMeshResource = InMesh->GetImportedModel();
		int32 LODNum = SkelMeshResource->LODModels.Num();
		if (LODNum <= 0) return OutData;
		OutData.SetNum(LODNum);
		
		// export each LOD data 
		for (int32 i = 0; i < LODNum; ++i)
		{
			const FSkeletalMeshLODModel& RenderMesh = SkelMeshResource->LODModels[i];
			MeshData& OutDataNowLOD = OutData[i];

			// get LOD screen size
			OutDataNowLOD.LODScreenSize = InMesh->GetLODInfo(i)->ScreenSize.Default;
			
			// get vertices data, with out cloth 
			TArray<FSoftSkinVertex> Verts;
			RenderMesh.GetNonClothVertices(Verts);
			if (Verts.Num() == 0) continue;

			// export vertices data 
			for (int j = 0; j < Verts.Num(); ++j)
			{
				OutDataNowLOD.Position.Add(Verts[j].Position);
				OutDataNowLOD.Normal.Add(Verts[j].TangentZ);
				OutDataNowLOD.Tangent.Add(Verts[j].TangentX);
				OutDataNowLOD.Color.Add(Verts[j].Color);
				for (int k = 0; k < 4; ++k)
				{
					OutDataNowLOD.UVs[k].Add(Verts[j].UVs[k]);
				}
				Int4 Index;
				Float4 Weight;
				for (int k = 0; k < 4; ++k)
				{
					Index[k] = Verts[j].InfluenceBones[k];
					Weight[k] = Verts[k].InfluenceWeights[k];
				}
				OutDataNowLOD.BoneIndices.Add(Index);
				OutDataNowLOD.BoneWeight.Add(Weight);
			}
			
			// export indices data
			int32 SectionNum = RenderMesh.Sections.Num();
			int32 VertexOffset = 0;
			int32 IndexCount = 0;
			for (int32 j = 0; j < SectionNum; ++j)
			{
				const FSkelMeshSection& Section = RenderMesh.Sections[j];

				// skip clothing section 
				if (Section.HasClothingData())
				{
					VertexOffset += Section.GetNumVertices();
					continue;
				}

				// export section data
				SectionData SecData;
				SecData.MaterialIndex = Section.MaterialIndex;
				SecData.NumTriangles = Section.NumTriangles;
				SecData.FirstIndex = IndexCount;
				OutDataNowLOD.Sections.Add(SecData);

				// now export indices
				IndexCount += Section.NumTriangles;
				for (uint32 k = 0; k < Section.NumTriangles * 3; ++k)
				{
					OutDataNowLOD.Indices.Add(RenderMesh.IndexBuffer[Section.BaseIndex + k] - VertexOffset);
				}
			}
		}
		
		
		return  OutData;
	}

	AnimationInfo ExportAnimationSequence(const UAnimSequence* InAnimation)
	{
		AnimationInfo OutInfo;

		// get skeleton
		USkeleton* Skeleton = InAnimation->GetSkeleton();
		if (Skeleton == nullptr) return OutInfo;
		if (InAnimation->SequenceLength == 0.f) return OutInfo;

		// write bound info, missing 
		OutInfo.BoundCenter;
		OutInfo.BoundExtent;

		// write frame info
		OutInfo.LengthInSecond = InAnimation->SequenceLength;
		OutInfo.FramePerSecond = InAnimation->GetFrameRate();
		OutInfo.FrameCount = InAnimation->GetNumberOfFrames();

		// write bone info 
		const FReferenceSkeleton& RefSkel = Skeleton->GetReferenceSkeleton();
		OutInfo.BonesCount = RefSkel.GetRawBoneNum();
		OutInfo.AllBonePos.SetNum(OutInfo.BonesCount * OutInfo.FrameCount);
		
		// each frame 
		float AnimTime = 0;
		float AnimEndTime = InAnimation->SequenceLength;
		double TimePerKey = (InAnimation->SequenceLength / (InAnimation->GetRawNumberOfFrames() - 1));
		for (int32 CurFrame = 0; CurFrame < OutInfo.FrameCount; ++CurFrame)
		{
			// current time must smaller then sequence length 
			if (AnimEndTime < AnimTime - KINDA_SMALL_NUMBER)
			{
				UE_LOG(LogTemp,Display,TEXT("Oh Shit"));
			}
			
			// each bone 
			for (int32 CurBone = 0; CurBone < OutInfo.BonesCount; ++CurBone)
			{
				int32 BoneTrackIndex = Skeleton->GetRawAnimationTrackIndex(CurBone, InAnimation);
				if (BoneTrackIndex == INDEX_NONE)
				{
					UE_LOG(LogTemp,Display,TEXT("Shit"));
				}
				
				FTransform BoneAtom;
				InAnimation->GetBoneTransform(BoneAtom,BoneTrackIndex,AnimTime,true);
				OutInfo.AllBonePos[CurFrame * OutInfo.BonesCount + CurBone] = BoneAtom.ToMatrixWithScale();
			}

			AnimTime += TimePerKey;
		}

		return OutInfo;
	}

	// HDR -> LDR 
	FColor RGBMEncode( FLinearColor Color )
	{
		FColor Encoded;

		// Convert to gamma space
		Color.R = FMath::Sqrt( Color.R );
		Color.G = FMath::Sqrt( Color.G );
		Color.B = FMath::Sqrt( Color.B );

		// Range
		Color /= 16.0f;
		
		float MaxValue = FMath::Max( FMath::Max(Color.R, Color.G), FMath::Max(Color.B, DELTA) );
		
		if( MaxValue > 0.75f )
		{
			// Fit to valid range by leveling off intensity
			float Tonemapped = ( MaxValue - 0.75 * 0.75 ) / ( MaxValue - 0.5 );
			Color *= Tonemapped / MaxValue;
			MaxValue = Tonemapped;
		}

		Encoded.A = FMath::Min( FMath::CeilToInt( MaxValue * 255.0f ), 255 );
		Encoded.R = FMath::RoundToInt( ( Color.R * 255.0f / Encoded.A ) * 255.0f );
		Encoded.G = FMath::RoundToInt( ( Color.G * 255.0f / Encoded.A ) * 255.0f );
		Encoded.B = FMath::RoundToInt( ( Color.B * 255.0f / Encoded.A ) * 255.0f );

		return Encoded;
	}
	
	void ExportReflectionCaptureDataHDR(const USphereReflectionCaptureComponent* InReflectionCapture)
	{
		if (InReflectionCapture == nullptr) return;
		FReflectionCaptureMapBuildData* Data = InReflectionCapture->GetMapBuildData();
		if (Data == nullptr) return;
		const uint32 NumMips = FMath::CeilLogTwo(Data->CubemapSize) + 1;
		TArray<uint8>& FullData = Data->FullHDRCapturedData;
		uint32 MipOffset = 0;
		
		// each mips
		for (uint32 CurMip = 0; CurMip < NumMips; ++CurMip)
		{
			uint32 MipSize = 1 << (NumMips - CurMip - 1);

			const FFloat16Color* CurMipData = (FFloat16Color*)&FullData[MipOffset];
			const uint32 PerFaceSize = MipSize * MipSize;
			
			// each cube face 
			for (uint32 CurCubeFace = 0; CurCubeFace < CubeFace_MAX; ++CurCubeFace)
			{
				const FFloat16Color* CurFaceData = CurMipData + CurCubeFace * PerFaceSize;
				// each row 
				for (uint32 CurRow = 0; CurRow < MipSize; ++CurRow)
				{
					// each col 
					for (uint32 CurCol = 0; CurCol < MipSize; ++CurCol)
					{
						// TODO: here do export
						// !!Tip: Data may be BGR mode 
						CurFaceData[CurRow * MipSize + CurCol];
					}
				}
			}
			
			MipOffset += PerFaceSize * sizeof(FFloat16Color) * CubeFace_MAX;
		}
	}

	void ExportReflectionCaptureDataLDR(const USphereReflectionCaptureComponent* InReflectionCapture)
	{
		if (InReflectionCapture == nullptr) return;
		FReflectionCaptureMapBuildData* Data = InReflectionCapture->GetMapBuildData();
		if (Data == nullptr) return;
		const uint32 NumMips = FMath::CeilLogTwo(Data->CubemapSize) + 1;
		TArray<uint8>& FullData = Data->FullHDRCapturedData;
		uint32 MipOffset = 0;
			
		// each mips
		for (uint32 CurMip = 0; CurMip < NumMips; ++CurMip)
		{
			uint32 MipSize = 1 << (NumMips - CurMip - 1);

			const FFloat16Color* CurMipData = (FFloat16Color*)&FullData[MipOffset];
			const uint32 PerFaceSize = MipSize * MipSize;
				
			// each cube face 
			for (uint32 CurCubeFace = 0; CurCubeFace < CubeFace_MAX; ++CurCubeFace)
			{
				const FFloat16Color* CurFaceData = CurMipData + CurCubeFace * PerFaceSize;
				// each row 
				for (uint32 CurRow = 0; CurRow < MipSize; ++CurRow)
				{
					// each col 
					for (uint32 CurCol = 0; CurCol < MipSize; ++CurCol)
					{
						// TODO: here do export
						// !!Tip: Data may be BGR mode 
						const FLinearColor LinearColor = FLinearColor(CurFaceData[CurRow * MipSize + CurCol]) * Data->Brightness;
						FColor LDRData = RGBMEncode( LinearColor );
					}
				}
			}
				
			MipOffset += PerFaceSize * sizeof(FFloat16Color) * CubeFace_MAX;
		}
	}

	TArray<MipData> ExportTexture(UTexture2D* InTexture)
	{
		TArray<MipData> OutData;
		if (!InTexture)
		{
			UE_LOG(LogTemp,Error,TEXT("Export error, reason: InTexture is null"));
			return OutData;
		}

		int32 NumMips = InTexture->GetNumMips();

		// each mip 
		for (int32 i = 0; i < NumMips; ++i)
		{
			MipData CurOutMip;
			FTexture2DMipMap* CurMip = &InTexture->PlatformData->Mips[i];
			FByteBulkData* ImageData = &CurMip->BulkData;

			int32 MipWidth = CurMip->SizeX;
			int32 MipHeight = CurMip->SizeY;

			CurOutMip.Width = MipWidth;
			CurOutMip.Height = MipHeight;
			
			// each texel
			for (int32 Row = 0; Row < MipHeight; ++Row)
			{
				for (int32 Col = 0; Col < MipWidth; ++Col)
				{
					// use platform data 
					if ((ImageData->IsBulkDataLoaded() && ImageData->GetBulkDataSize() > 0)
					&&	(InTexture->GetPixelFormat() == PF_B8G8R8A8 || InTexture->GetPixelFormat() == PF_FloatRGBA))
					{
						int32 TexelIndex = Row * MipWidth + Col;

						if (InTexture->GetPixelFormat() == PF_B8G8R8A8)
						{
							FColor* MipData = static_cast<FColor*>(ImageData->Lock(LOCK_READ_ONLY));
							FColor Texel = MipData[TexelIndex];
							ImageData->Unlock();

							if (InTexture->SRGB)
							{
								CurOutMip.Data.Emplace(Texel);
							}
							else
							{
								CurOutMip.Data.Emplace(Texel.ReinterpretAsLinear());
							}
						}
						else if (InTexture->GetPixelFormat() == PF_FloatRGBA)
						{
							FFloat16Color* MipData = static_cast<FFloat16Color*>(ImageData->Lock(LOCK_READ_ONLY));
							FFloat16Color Texel = MipData[TexelIndex];

							ImageData->Unlock();
							CurOutMip.Data.Emplace(float(Texel.R), float(Texel.G), float(Texel.B), float(Texel.A));
						}
					}
					// read texture source if platform data is unavailable
					else
					{
						FTextureSource& TextureSource = InTexture->Source;

						TArray64<uint8> SourceData;
						InTexture->Source.GetMipData(SourceData, Mip);
						ETextureSourceFormat SourceFormat = TextureSource.GetFormat();
						int32 Index = ((Row * MipWidth) + Col) * TextureSource.GetBytesPerPixel();

						if ((SourceFormat == TSF_BGRA8 || SourceFormat == TSF_BGRE8))
						{
							FColor Texel;
							const uint8* PixelPtr = SourceData.GetData() + Index;
							Texel = *((FColor*)PixelPtr);
							if (InTexture->SRGB)
							{
								CurOutMip.Data.Emplace(FLinearColor::FromSRGBColor(Texel));
							}
							CurOutMip.Data.Emplace(Texel.ReinterpretAsLinear());
						}
						else if ((SourceFormat == TSF_RGBA16 || SourceFormat == TSF_RGBA16F))
						{
							FFloat16Color Texel;
							const uint8* PixelPtr = SourceData.GetData() + Index;
							Texel = *((FFloat16Color*)PixelPtr);

							CurOutMip.Data.Emplace(float(Texel.R), float(Texel.G), float(Texel.B), float(Texel.A));
						}
						else if (SourceFormat == TSF_G8)
						{
							const uint8* PixelPtr = SourceData.GetData() + Index;
							const uint8 Value = *PixelPtr;
							if (InTexture->SRGB)
							{
								CurOutMip.Data.Emplace(FLinearColor::FromSRGBColor(FColor(float(Value), 0, 0, 0)));
							}
							CurOutMip.Data.Emplace(float(Value), 0, 0, 0);
						}
					}
				}
			}

			// emplace data
			OutData.Emplace(CurOutMip);
		}
		
		return OutData;
	}

	TArray<MipData> ExportCubeTexture(UTextureCube* InTexture)
	{
		TArray<MipData> OutData;
		
		return OutData;
	}
}

