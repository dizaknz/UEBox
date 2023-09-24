#include "CaptureActor.h"
#include "CaptureComponent.h"

ACaptureActor::ACaptureActor()
{
    PrimaryActorTick.bCanEverTick = true;

    CaptureComp = CreateDefaultSubobject<UCaptureComponent>(TEXT("Capture Component"));
    CaptureComp->SetupAttachment(RootComponent);
    RootComponent = CaptureComp;
}

void ACaptureActor::BeginPlay()
{
    CaptureBeginEvent.Broadcast();
}

bool ACaptureActor::Initialise(EOutputFormat Format, const FString& Directory)
{
    check(CaptureComp);
    return CaptureComp->Initialise(Format, Directory);
}

void ACaptureActor::Capture(const FVector& Location, const FString& Name)
{
    check(CaptureComp);

    // move to capture location
    SetActorLocation(Location, true, nullptr, ETeleportType::TeleportPhysics);
    // capture panorama    
    CaptureComp->Capture(Name);
}
