#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"

#include "UMGExtensionDefine.generated.h"

UENUM(BlueprintType)
enum class EToggleSlotType : uint8
{
	/** Unchecked */
	Unchecked,
	/** Checked */
	Checked,
	/** Neither checked nor unchecked */
	Undetermined,
	/** Neither checked nor unchecked */
	Other,
};