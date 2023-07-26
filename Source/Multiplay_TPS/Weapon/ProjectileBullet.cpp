// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

void AProjectileBullet::OnHit(UPrimitiveComponent* _HitComp, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, FVector _NormalImpulse, const FHitResult& _Hit)
{
	ACharacter* character = Cast<ACharacter>(GetOwner());
	if (character)
	{
		AController* controller = character->Controller;
		if (controller)
		{
			UGameplayStatics::ApplyDamage(_OtherActor, damage, controller, this, UDamageType::StaticClass());
		}
	}
	
	Super::OnHit(_HitComp, _OtherActor, _OtherComp, _NormalImpulse, _Hit);
}