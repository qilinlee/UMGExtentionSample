// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "Layout/Margin.h"
#include "Layout/Geometry.h"
#include "Widgets/Layout/Anchors.h"
#include "SMyToggle.h"
#include "Components/PanelSlot.h"
#include "Components/CanvasPanelSlot.h"
#include "UMGExtensionDefine.h"
#include "MyToggleSlot.generated.h"


class SMyToggle;
class SMyToggle::FSlot;



UCLASS()
class UMGEXTENTIONSAMPLE_API UMyToggleSlot : public UPanelSlot
{
	GENERATED_UCLASS_BODY()

public:

	/** The anchoring information for the slot */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Layout|Toggle Slot")
		FAnchorData LayoutData;

	/** When AutoSize is true we use the widget's desired size */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Layout|Toggle Slot", AdvancedDisplay, meta = (DisplayName = "Size To Content"))
		bool bAutoSize;

	/** The order priority this widget is rendered in.  Higher values are rendered last (and so they will appear to be on top). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Layout|Toggle Slot")
		int32 ZOrder;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Layout|Toggle Slot")
		EToggleSlotType SlotType;
public:
#if WITH_EDITOR
	virtual bool NudgeByDesigner(const FVector2D& NudgeDirection, const TOptional<int32>& GridSnapSize) override;
	virtual bool DragDropPreviewByDesigner(const FVector2D& LocalCursorPosition, const TOptional<int32>& XGridSnapSize, const TOptional<int32>& YGridSnapSize) override;
	virtual void SynchronizeFromTemplate(const UPanelSlot* const TemplateSlot) override;
#endif //WITH_EDITOR

	/** Sets the layout data of the slot */
	UFUNCTION(BlueprintCallable, Category = "Layout|Toggle Slot")
		void SetLayout(const FAnchorData& InLayoutData);

	/** Gets the layout data of the slot */
	UFUNCTION(BlueprintCallable, Category = "Layout|Toggle Slot")
		FAnchorData GetLayout() const;

	/** Sets the position of the slot */
	UFUNCTION(BlueprintCallable, Category = "Layout|Toggle Slot")
		void SetPosition(FVector2D InPosition);

	/** Gets the position of the slot */
	UFUNCTION(BlueprintCallable, Category = "Layout|Toggle Slot")
		FVector2D GetPosition() const;

	/** Sets the size of the slot */
	UFUNCTION(BlueprintCallable, Category = "Layout|Toggle Slot")
		void SetSize(FVector2D InSize);

	/** Gets the size of the slot */
	UFUNCTION(BlueprintCallable, Category = "Layout|Toggle Slot")
		FVector2D GetSize() const;

	/** Sets the offset data of the slot, which could be position and size, or margins depending on the anchor points */
	UFUNCTION(BlueprintCallable, Category = "Layout|Toggle Slot")
		void SetOffsets(FMargin InOffset);

	/** Gets the offset data of the slot, which could be position and size, or margins depending on the anchor points */
	UFUNCTION(BlueprintCallable, Category = "Layout|Toggle Slot")
		FMargin GetOffsets() const;

	/** Sets the anchors on the slot */
	UFUNCTION(BlueprintCallable, Category = "Layout|Toggle Slot")
		void SetAnchors(FAnchors InAnchors);

	/** Gets the anchors on the slot */
	UFUNCTION(BlueprintCallable, Category = "Layout|Toggle Slot")
		FAnchors GetAnchors() const;

	/** Sets the alignment on the slot */
	UFUNCTION(BlueprintCallable, Category = "Layout|Toggle Slot")
		void SetAlignment(FVector2D InAlignment);

	/** Gets the alignment on the slot */
	UFUNCTION(BlueprintCallable, Category = "Layout|Toggle Slot")
		FVector2D GetAlignment() const;

	/** Sets if the slot to be auto-sized */
	UFUNCTION(BlueprintCallable, Category = "Layout|Toggle Slot")
		void SetAutoSize(bool InbAutoSize);

	/** Gets if the slot to be auto-sized */
	UFUNCTION(BlueprintCallable, Category = "Layout|Toggle Slot")
		bool GetAutoSize() const;

	/** Sets the z-order on the slot */
	UFUNCTION(BlueprintCallable, Category = "Layout|Toggle Slot")
		void SetZOrder(int32 InZOrder);

	/** Gets the z-order on the slot */
	UFUNCTION(BlueprintCallable, Category = "Layout|Toggle Slot")
		int32 GetZOrder() const;

	UFUNCTION(BlueprintCallable, Category = "Layout|Toggle Slot")
		void SetSlotType(EToggleSlotType InType);

	UFUNCTION(BlueprintCallable, Category = "Layout|Toggle Slot")
		EToggleSlotType GetSlotType() const;

public:

	/** Sets the anchors on the slot */
	UFUNCTION()
		void SetMinimum(FVector2D InMinimumAnchors);

	/** Sets the anchors on the slot */
	UFUNCTION()
		void SetMaximum(FVector2D InMaximumAnchors);

public:

	void BuildSlot(TSharedRef<SMyToggle> Canvas);

	// UPanelSlot interface
	virtual void SynchronizeProperties() override;
	// End of UPanelSlot interface

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

#if WITH_EDITOR
	// UObject interface
	using Super::PreEditChange;
	virtual void PreEditChange(UProperty* PropertyThatWillChange) override;
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
	// End of UObject interface

	/** Stores the current layout information about the slot and parent canvas. */
	void SaveBaseLayout();

	void SetDesiredPosition(FVector2D InPosition);

	/** Compares the saved base layout against the current state.  Updates the necessary properties to maintain a stable position. */
	void RebaseLayout(bool PreserveSize = true);
#endif

private:
	SMyToggle::FSlot* Slot;

#if WITH_EDITORONLY_DATA
	FGeometry PreEditGeometry;
	FAnchorData PreEditLayoutData;

	TOptional<FVector2D> DesiredPosition;
#endif
};
