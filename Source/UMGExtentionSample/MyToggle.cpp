// Fill out your copyright notice in the Description page of Project Settings.


#include "MyToggle.h"
#include "SMyToggle.h"
#include "MyToggleSlot.h"
#include "Layout/ArrangedChildren.h"

#define LOCTEXT_NAMESPACE "UMG"

UMyToggle::UMyToggle(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsVariable = true;
	SMyToggle::FArguments Defaults;
	Visibility = UWidget::ConvertRuntimeToSerializedVisibility(Defaults._Visibility.Get());
}

void UMyToggle::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MyToggle.Reset();
}

void UMyToggle::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (MyToggle.IsValid())
		MyToggle->SetToggleIsChecked(PROPERTY_BINDING(ECheckBoxState, CheckedState));
}

#if WITH_EDITOR
const FText UMyToggle::GetPaletteCategory()
{
	return LOCTEXT("Custom Control", "Custom Control");
}
#endif

TSharedRef<SWidget> UMyToggle::RebuildWidget()
{
	MyToggle = SNew(SMyToggle)
		.IsToggleChecked(CheckedState)
		.IsFocusable(IsFocusable)
		.OnToggleCheckStateChanged(BIND_UOBJECT_DELEGATE(FOnToggleCheckStateChanged, SlateOnToggleCheckeStateChanged));

	for (UPanelSlot* slot : Slots)
	{
		if (UMyToggleSlot* ToggleSlot = Cast<UMyToggleSlot>(slot))
		{
			ToggleSlot->Parent = this;
			ToggleSlot->BuildSlot(MyToggle.ToSharedRef());
		}
	}

	return MyToggle.ToSharedRef();
}

void UMyToggle::OnWidgetRebuilt()
{
}

UClass* UMyToggle::GetSlotClass() const
{
	return UMyToggleSlot::StaticClass();
}

void UMyToggle::OnSlotAdded(UPanelSlot* InSlot)
{
	if (MyToggle.IsValid())
	{
		CastChecked<UMyToggleSlot>(InSlot)->BuildSlot(MyToggle.ToSharedRef());
	}
}

void UMyToggle::OnSlotRemoved(UPanelSlot* InSlot)
{
	if (MyToggle.IsValid())
	{
		TSharedPtr<SWidget> Widget = InSlot->Content->GetCachedWidget();
		if (Widget.IsValid())
		{
			MyToggle->RemoveSlot(Widget.ToSharedRef());
		}
	}
}

TSharedPtr<SMyToggle> UMyToggle::GetToggleWidget() const
{
	return MyToggle;
}

bool UMyToggle::GetGeometryForSlot(UMyToggleSlot* InSlot, FGeometry& ArrangedGeometry) const
{
	if (InSlot->Content == nullptr)
	{
		return false;
	}

	TSharedPtr<SMyToggle> Canvas = GetToggleWidget();
	if (Canvas.IsValid())
	{
		FArrangedChildren ArrangedChildren(EVisibility::All);
		Canvas->ArrangeChildren(Canvas->GetCachedGeometry(), ArrangedChildren);

		for (int32 ChildIndex = 0; ChildIndex < ArrangedChildren.Num(); ChildIndex++)
		{
			if (ArrangedChildren[ChildIndex].Widget == InSlot->Content->GetCachedWidget())
			{
				ArrangedGeometry = ArrangedChildren[ChildIndex].Geometry;
				return true;
			}
		}
	}

	return false;
}

void UMyToggle::SlateOnToggleCheckeStateChanged(ECheckBoxState NewState)
{
	ECheckBoxState Last = CheckedState;
	CheckedState = NewState;

	OnToggleCheckStateChanged.Broadcast(Last, NewState);
}

#undef LOCTEXT_NAMESPACE