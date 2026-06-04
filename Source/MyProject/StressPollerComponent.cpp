#include "StressPollerComponent.h"
#include "TimerManager.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
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
    UE_LOG(LogTemp, Warning, TEXT("StartPolling CALLED"));

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
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(PollTimer);
    }
}

void UStressPollerComponent::PollServer()
{
    UE_LOG(LogTemp, Warning, TEXT("PollServer CALLED"));

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request =
        FHttpModule::Get().CreateRequest();

    Request->SetURL(EndpointURL);
    Request->SetVerb(TEXT("GET"));

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
    UE_LOG(LogTemp, Warning, TEXT("HTTP CALLBACK FIRED"));

    float StressValue = 0.0f;

    if (bWasSuccessful && Response.IsValid())
    {
        FString Content = Response->GetContentAsString();
        UE_LOG(LogTemp, Warning, TEXT("RAW RESPONSE: %s"), *Content);

        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Content);

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            StressValue = JsonObject->GetNumberField(TEXT("stress"));
            UE_LOG(LogTemp, Warning, TEXT("PARSED STRESS VALUE: %f"), StressValue);
        }
    }

    FStressData Data;
    Data.Stress = StressValue;
    Data.bSuccess = bWasSuccessful;
    UE_LOG(LogTemp, Warning, TEXT("BROADCASTING STRUCT: %f"), Data.Stress);
    UE_LOG(LogTemp, Warning, TEXT("[INSTANCE] this=%p IsTemplate=%d Listeners=%d"),
        this, (int32)IsTemplate(), OnStressReceived.GetAllObjects().Num());
    OnStressReceived.Broadcast(Data);

}
