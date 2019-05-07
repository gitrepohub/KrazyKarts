// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKart.generated.h"

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

	void MoveForward(float Val);
	void MoveRight(float Val);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveForward(float Val);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveRight(float Val);

	void UpdateLocationFromVelocity(float DeltaTime);
	void ApplyRotation(float DeltaTime);
	FVector GetAirResistance() const;
	FVector GetRollingResistance() const;



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

	UPROPERTY(Replicated)
	FVector Velocity = FVector(0);

	UPROPERTY(ReplicatedUsing= OnRep_ReplicatedTransform)
	FTransform ReplicatedTransform;

	UFUNCTION()
	void OnRep_ReplicatedTransform();

	UPROPERTY(Replicated)
	float Throttle = 0.0f;

	UPROPERTY(Replicated)
	float SteeringThrow = 0.0f;

};
