#include "Manager/DynamicCameraMap.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"


ADynamicCameraMap::ADynamicCameraMap()
{
	SceneCaptureComp = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCaptureComp");
	SceneCaptureComp->SetupAttachment(GetRootComponent());

	SceneCaptureComp->ProjectionType      = ECameraProjectionMode::Orthographic;
	SceneCaptureComp->OrthoWidth          = OrthoWidth;
	SceneCaptureComp->CaptureSource       = SCS_FinalColorLDR;
	SceneCaptureComp->bCaptureEveryFrame  = true;
	SceneCaptureComp->bCaptureOnMovement  = false;
	SceneCaptureComp->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_RenderScenePrimitives;
}

void ADynamicCameraMap::BeginPlay()
{
	Super::BeginPlay();
	
	UpdateRenderTarget();
}

void ADynamicCameraMap::UpdateRenderTarget()
{
	if (!MinimapRenderTarget)
	{
		MinimapRenderTarget = NewObject<UTextureRenderTarget2D>(this, "MinimapRT");
		MinimapRenderTarget->InitAutoFormat(512,512);
		MinimapRenderTarget->ClearColor  = FLinearColor::Black;
		MinimapRenderTarget->TargetGamma = 2.2f;
	}
	SceneCaptureComp->TextureTarget = MinimapRenderTarget;

	for (auto& Cls : ClassesToHide)
	{
		if (!*Cls) continue;
		TArray<AActor*> Found;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), Cls, Found);
		for (AActor* A : Found)
		{
			if (A)
				SceneCaptureComp->HiddenActors.AddUnique(A);
		}
	}
}

void ADynamicCameraMap::RegisterMapIcon(UMapIconComponent* Comp)
{
	RegisteredIcons.Add(Comp);
}

void ADynamicCameraMap::UnregisterMapIcon(UMapIconComponent* Comp)
{
	RegisteredIcons.Remove(Comp);
}


