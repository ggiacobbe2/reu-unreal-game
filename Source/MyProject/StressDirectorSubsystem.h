#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "StressDirectorSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStressChanged, float, StressValue);

/**
 * Central runtime system for normalized stress signal distribution.
 * Receives stress input and broadcasts to gameplay systems.
 */
UCLASS()
class MYPROJECT_API UStressDirectorSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    // Sets the current normalized stress value (0–1)
    UFUNCTION(BlueprintCallable, Category = "Stress")
    void SetStressValue(float NewStress);

    // Returns current stress value
    UFUNCTION(BlueprintPure, Category = "Stress")
    float GetStressValue() const;

    // Event fired whenever stress changes
    UPROPERTY(BlueprintAssignable, Category = "Stress")
    FOnStressChanged OnStressChanged;

private:

    // Internal cached stress value
    float StressValue = 0.0f;
};