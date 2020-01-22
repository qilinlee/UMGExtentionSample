#include "SMyToggle.h"
#include "Types/PaintArgs.h"
#include "Layout/ArrangedChildren.h"
#include "SlateSettings.h"
#include "Widgets/SWidget.h"
#include "Layout/WidgetPath.h"
#include "Framework/Application/SlateApplication.h"


SMyToggle::SMyToggle()
	: Children(this)
{
	SetCanTick(false);
	bCanSupportFocus = true;
}

void SMyToggle::Construct(const SMyToggle::FArguments& InArgs)
{
	const int32 NumSlots = InArgs.Slots.Num();
	for (int32 SlotIndex = 0; SlotIndex < NumSlots; ++SlotIndex)
	{
		Children.Add(InArgs.Slots[SlotIndex]);
	}

	IsToggleChecked = InArgs._IsToggleChecked;
	bIsFocusable = InArgs._IsFocusable;
	OnToggleCheckStateChanged = InArgs._OnToggleCheckStateChanged;
	ClickMethod = InArgs._ClickMethod.Get();
	OnGetMenuContent = InArgs._OnGetMenuContent;

	bIsPressed = false;

}

void SMyToggle::ClearChildren()
{
	if (Children.Num())
	{
		Invalidate(EInvalidateWidget::Layout);
		Children.Empty();
	}
}

struct FToggleChildZOrder
{
	int32 ChildIndex;
	float ZOrder;
};

struct FToggleSortSlotsByZOrder
{
	FORCEINLINE bool operator()(const FToggleChildZOrder& A, const FToggleChildZOrder& B) const
	{
		return A.ZOrder == B.ZOrder ? A.ChildIndex < B.ChildIndex : A.ZOrder < B.ZOrder;
	}
};

bool SMyToggle::IsSameWithCheckState(const EToggleSlotType& SlotType) const
{
	if (EToggleSlotType::Other == SlotType)
		return true;

	return (uint8)SlotType == (uint8)IsToggleChecked.Get();
}

void SMyToggle::ArrangeLayeredChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren, FArrangedChildLayers& ArrangedChildLayers) const
{
	if (Children.Num() <= 0)
		return;

#if WITH_EDITOR
	const bool bExplicitChildZOrder = GetDefault<USlateSettings>()->bExplicitCanvasChildZOrder;
#else
	const static bool bExplicitChildZOrder = GetDefault<USlateSettings>()->bExplicitCanvasChildZOrder;
#endif

	TArray< FToggleChildZOrder, TInlineAllocator<64> > SlotOrder;
	SlotOrder.Reserve(Children.Num());

	for (int32 ChildIndex = 0; ChildIndex < Children.Num(); ++ChildIndex)
	{
		const SMyToggle::FSlot& CurChild = Children[ChildIndex];
		if (IsSameWithCheckState(CurChild.SlotTypeAttr.Get()))
		{
			FToggleChildZOrder Order;
			Order.ChildIndex = ChildIndex;
			Order.ZOrder = CurChild.ZOrderAttr.Get();
			SlotOrder.Add(Order);
		}
	}

	SlotOrder.Sort(FToggleSortSlotsByZOrder());
	float LastZOrder = -FLT_MAX;

	for (int32 ChildIndex = 0; ChildIndex < SlotOrder.Num(); ++ChildIndex)
	{
		const FToggleChildZOrder& CurSlotZOrder = SlotOrder[ChildIndex];
		const SMyToggle::FSlot& CurSlot = Children[CurSlotZOrder.ChildIndex];
		const TSharedRef<SWidget>& CurWidget = CurSlot.GetWidget();

		const EVisibility ChildVisibility = CurWidget->GetVisibility();
		if (!ArrangedChildren.Accepts(ChildVisibility))
			continue;

		if (!IsSameWithCheckState(CurSlot.SlotTypeAttr.Get()))
			continue;

		const FMargin& Offset = CurSlot.OffsetAttr.Get();
		const FVector2D& Alignment = CurSlot.AlignmentAttr.Get();
		const FAnchors& Anchors = CurSlot.AnchorsAttr.Get();
		const bool AutoSize = CurSlot.AutoSizeAttr.Get();

		const FVector2D& LocalSizeGeometry = AllottedGeometry.GetLocalSize();
		const FMargin AnchorPixels = FMargin(
			Anchors.Minimum.X * LocalSizeGeometry.X,
			Anchors.Minimum.Y * LocalSizeGeometry.Y,
			Anchors.Maximum.X * LocalSizeGeometry.X,
			Anchors.Maximum.Y * LocalSizeGeometry.Y);

		bool bIsHorizontalStretch = Anchors.Minimum.X != Anchors.Maximum.X;
		bool bIsVerticalStretch = Anchors.Minimum.Y != Anchors.Maximum.Y;
		FVector2D SlotSize(Offset.Right, Offset.Bottom);
		FVector2D Size = AutoSize ? CurWidget->GetDesiredSize() : SlotSize;
		FVector2D AlignmentOffset = Size * Alignment;
		FVector2D LocalPosition, LocalSize;

		if (bIsHorizontalStretch)
		{
			LocalPosition.X = AnchorPixels.Left + Offset.Left;
			LocalSize.X = AnchorPixels.Right - LocalPosition.X - Offset.Right;
		}
		else
		{
			LocalPosition.X = AnchorPixels.Left + Offset.Left - AlignmentOffset.X;
			LocalSize.X = Size.X;
		}

		if (bIsVerticalStretch)
		{
			LocalPosition.Y = AnchorPixels.Top + Offset.Top;
			LocalSize.Y = AnchorPixels.Bottom - LocalPosition.Y - Offset.Bottom;
		}
		else
		{
			LocalPosition.Y = AnchorPixels.Top + Offset.Top - AlignmentOffset.Y;
			LocalSize.Y = Size.Y;
		}

		ArrangedChildren.AddWidget(ChildVisibility,
			AllottedGeometry.MakeChild(CurWidget, LocalPosition, LocalSize));

		bool bNewLayer = true;
		if (bExplicitChildZOrder)
		{
			bNewLayer = false;
			if (CurSlotZOrder.ZOrder > LastZOrder + DELTA)
			{
				if (ArrangedChildLayers.Num() > 0)
				{
					bNewLayer = true;
				}
				LastZOrder = CurSlotZOrder.ZOrder;
			}
		}

		ArrangedChildLayers.Add(bNewLayer);
	}

}

