#include "HGameMode.h"

#include "FMODStudioModule.h"
#include "Kismet/KismetSystemLibrary.h"
#include "fmod_studio.hpp"

/*
 * Console command to enable debug draw visualizations
 * Open the console with the (`) key
 * Project.Audio.EnableDebug 1 (To enable)
 * Project.Audio.EnableDebug 0 (To disable)
 */
FAutoConsoleVariable CVarEnableAudioDebug(TEXT("Project.Audio.EnableDebug"),false,TEXT("Enables audio debug visualizations"),ECVF_Default);


void AHGameMode::SetupAudioListenerCPP(APlayerController* PlayerController, const bool bOverrideAttenuation, const FName AttenuationComponentTag)
{
	if (!IsValid(PlayerController))
	{
		return;
	}

	if (!bOverrideAttenuation || AttenuationComponentTag.IsNone())
	{
		PlayerController->ClearAudioListenerAttenuationOverride();
		PlayerController->ClearAudioListenerOverride();
		return;
	}

	if (const APawn* ControlledPawn = PlayerController->GetPawn())
	{
		const TArray<UActorComponent*> SceneComponents = ControlledPawn->GetComponentsByTag(USceneComponent::StaticClass(), AttenuationComponentTag);

		if (SceneComponents.IsEmpty())
		{
			return;
		}

		USceneComponent* CameraTransformComponent = PlayerController->PlayerCameraManager->GetTransformComponent();
		PlayerController->SetAudioListenerOverride(CameraTransformComponent, FVector::ZeroVector, FRotator::ZeroRotator);
		PlayerController->SetAudioListenerAttenuationOverride(CastChecked<USceneComponent>(SceneComponents[0]), FVector::ZeroVector);
	}
}

void AHGameMode::DrawAudioListenerDebugCPP() const
{
	// Only draw if the console variable is set to true
	if (!CVarEnableAudioDebug->GetBool())
	{
		return;
	}

	// Get the FMOD Studio system from using its module interface
	const FMOD::Studio::System* StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);

	FMOD_3D_ATTRIBUTES Listener3DAttributes;
	FMOD_VECTOR FMODAttenuationLocation;
	constexpr int ListenerIndex = 0; // We only care about the first listener;
	StudioSystem->getListenerAttributes(ListenerIndex, &Listener3DAttributes, &FMODAttenuationLocation);

	// FMOD and UE use a different vector axis matrix and scale, so it's essential to convert them here:
	constexpr float FMOD_TO_UE_SCALAR = 100.f;
	const FMOD_VECTOR FMODListenerLocation = Listener3DAttributes.position;

	const FVector UnrealListenerLocation = FVector(FMODListenerLocation.z, FMODListenerLocation.x, FMODListenerLocation.y) * FMOD_TO_UE_SCALAR;
	const FVector UnrealAttenuationLocation = FVector(FMODAttenuationLocation.z, FMODAttenuationLocation.x, FMODAttenuationLocation.y) * FMOD_TO_UE_SCALAR;

	UKismetSystemLibrary::DrawDebugSphere(this, UnrealListenerLocation, 20, 10, FLinearColor::White, 0.0, 0.5);
	UKismetSystemLibrary::DrawDebugSphere(this, UnrealAttenuationLocation, 18, 8, FLinearColor::Green, 0.0, 1);

	// NOTE:
	// The same locations can be retrieved by using PlayerController::GetAudioListenerPosition()
	// and PlayerController::GetAudioListenerAttenuationOverridePosition();
	// However, I wanted to prove that the FMOD Audio engine has the accurate locations.
}
