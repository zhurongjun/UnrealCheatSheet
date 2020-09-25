// Fill out your copyright notice in the Description page of Project Settings.


#include "MaterialTools.h"

#include "Materials/MaterialInstanceDynamic.h"

void UMaterialTools::GetMaterialParams(UMaterial* InMaterial)
{
	if (InMaterial == nullptr) return;

	TArray<FMaterialParameterInfo> Infos;
	TArray<FGuid> Guids;
	InMaterial->GetAllScalarParameterInfo(Infos,Guids);
	InMaterial->GetAllTextureParameterInfo(Infos,Guids);
	InMaterial->GetAllVectorParameterInfo(Infos,Guids);
}