void SMyToggle::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
	FArrangedChildLayers ChildLayers;
	ArrangeLayeredChildren(AllottedGeometry, ArrangedChildren, ChildLayers);
}

int32 SMyToggle::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	SCOPED_NAMED_EVENT_TEXT("SMyToggle", FColor::Cyan);
	FArrangedChildLayers ChildLayers;
	FArrangedChildren ArrangedChildren(EVisibility::Visible);
	ArrangeLayeredChildren(AllottedGeometry, ArrangedChildren, ChildLayers);
	const bool bForwardedEnabled = ShouldBeEnabled(bParentEnabled);

	int32 MaxLayerId = LayerId;
	int32 ChildLayerId = LayerId;

	const FPaintArgs NewArgs = Args.WithNewParent(this);

	for (int32 ChildIndex = 0; ChildIndex < ArrangedChildren.Num(); ++ChildIndex)
	{
		FArrangedWidget& CurWidget = ArrangedChildren[ChildIndex];
		if (!IsChildWidgetCulled(MyCullingRect, CurWidget))
		{
			ChildLayerId = ChildLayers[ChildIndex] ? MaxLayerId + 1 : ChildLayerId;
			const int32 CurWidgetsMaxLayerId = CurWidget.Widget->Paint(NewArgs,
				CurWidget.Geometry, MyCullingRect, OutDrawElements,
				ChildLayerId, InWidgetStyle, bForwardedEnabled);
			MaxLayerId = FMath::Max(MaxLayerId, CurWidgetsMaxLayerId);
		}
	}

	return MaxLayerId;
}

FVector2D SMyToggle::ComputeDesiredSize(float) const
{
	FVector2D FinalDesiredSize(0, 0);

	// Arrange the children now in their proper z-order.
	for (int32 ChildIndex = 0; ChildIndex < Children.Num(); ++ChildIndex)
	{
		const SMyToggle::FSlot& CurChild = Children[ChildIndex];
		const TSharedRef<SWidget>& Widget = CurChild.GetWidget();
		const EVisibility ChildVisibilty = Widget->GetVisibility();

		// As long as the widgets are not collapsed, they should contribute to the desired size.
		if (ChildVisibilty != EVisibility::Collapsed && IsSameWithCheckState(CurChild.SlotTypeAttr.Get()))
		{
			const FMargin Offset = CurChild.OffsetAttr.Get();
			const FVector2D Alignment = CurChild.AlignmentAttr.Get();
			const FAnchors Anchors = CurChild.AnchorsAttr.Get();

			const FVector2D SlotSize = FVector2D(Offset.Right, Offset.Bottom);

			const bool AutoSize = CurChild.AutoSizeAttr.Get();

			const FVector2D Size = AutoSize ? Widget->GetDesiredSize() : SlotSize;

			const bool bIsDockedHorizontally = (Anchors.Minimum.X == Anchors.Maximum.X) && (Anchors.Minimum.X == 0 || Anchors.Minimum.X == 1);
			const bool bIsDockedVertically = (Anchors.Minimum.Y == Anchors.Maximum.Y) && (Anchors.Minimum.Y == 0 || Anchors.Minimum.Y == 1);

			FinalDesiredSize.X = FMath::Max(FinalDesiredSize.X, Size.X + (bIsDockedHorizontally ? FMath::Abs(Offset.Left) : 0.0f));
			FinalDesiredSize.Y = FMath::Max(FinalDesiredSize.Y, Size.Y + (bIsDockedVertically ? FMath::Abs(Offset.Top) : 0.0f));
		}
	}

	return FinalDesiredSize;
}

