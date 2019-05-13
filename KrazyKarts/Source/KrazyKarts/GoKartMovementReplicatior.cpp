// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKartMovementReplicatior.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "UnrealNetwork.h"

// Sets default values for this component's properties
UGoKartMovementReplicatior::UGoKartMovementReplicatior()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);

	// ...
}


// Called when the game starts
void UGoKartMovementReplicatior::BeginPlay()
{
	Super::BeginPlay();

	// ...
	MovementComponent = GetOwner()->FindComponentByClass<UGoKartMovementComponent>();
	
}


// Called every frame
void UGoKartMovementReplicatior::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	if (MovementComponent == nullptr) return;

	if (GetOwnerRole() == ROLE_AutonomousProxy) // client
	{
		FGoKartMove Move = MovementComponent->CreateMove(DeltaTime);
		MovementComponent->SimulateMove(Move);
		Server_SendMove(Move);
	}

	// We are the server and in control of the pawn
	if (GetOwnerRole() == ROLE_Authority && GetOwner()->GetRemoteRole() == ROLE_SimulatedProxy)
	{
		FGoKartMove Move = MovementComponent->CreateMove(DeltaTime);
		Server_SendMove(Move);
	}

	// update server moves on client
	if (GetOwnerRole() == ROLE_SimulatedProxy)
	{
		MovementComponent->SimulateMove(GetLastMove());
	}

}


void UGoKartMovementReplicatior::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UGoKartMovementReplicatior, ServerState);
}


void UGoKartMovementReplicatior::OnRep_ServerState()
{
	if (MovementComponent == nullptr) return;

	GetOwner()->SetActorTransform(ServerState.Tranform);
	MovementComponent->SetVelocity(ServerState.Velocity);

	ClearAcknowledgeMoves(ServerState.LastMove);

	for (const FGoKartMove& Move : UnacknowledgedMoves)
	{
		MovementComponent->SimulateMove(Move);
	}
}


void UGoKartMovementReplicatior::ClearAcknowledgeMoves(const FGoKartMove& LastMove)
{
	TArray<FGoKartMove> NewMoves;

	for (const FGoKartMove& Move : UnacknowledgedMoves)
	{
		if (Move.Time > LastMove.Time)
		{
			NewMoves.Add(Move);
		}
	}

	UnacknowledgedMoves = NewMoves;
}


void UGoKartMovementReplicatior::Server_SendMove_Implementation(FGoKartMove Move)
{
	if (MovementComponent == nullptr) return;

	MovementComponent->SimulateMove(Move);

	FGoKartState ServerState;
	ServerState.LastMove = Move;
	ServerState.Tranform = GetOwner()->GetActorTransform();
	ServerState.Velocity = MovementComponent->GetVelocity();

	SetServerState(ServerState);
}


bool UGoKartMovementReplicatior::Server_SendMove_Validate(FGoKartMove Move)
{
	return true; //TODO: Make better validation
}