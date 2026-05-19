#include "StressPollerComponent.h"
#include "TimerManager.h"
#include "Json.h"
#include "JsonUtilities.h"

UStressPollerComponent::UStressPollerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    PollInterval = 1.0f;
}

void UStressPollerComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UStressPollerComponent::StartPolling(const FString& URL, float IntervalSeconds)
{
    EndpointURL = URL;
    PollInterval = IntervalSeconds;

    GetWorld()->GetTimerManager().SetTimer(
        PollTimer,
        this,
        &UStressPollerComponent::PollServer,
        PollInterval,
        true
    );
}

void UStressPollerComponent::StopPolling()
{
    GetWorld()->GetTimerManager().ClearTimer(PollTimer);
}

void UStressPollerComponent::PollServer()
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request =
        FHttpModule::Get().CreateRequest();

    Request->SetURL(EndpointURL);
    Request->SetVerb("GET");

    Request->OnProcessRequestComplete().BindUObject(
        this,
        &UStressPollerComponent::OnResponseReceived
    );

    Request->ProcessRequest();
}

void UStressPollerComponent::OnResponseReceived(
    FHttpRequestPtr Request,
    FHttpResponsePtr Response,
    bool bWasSuccessful
)
{
    float StressValue = 0.0f;

    if (bWasSuccessful && Response.IsValid())
    {
        FString Content = Response->GetContentAsString();

        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Content);

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            StressValue = JsonObject->GetNumberField("stress");
        }
        else
        {
            bWasSuccessful = false;
        }
    }
    else
    {
        bWasSuccessful = false;
    }

    OnStressReceived.Broadcast(StressValue, bWasSuccessful);
}