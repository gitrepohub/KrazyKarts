// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKart.h"
#include "Components/InputComponent.h"
#include "WheeledVehicle.h"


// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGoKart::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Force = GetActorForwardVector() * MaxDrivingForce * Throttle;

	FVector Acceleration = Force / Mass;

	Velocity = Velocity + Acceleration * DeltaTime;

	ApplyRotation(DeltaTime);
	UpdateLocationFromVelocity(DeltaTime);

}

void AGoKart::ApplyRotation(float DeltaTime)
{
	float RotationAngle = MaxDegreesPerSecond * DeltaTime * SteeringThrow;
	FQuat RotationDelta(GetActorUpVector(), FMath::DegreesToRadians(RotationAngle));
	Velocity = RotationDelta.RotateVector(Velocity);
	AddActorWorldRotation(RotationDelta);
}


void AGoKart::UpdateLocationFromVelocity(float DeltaTime)
{
	FVector Translation = Velocity * DeltaTime * 100; // centimeters

	FHitResult Result;
	AddActorWorldOffset(Translation, true, &Result);
	if (Result.bBlockingHit)
	{
		Velocity = FVector::ZeroVector;
		Throttle = 0;
	}
}


// Called to bind functionality to input
void AGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AGoKart::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGoKart::MoveRight);


}


void AGoKart::MoveForward(float Val)
{
	Throttle = Val;
	// remove mouse as it just jump up to silly numbers
	//Velocity = GetActorForwardVector() * 20 * Val; // 20 meters per second
}

void AGoKart::MoveRight(float Val)
{
	SteeringThrow = Val;
}