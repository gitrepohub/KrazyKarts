// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKart.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "UnrealNetwork.h"


// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// sets actor to replicate
	bReplicates = true;
}

// Called when the game starts or when spawned
void AGoKart::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		NetUpdateFrequency = 1;
	}
	
}


static FString GetEnumText(ENetRole Role)
{
	switch (Role)
	{
	case ROLE_None:
		return "None";
	case ROLE_SimulatedProxy:
		return "ROLE_SimulatedProxy";
	case ROLE_AutonomousProxy:
		return "ROLE_AutonomousProxy";
	case ROLE_Authority:
		return "ROLE_Authority";
	default:
		return "Error";
	}
}


// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled())
	{
		FGoKartMove Move;
		Move.DeltaTime = DeltaTime;
		Move.SteeringThrow = SteeringThrow;
		Move.Throttle = Throttle;
		// todo set time

		Server_SendMove(Move);

		SimulateMove(Move);
	}

	DrawDebugString(GetWorld(), FVector(0.0f, 0.0f, 100.0f), GetEnumText(Role), this, FColor::White, 0);
}


FVector AGoKart::GetAirResistance() const
{

	return - Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient;

}

FVector AGoKart::GetRollingResistance() const
{
	float AccelerationDueToGravity = -GetWorld()->GetGravityZ() / 100;

	float NormalForce = Mass * AccelerationDueToGravity;
	return - Velocity.GetSafeNormal() * RollingResistanceCoefficient * NormalForce;

}


void AGoKart::OnRep_ServerState()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_ServerState"));
	SetActorTransform(ServerState.Transform);
	Velocity = ServerState.Velocity;
}


void AGoKart::ApplyRotation(float DeltaTime, float StreeringThrow)
{
	float DeltaLocation = FVector::DotProduct(GetActorForwardVector(), Velocity) * DeltaTime;
	float RotationAngle = DeltaLocation / MinTurningRadius * StreeringThrow;
	FQuat RotationDelta(GetActorUpVector(), RotationAngle);
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


void AGoKart::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGoKart, ServerState);
}


void AGoKart::SimulateMove(FGoKartMove Move)
{

	FVector Force = GetActorForwardVector() * MaxDrivingForce * Move.Throttle;
	Force += GetAirResistance();
	Force += GetRollingResistance();

	FVector Acceleration = Force / Mass;

	Velocity = Velocity + Acceleration * Move.DeltaTime;

	ApplyRotation(Move.DeltaTime, Move.SteeringThrow);
	UpdateLocationFromVelocity(Move.DeltaTime);

}


void AGoKart::MoveForward(float Val)
{
	Throttle = Val;
}


void AGoKart::MoveRight(float Val)
{
	SteeringThrow = Val;
}


void AGoKart::Server_SendMove_Implementation(FGoKartMove Move)
{

	SimulateMove(Move);

	ServerState.LastMove = Move;
	ServerState.Transform = GetActorTransform();
	ServerState.Velocity = Velocity;

	// remove mouse as it just jump up to silly numbers
	//Velocity = GetActorForwardVector() * 20 * Val; // 20 meters per second
}


bool AGoKart::Server_SendMove_Validate(FGoKartMove Move)
{
	return true; // todo better validation
}