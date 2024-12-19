#include "RtsGame/Public/SoldierRts.h"

ASoldierRts::ASoldierRts()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASoldierRts::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASoldierRts::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASoldierRts::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}



void ASoldierRts::CommandStart(const FInputActionValue& Value)
{
	if (!PlayerOwner)
	{
		return;
	}

	//CommandLocation = PlayerOwner->GetMousePositionOnTerrain;
}

void ASoldierRts::Select()
{
	Selected = true;
	Highlight(Selected);
}

void ASoldierRts::Deselect()
{
	Selected = false;
	Highlight(Selected);
}

void ASoldierRts::Highlight(const bool Highlight)
{
	TArray<UPrimitiveComponent*> Components;
	GetComponents<UPrimitiveComponent>(Components);
	for(UPrimitiveComponent* VisualComp : Components)
	{
		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(VisualComp))
		{
			Prim->SetRenderCustomDepth(Highlight);
		}
	}
}

//FCommandData ASoldierRts::CreatCommandData(const ECommandType Type) const
//{
//}

