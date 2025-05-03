#include "Widgets/BuildsInfo/BuildProgressWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Interfaces/BuildInterface.h"
#include "Structures/StructureBase.h"
#include "Widgets/BuildsInfo/BuildInfoBox.h"


void UBuildProgressWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (UBuildInfoBox* ParentBox = GetTypedOuter<UBuildInfoBox>())
	{
		BuildInfoBox = ParentBox;
	}
}

void UBuildProgressWidget::UpdateProgress(AActor* NewBuild)
{
	if (Build)
	{
		Build->OnBuildStart.RemoveDynamic(this, &UBuildProgressWidget::OnBuildStart);
		Build->OnBuildStop.RemoveDynamic(this, &UBuildProgressWidget::OnBuildStop);
		Build->OnBuildComplete.RemoveDynamic(this, &UBuildProgressWidget::OnBuildCompleted);
	}

	Build = Cast<AStructureBase>(NewBuild);
	SimulatedElapsed = 0.f;
	bUpdatePercent   = false;

	if (Build)
	{
		Build->OnBuildStart.AddDynamic(this, &UBuildProgressWidget::OnBuildStart);
		Build->OnBuildStop.AddDynamic(this, &UBuildProgressWidget::OnBuildStop);
		Build->OnBuildComplete.AddDynamic(this, &UBuildProgressWidget::OnBuildCompleted);

		float Initial = IBuildInterface::Execute_GetBuildProgress(Build);
		
		ProgressBar->SetPercent(Initial);
		ProgressText->SetText(FText::AsPercent(Initial));

		if (Build->GetIsInBuild())
		{
			OnBuildStart(Initial);
		}
	}
}

void UBuildProgressWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bUpdatePercent || !Build)
		return;

	SimulatedElapsed += InDeltaTime * Build->GetBuilders();
	
	SimulatedElapsed = FMath::Min(SimulatedElapsed, TotalBuildTime);
	float Percentage = SimulatedElapsed / TotalBuildTime;

	ProgressBar->SetPercent(Percentage);
	ProgressText->SetText(FText::AsPercent(Percentage));

	if (SimulatedElapsed >= TotalBuildTime)
	{
		bUpdatePercent = false;

		TArray<AActor*> Builds;
		Builds.Add(Build);
		BuildInfoBox->SetupBuildInfo(Builds, nullptr);
	}
}

void UBuildProgressWidget::OnBuildStart(float Progress)
{
	if (!Build) return;
	
	TotalBuildTime = Build->GetBuildData().TimeToBuild;
	SimulatedElapsed = Progress * TotalBuildTime;
	bUpdatePercent   = true;
}

void UBuildProgressWidget::OnBuildStop()
{
	bUpdatePercent = false;
}

void UBuildProgressWidget::OnBuildCompleted()
{
	TArray<AActor*> Builds;
	Builds.Add(Build);
	BuildInfoBox->SetupBuildInfo(Builds, nullptr);
}
