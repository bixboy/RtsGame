#include "Widgets/Map/MapWidget.h"
#include "EngineUtils.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/MapIconComponent.h"
#include "Manager/DynamicCameraMap.h"


void UMapWidget::NativePreConstruct()
{
    Super::NativePreConstruct();

    if (MinimapImage)
    {
        if (UCanvasPanelSlot* TempSlot = Cast<UCanvasPanelSlot>(MinimapImage->Slot))
        {
            TempSlot->SetSize(FVector2D(MiniMapSize, MiniMapSize));
        }
    }
}

void UMapWidget::NativeConstruct()
{
    Super::NativeConstruct();

    for (TActorIterator<ADynamicCameraMap> It(GetWorld()); It; ++It)
    {
        MapCam = *It;
        break;
    }
    if (!MapCam) return;

    if (MinimapImage && MapCam)
    {
        if (UCanvasPanelSlot* TempSlot = Cast<UCanvasPanelSlot>(MinimapImage->Slot))
        {
            TempSlot->SetSize(FVector2D(MiniMapSize, MiniMapSize));
            ImageSize = TempSlot->GetSize();
        }
    }

    FVector C = MapCam->GetActorLocation();
    WorldSize = MapCam->OrthoWidth;
    
    float Half = WorldSize * 0.5f;
    WorldOrig = FVector2D(C.X - Half, C.Y + Half);

    for (auto& WeakComp : MapCam->GetMapIcons())
    {
        if (UMapIconComponent* Comp = WeakComp.Get())
        {
            if (Comp->bCanShow)
                AddIcon(Comp);
        }
    }
}

void UMapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    
    if (!MapCam) return;
    
    TArray<UMapIconComponent*> ToRemove;
    for (auto& WeakComp : MapCam->GetMapIcons())
    {
        UMapIconComponent* Comp = WeakComp.Get();
        if (!Comp) continue;

        bool bHasIcon = IconMap.Contains(Comp);
        if (Comp->bCanShow && !bHasIcon)
        {
            AddIcon(Comp);
        }
        else if (!Comp->bCanShow && bHasIcon)
        {
            ToRemove.Add(Comp);
        }
    }

    for (UMapIconComponent* Comp : ToRemove)
    {
        RemoveIcon(Comp);
    }

    FVector CamLoc = MapCam->GetActorLocation();
    float HalfSize = MapCam->OrthoWidth * 0.5f;
    
    float MinX = CamLoc.X - HalfSize;
    float MinY = CamLoc.Y - HalfSize;
    float Size  = MapCam->OrthoWidth;

    for (auto& Pair : IconMap)
    {
        UMapIconComponent* Comp = Pair.Key;
        UImage* IconImg = Pair.Value;
        
        if (!Comp || !Comp->bCanShow || !IconImg) continue;

        FVector Pos3D = Comp->GetOwner()->GetActorLocation();

        float U = (Pos3D.Y - MinY) / Size;
        float V = (Pos3D.X - MinX) / Size;

        U = FMath::Clamp(U, 0.f, 1.f);
        V = FMath::Clamp(V, 0.f, 1.f);

        float Px = U * ImageSize.X;
        float Py = (1.f - V) * ImageSize.Y;

        if (auto* TempSlot = Cast<UCanvasPanelSlot>(IconImg->Slot))
        {
            FVector2D HalfIcon = TempSlot->GetSize() * 0.5f;
            TempSlot->SetPosition(FVector2D(Px, Py) - HalfIcon);
        }
    }
}


void UMapWidget::AddIcon(UMapIconComponent* Comp)
{
    if (!Comp || IconMap.Contains(Comp) || !IconCanvas) return;

    UImage* IconWidget = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
    UTexture2D* Tex = Comp->IconTexture ? Comp->IconTexture : DefaultIcon;
    
    IconWidget->SetBrushFromTexture(Tex, true);

    if (auto* TempSlot = IconCanvas->AddChildToCanvas(IconWidget))
    {
        TempSlot->SetSize(Comp->IconSize);
        TempSlot->SetAutoSize(false);
    }

    IconMap.Add(Comp, IconWidget);
}

void UMapWidget::RemoveIcon(UMapIconComponent* Comp)
{
    if (!Comp || !IconCanvas) return;

    UImage** WidgetPtr = IconMap.Find(Comp);
    if (WidgetPtr && *WidgetPtr)
    {
        UImage* IconWidget = *WidgetPtr;
        
        IconCanvas->RemoveChild(IconWidget);
        IconWidget->RemoveFromParent();
    }

    IconMap.Remove(Comp);
}
