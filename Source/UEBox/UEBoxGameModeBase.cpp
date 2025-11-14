#include "UEBoxGameModeBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "NativeGameplayTags.h"

void AUEBoxGameModeBase::BeginPlay()
{
    UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Event_Explode, "Event.Explode");
}