void SMyToggle::SetToggleIsChecked(TAttribute<ECheckBoxState> InIsToggleChecked)
{
	IsToggleChecked = InIsToggleChecked;
}

int32 SMyToggle::RemoveSlot(const TSharedRef<SWidget>& SlotWidget)
{
	Invalidate(EInvalidateWidget::Layout);
	for (int32 SlotIdx = 0; SlotIdx < Children.Num(); ++SlotIdx)
	{
		if (SlotWidget == Children[SlotIdx].GetWidget())
		{
			Children.RemoveAt(SlotIdx);
			return SlotIdx;
		}
	}

	return -1;
}

bool SMyToggle::SupportsKeyboardFocus() const
{
	return bIsFocusable;
}

FReply SMyToggle::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Enter 
		|| InKeyEvent.GetKey() == EKeys::SpaceBar 
		|| InKeyEvent.GetKey() == EKeys::Virtual_Accept)
	{
		ToggleCheckedState();
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply SMyToggle::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bIsPressed = true;

		if (ClickMethod == EButtonClickMethod::MouseDown)
		{
			ToggleCheckedState();

			// Set focus to this button, but don't capture the mouse
			return FReply::Handled().SetUserFocus(AsShared(), EFocusCause::Mouse);
		}
		else
		{
			// Capture the mouse, and also set focus to this button
			return FReply::Handled().CaptureMouse(AsShared()).SetUserFocus(AsShared(), EFocusCause::Mouse);
		}
	}
	else if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton && OnGetMenuContent.IsBound())
	{
		FWidgetPath WidgetPath = MouseEvent.GetEventPath() != nullptr ? *MouseEvent.GetEventPath() : FWidgetPath();

		FSlateApplication::Get().PushMenu(
			AsShared(),
			WidgetPath,
			OnGetMenuContent.Execute(),
			MouseEvent.GetScreenSpacePosition(),
			FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu)
		);

		return FReply::Handled();
	}
	else
	{
		return FReply::Unhandled();
	}
}

FReply SMyToggle::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	return OnMouseButtonDown(InMyGeometry, InMouseEvent);
}

FReply SMyToggle::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bIsPressed = false;

		if (ClickMethod == EButtonClickMethod::MouseDown)
		{
			// NOTE: If we're configured to click on mouse-down, then we never capture the mouse thus
			//       may never receive an OnMouseButtonUp() call.  We make sure that our bIsPressed
			//       state is reset by overriding OnMouseLeave().
		}
		else
		{
			const bool IsUnderMouse = MyGeometry.IsUnderLocation(MouseEvent.GetScreenSpacePosition());
			if (IsUnderMouse)
			{
				// If we were asked to allow the button to be clicked on mouse up, regardless of whether the user
				// pressed the button down first, then we'll allow the click to proceed without an active capture
				if (ClickMethod == EButtonClickMethod::MouseUp || HasMouseCapture())
				{
					ToggleCheckedState();
				}
			}
		}

		return FReply::Handled().ReleaseMouseCapture();
	}

	return FReply::Unhandled();
}

void SMyToggle::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	SWidget::OnMouseEnter(MyGeometry, MouseEvent);
}

void SMyToggle::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	SWidget::OnMouseLeave(MouseEvent);

	// If we're setup to click on mouse-down, then we never capture the mouse and may not receive a
	// mouse up event, so we need to make sure our pressed state is reset properly here
	if (ClickMethod == EButtonClickMethod::MouseDown)
	{
		bIsPressed = false;
	}
}

bool SMyToggle::IsInteractable() const
{
	return IsEnabled();
}

void SMyToggle::ToggleCheckedState()
{
	const ECheckBoxState State = IsToggleChecked.Get();

	// If the current check box state is checked OR undetermined we set the check box to checked.
	if (State == ECheckBoxState::Checked || State == ECheckBoxState::Undetermined)
	{
		if (!IsToggleChecked.IsBound())
		{
			// When we are not bound, just toggle the current state.
			IsToggleChecked.Set(ECheckBoxState::Unchecked);
		}

		// The state of the check box changed.  Execute the delegate to notify users
		OnToggleCheckStateChanged.ExecuteIfBound(ECheckBoxState::Unchecked);
	}
	else if (State == ECheckBoxState::Unchecked)
	{
		if (!IsToggleChecked.IsBound())
		{
			// When we are not bound, just toggle the current state.
			IsToggleChecked.Set(ECheckBoxState::Checked);
		}

		// The state of the check box changed.  Execute the delegate to notify users
		OnToggleCheckStateChanged.ExecuteIfBound(ECheckBoxState::Checked);
	}
}