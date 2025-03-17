#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "HGameMode.generated.h"

UCLASS()
class AULISTENERUEFMOD_API AHGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetupAudioListenerCPP(APlayerController* PlayerController, bool bOverrideAttenuation, FName AttenuationComponentTag = "AttenuationOverride");

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void DrawAudioListenerDebugCPP() const;
};
