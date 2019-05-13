// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GoKartMovementComponent.h"
#include "GoKartMovementReplicatior.generated.h"

USTRUCT()
struct FGoKartState
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FTransform Tranform;

	UPROPERTY()
	FVector Velocity;

	UPROPERTY()
	FGoKartMove LastMove;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAZYKARTS_API UGoKartMovementReplicatior : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGoKartMovementReplicatior();
	FGoKartMove GetLastMove() const { return ServerState.LastMove; }
	void SetServerState(FGoKartState InServerState) { ServerState = InServerState;  }

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FGoKartMove Move);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void ClearAcknowledgeMoves(const FGoKartMove& LastMove);

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FGoKartState ServerState;

	UFUNCTION()
	void OnRep_ServerState();

	TArray<FGoKartMove> UnacknowledgedMoves;

	UPROPERTY()
	UGoKartMovementComponent* MovementComponent = nullptr;

};
