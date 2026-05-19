#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Http.h"
#include "StressPollerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnStressReceived,
    float, StressValue,
    bool, bSuccess
);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYPROJECT_API UStressPollerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UStressPollerComponent();

    UFUNCTION(BlueprintCallable, Category="Stress")
    void StartPolling(const FString& URL, float IntervalSeconds);

    UFUNCTION(BlueprintCallable, Category="Stress")
    void StopPolling();

    UPROPERTY(BlueprintAssignable, Category="Stress")
    FOnStressReceived OnStressReceived;

protected:
    virtual void BeginPlay() override;

private:
    void PollServer();
    void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

    FTimerHandle PollTimer;

    FString EndpointURL;
    float PollInterval;
};