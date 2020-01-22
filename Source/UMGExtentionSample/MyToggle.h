// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Components/PanelWidget.h"
#include "SlateCore/Public/Styling/SlateTypes.h"
#include "SMyToggle.h"
#include "MyToggle.generated.h"

class SMyToggle;
class UMyToggleSlot;
class SWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnToggleStateChanged, ECheckBoxState, LastState, ECheckBoxState, NewState);
/**
 * 
 */
UCLASS()
class UMGEXTENTIONSAMPLE_API UMyToggle : public UPanelWidget
{
	GENERATED_UCLASS_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance")
    ECheckBoxState CheckedState;    

	UPROPERTY()
	FGetCheckBoxState CheckedStateDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool IsFocusable;

	UPROPERTY(BlueprintAssignable, Category = "Toggle|Event")
	FOnToggleStateChanged OnToggleCheckStateChanged;

public:
    // Begin UVisual Interface
    virtual void ReleaseSlateResources(bool bReleaseChildren) override;
    // End UVisual Interface
    
    // Begin UWidget
    virtual void SynchronizeProperties() override;
#if WITH_EDITOR
    virtual const FText GetPaletteCategory() override;
#endif
    // End UWidget
    
	TSharedPtr<SMyToggle> GetToggleWidget()const;
	bool GetGeometryForSlot(UMyToggleSlot* InSlot, FGeometry& ArrangedGeometry) const;
protected:
    // Begin UWidget
    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual void OnWidgetRebuilt() override;
    // End UWidget
    
    // Begin UPanelWidget
    virtual UClass* GetSlotClass() const override;
    virtual void OnSlotAdded(UPanelSlot* InSlot) override;
    virtual void OnSlotRemoved(UPanelSlot* InSlot) override;
    // End UPanelWidget

	void SlateOnToggleCheckeStateChanged(ECheckBoxState NewState);
	
protected:
	TSharedPtr<SMyToggle> MyToggle;

	PROPERTY_BINDING_IMPLEMENTATION(ECheckBoxState, CheckedState)
};
