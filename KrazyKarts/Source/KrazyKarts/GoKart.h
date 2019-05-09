// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKart.generated.h"


USTRUCT()
struct FGoKartMove
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	float Throttle = 0.0f;

	UPROPERTY()
	float SteeringThrow = 0.0f;

	UPROPERTY()
	float DeltaTime; // tick time

	UPROPERTY()
	float Time; // time stamp
};


USTRUCT()
struct FGoKartState
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FVector Velocity = FVector(0);

	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	FGoKartMove LastMove;
};


UCLASS()
class KRAZYKARTS_API AGoKart : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGoKart();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void SimulateMove(FGoKartMove Move);

	void MoveForward(float Val);
	void MoveRight(float Val);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FGoKartMove Move);


	void UpdateLocationFromVelocity(float DeltaTime);
	void ApplyRotation(float DeltaTime, float StreeringThrow);
	FVector GetAirResistance() const;
	FVector GetRollingResistance() const;

	FVector Velocity = FVector(0);
	float Throttle = 0.0f;
	float SteeringThrow = 0.0f;

	//The mass of the car (kg)
	UPROPERTY(EditAnywhere)
	float Mass = 1000;

	// The force applied to the car when the throttle is fully down (N)
	UPROPERTY(EditAnywhere)
    float MaxDrivingForce = 10000;

	// The number of degrees rotated per second at full control throw (degrees/s)
	//UPROPERTY(EditAnywhere)
	//float MaxDegreesPerSecond = 90;

	// heigher is more drag (kgpermeter)
	UPROPERTY(EditAnyWhere)
	float DragCoefficient = 16;

	// heigher is more rolling resistance (kgpermeter)
	UPROPERTY(EditAnyWhere)
	float RollingResistanceCoefficient = 0.015;

	// Minimum Radius of the car turning circle at full lock (m)
	UPROPERTY(EditAnyWhere)
	float MinTurningRadius = 10;

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FGoKartState ServerState;

	UFUNCTION()
	void OnRep_ServerState();

};
