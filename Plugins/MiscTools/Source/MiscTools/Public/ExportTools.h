#pragma once

#include "CoreMinimal.h"

class USphereReflectionCaptureComponent;

// export mesh & animation 
namespace Fuko
{
	
    // help data 
    struct Int4
    {
    	int32 Data[4];
    	int32& operator[](int32 Index) { return Data[Index]; }
    	const int32& operator[](int32 Index) const { return Data[Index]; }
    };
    struct Float4
    {
    	float Data[4];
    	float& operator[](int32 Index) { return Data[Index]; }
    	const float& operator[](int32 Index) const { return Data[Index]; }
    };
	
	// mesh section 
    struct SectionData
    {
    	// index of material 
    	int32 MaterialIndex;
    	// offset of index buffer 
    	uint32 FirstIndex;
    	// num of triangle, index = NumTriangles * 3  
    	uint32 NumTriangles;
    };

    // mesh data 
    struct MeshData
    {
    	//---------------VertexData---------------
    	TArray<FVector>		Position;
    	TArray<FVector>		Normal;
    	TArray<FVector>		Tangent;
    	TArray<FColor>		Color;

    	//---------------UV---------------
        TArray<FVector2D> 	UVs[4];

    	//---------------BoneData---------------
    	TArray<Int4> 		BoneIndices;
    	TArray<Float4> 		BoneWeight;
    	//---------------Indices---------------
    	TArray<uint32> 		Indices;
    	//---------------SectionData---------------
    	TArray<SectionData> Sections;
		//---------------LOD data----------------
    	float				LODScreenSize;
    	
    	void CleanData()
    	{
    		Position.Reset();
    		Normal.Reset();
    		Tangent.Reset();
    		Color.Reset();
    		for(int i = 0; i < 4; ++i)
    		{
    			UVs[i].Reset();
    		}
    		BoneIndices.Reset();
    		BoneWeight.Reset();
    		Indices.Reset();
    	}
    };

    // animation data
    struct AnimationInfo
    {
    	FVector	BoundCenter = FVector::ZeroVector;
    	FVector BoundExtent = FVector::ZeroVector;
    	float LengthInSecond = 0.f;
    	float FramePerSecond = 0.f;
    	int32 FrameCount = 0;
    	int32 BonesCount = 0;
    	TArray<FMatrix> AllBonePos;
    };

	// convert LOD screen size to LOD distance
	// use 1080p and 90 degree FOV default 
	float LODScreenSizeToDistance(
        float InSize,
        float InBoundingSphereRadius,
        float InHalfFOV = PI * 0.25f,
        float InScreenWidth = 1920.0f,
        float InScreenHeight = 1080.0f);

	// export mesh data, include full LOD information 
	TArray<MeshData> ExportStaticMesh(const UStaticMesh* InMesh);

	// export skeletal mesh data, include full LOD information
	TArray<MeshData> ExportSkeletalMesh(const USkeletalMesh* InMesh);

	// export animation sequence data 
	AnimationInfo ExportAnimationSequence(const UAnimSequence* InAnimation);

	// export reflection capture data
	void ExportReflectionCaptureDataHDR(const USphereReflectionCaptureComponent* InReflectionCapture);
	void ExportReflectionCaptureDataLDR(const USphereReflectionCaptureComponent* InReflectionCapture);
}

// export texture
// unreal texel sample api UBlueprintMaterialTextureNodesBPLibrary::Texture2D_SampleUV_EditorOnly
namespace Fuko
{
	struct MipData
	{
		int32 					Width;
		int32 					Height;
		TArray<FLinearColor> 	Data;
	};

	TArray<MipData> ExportTexture(UTexture2D* InTexture);
	TArray<MipData> ExportCubeTexture(UTextureCube* InTexture);
}