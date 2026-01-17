// Fill out your copyright notice in the Description page of Project Settings.


#include "Tank.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"

ATank::ATank()
{
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(CapsuleComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
}

// Called when the game starts or when spawned
void ATank::BeginPlay()
{
	Super::BeginPlay();
 
	PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController) {

		if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer()) {

			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer)) {
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}

	SetPlayerEnabled(false);

}


// Called every frame
void ATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!PlayerController) return;

	FHitResult HitResult;
	PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
	RotateTurret(HitResult.ImpactPoint);
	//DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 25.0f, 16, FColor::Red);
}

// Called to bind functionality to input
void ATank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent)) 
	{
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATank::MoveInput);
		EIC->BindAction(TurnAction, ETriggerEvent::Triggered, this, &ATank::TurnInput);
		EIC->BindAction(FireAction, ETriggerEvent::Started, this, &ATank::Fire);
	}

}

void ATank::MoveInput(const FInputActionValue& Value)
{
	float InputValue = Value.Get<float>();

	FVector DeltaLocation = FVector(0.0f, 0.0f, 0.0f);
	DeltaLocation.X = Speed * InputValue * UGameplayStatics::GetWorldDeltaSeconds(GetWorld());

	AddActorLocalOffset(DeltaLocation, true);
}

void ATank::TurnInput(const FInputActionValue& Value)
{
	float InputValue = Value.Get<float>();
	
	FRotator DeltaRotation = FRotator(0.0f,0.0f,0.0f);
	DeltaRotation.Yaw = TurnRate * InputValue * GetWorld()->GetDeltaSeconds();

	AddActorLocalRotation(DeltaRotation, true);
}


void ATank::HandleDestruction()
{
	Super::HandleDestruction();
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	SetPlayerEnabled(false);
	IsAlive = false;
}

void ATank::SetPlayerEnabled(bool Enabled)
{
	if (!PlayerController) return;
	PlayerController->bShowMouseCursor = Enabled;
	Enabled ? EnableInput(PlayerController) : DisableInput(PlayerController);
}