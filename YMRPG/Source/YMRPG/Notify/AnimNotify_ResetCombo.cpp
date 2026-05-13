#include "AnimNotify_ResetCombo.h"
#include "YMRPGCharacterBase.h"
#include "YMRPGComboComponent.h"

UAnimNotify_ResetCombo::UAnimNotify_ResetCombo()
{
}

FString UAnimNotify_ResetCombo::GetNotifyName_Implementation() const
{
    return Super::GetNotifyName_Implementation();
}

void UAnimNotify_ResetCombo::Notify(
    USkeletalMeshComponent* MeshComp,
    UAnimSequenceBase* Animation,
    const FAnimNotifyEventReference& EventReference
)
{
    Super::Notify(MeshComp, Animation, EventReference);

    if(AYMRPGCharacterBase* Character = Cast<AYMRPGCharacterBase>(MeshComp->GetOuter()))
    {
        if(UYMRPGComboComponent* ComboComponent = Character->FindComponentByClass<UYMRPGComboComponent>())
        {
            ComboComponent->ResetComboIndex();
        }
	}
}
