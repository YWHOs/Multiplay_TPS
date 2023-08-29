
#include "JumpPickup.h"
#include "Multiplay_TPS/Character/TPSCharacter.h"
#include "Multiplay_TPS/Components/BuffComponent.h"

void AJumpPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OhterComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OhterComp, OtherBodyIndex, bFromSweep, SweepResult);

	ATPSCharacter* TPSCharacter = Cast<ATPSCharacter>(OtherActor);
	if (TPSCharacter)
	{
		UBuffComponent* buffComponent = TPSCharacter->GetBuffComponent();
		if (buffComponent)
		{
			buffComponent->Jump(jumpBuff, jumpBuffTime);
		}
	}
	Destroy();
}