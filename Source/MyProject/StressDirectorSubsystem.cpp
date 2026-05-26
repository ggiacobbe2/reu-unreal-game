#include "StressDirectorSubsystem.h"

void UStressDirectorSubsystem::SetStressValue(float NewStress)
{
    // Clamp input to valid normalized range
    NewStress = FMath::Clamp(NewStress, 0.0f, 1.0f);

    // Early exit if no meaningful change
    if (FMath::IsNearlyEqual(NewStress, StressValue))
    {
        return;
    }

    // Update internal state
    StressValue = NewStress;

    // Broadcast change to all listeners (Blueprint + C++)
    OnStressChanged.Broadcast(StressValue);
}

float UStressDirectorSubsystem::GetStressValue() const
{
    return StressValue;
}