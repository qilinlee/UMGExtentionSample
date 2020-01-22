// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlateCore/Public/Widgets/SPanel.h"
#include "Styling/SlateTypes.h"
#include "Misc/Attribute.h"
#include "Layout/Visibility.h"
#include "Layout/Margin.h"
#include "Layout/Geometry.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SlotBase.h"
#include "Widgets/SWidget.h"
#include "Layout/Children.h"
#include "Widgets/SPanel.h"
#include "Widgets/Layout/Anchors.h"
#include "UMGExtensionDefine.h"
#include "Input/Reply.h"
#include "Framework/SlateDelegates.h"

struct FGeometry;
struct FPointerEvent;
struct FKeyEvent;

DECLARE_DELEGATE_OneParam(FOnToggleCheckStateChanged, ECheckBoxState);

/**
 * 
 */
class UMGEXTENTIONSAMPLE_API SMyToggle : public SPanel
{
public:
    class FSlot : public TSlotBase<FSlot>
    {
    public:
		/** Offset */
		TAttribute<FMargin> OffsetAttr;

		/** Anchors */
		TAttribute<FAnchors> AnchorsAttr;

		/** Size */
		TAttribute<FVector2D> AlignmentAttr;

		/** Auto-Size */
		TAttribute<bool> AutoSizeAttr;

		/** Z-Order */
		TAttribute<float> ZOrderAttr;

		/** StateBelonged */
        TAttribute<EToggleSlotType> SlotTypeAttr;
        
		FSlot()
			: TSlotBase<FSlot>()
			, OffsetAttr(FMargin(0, 0, 1, 1))
			, AnchorsAttr(FAnchors(0.0f, 0.0f))
			, AlignmentAttr(FVector2D(0.5f, 0.5f))
			, AutoSizeAttr(false)
			, ZOrderAttr(0)
			, SlotTypeAttr(EToggleSlotType::Other)
		{
		}

		FSlot& Offset(const TAttribute<FMargin>& InOffset)
		{
			OffsetAttr = InOffset;
			return *this;
		}

		FSlot& Anchors(const TAttribute<FAnchors>& InAnchors)
		{
			AnchorsAttr = InAnchors;
			return *this;
		}

		FSlot& Alignment(const TAttribute<FVector2D>& InAlignment)
		{
			AlignmentAttr = InAlignment;
			return *this;
		}

		FSlot& AutoSize(const TAttribute<bool>& InAutoSize)
		{
			AutoSizeAttr = InAutoSize;
			return *this;
		}

		FSlot& ZOrder(const TAttribute<float>& InZOrder)
		{
			ZOrderAttr = InZOrder;
			return *this;
		}

		FSlot& Expose(FSlot*& OutVarToInit)
		{
			OutVarToInit = this;
			return *this;
		}

        FSlot& SlotType(const TAttribute<EToggleSlotType>& InSlotType)
        {
			SlotTypeAttr = InSlotType;
			return *this;
        }
    };
public:
	SMyToggle();
    
    SLATE_BEGIN_ARGS(SMyToggle)
		: _IsToggleChecked(ECheckBoxState::Unchecked)
		, _IsFocusable(true)
    {
    }
    SLATE_SUPPORTS_SLOT(SMyToggle::FSlot)
	SLATE_ATTRIBUTE(ECheckBoxState, IsToggleChecked)
	SLATE_ARGUMENT(bool, IsFocusable)
	SLATE_ATTRIBUTE(EButtonClickMethod::Type, ClickMethod)
	SLATE_EVENT(FOnToggleCheckStateChanged, OnToggleCheckStateChanged)
	SLATE_EVENT(FOnGetContent, OnGetMenuContent)
    SLATE_END_ARGS()
    
    void Construct(const FArguments& InArgs);
    static FSlot& Slot()
    {
        return *(new FSlot());
    }
    
    FSlot& AddSlot()
    {
        Invalidate(EInvalidateWidget::Layout);
        SMyToggle::FSlot& slot = *(new FSlot());
        this->Children.Add(&slot);
        return slot;
    }

	void SetToggleIsChecked(TAttribute<ECheckBoxState> InIsToggleChecked);
    
    int32 RemoveSlot(const TSharedRef<SWidget>& SlotWidget);
    void ClearChildren();

	bool IsPressed() const
	{
		return bIsPressed;
	}

	void ToggleCheckedState();
public:
      // Begin SWidget overrides
    virtual void OnArrangeChildren( const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren ) const override;
    virtual int32 OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const override;
    virtual FChildren* GetChildren() override
    {
        return &Children;
    }

	virtual bool SupportsKeyboardFocus() const override;
	virtual FReply OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;
	virtual bool IsInteractable() const override;
    // End SWidget overrides
protected:
    // Begin SWidget overrides.
    virtual FVector2D ComputeDesiredSize(float) const override;
    // End SWidget overrides.
private:
	typedef TArray<bool, TInlineAllocator<16>> FArrangedChildLayers;

	void ArrangeLayeredChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren, FArrangedChildLayers& ArrangedChildLayers) const;
	bool IsSameWithCheckState(const EToggleSlotType& SlotType) const;
protected:
    TPanelChildren<FSlot> Children;

	TAttribute<ECheckBoxState> IsToggleChecked;

	FOnToggleCheckStateChanged OnToggleCheckStateChanged;
	FOnGetContent OnGetMenuContent;
	
	EButtonClickMethod::Type ClickMethod;


	bool bIsFocusable;
	bool bIsPressed;
};
