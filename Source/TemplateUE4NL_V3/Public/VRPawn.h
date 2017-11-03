// Fill out your copyright notice in the Description page of Project Settings.

//Gnu general public license version 3
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :
//
//The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
//OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
//LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
//IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "VRPawn.generated.h"

//Forward Declaration
class UCapsuleComponent;
class UCameraComponent;
class UMotionControllerComponent;

// Gravity Setting
USTRUCT(BlueprintType)
struct  FGravityProperty
{
	GENERATED_USTRUCT_BODY()

		// respond to uneven terrain - gravity Must alse be enabled
		UPROPERTY(EditAnywhere, Category = "VR")
		bool respondToUneveTerrain = false;

		// how fast this VRPawn will fall with gravity
		UPROPERTY(EditAnywhere, Category = "VR")
		float gravityStrength = 3.f;

		// how far shoul the check for a floor
		UPROPERTY(EditAnywhere, Category = "VR")
		float floorTraceRange = 150.f;
		
		// min z offset before terrain is considered uneven
		UPROPERTY(EditAnywhere, Category = "VR")
		float floorTraceTolerance = 3.f;

		// direction where this VRPawn will fall
		UPROPERTY(EditAnywhere, Category = "VR")
		FVector gravityDirection = FVector(0.f, 0.f, 0.f);

};

UCLASS()
class TEMPLATEUE4NL_V3_API AVRPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AVRPawn(const class FObjectInitializer & PCIP);
	
	//Properties

	// Enable Gravity for this pawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	bool enableGravity;

	// gravity variable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	FGravityProperty gravityVariable;

	//  Oculus HMD Location offset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	FVector oculusLocationOffset;

	// Capsule Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
	UCapsuleComponent *capsuleCollision;

	// Scene Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
	USceneComponent *scene;

	// Pawn Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
	UCameraComponent *camera;

	//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
	UMotionControllerComponent *motionControllerLeft = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
	UMotionControllerComponent *motionControllerRight = nullptr;

	//TODO Set override method default pawn VR

	//Override default pawn vr values
	UFUNCTION(BlueprintCallable, Category = "VR")
		void overridePawnValues(float PawnBaseEyeHeight = 0.f, float FOV = 110.f, float CapsuleHalfHeight = 96.f, float CapsuleRadius = 22.f,
			FVector CapsuleRelativeLocation = FVector(0.f, 0.f, -110.f),
			FVector SceneLocation = FVector(0.f, 0.f, -110.f),
			FVector LeftControllerLocation = FVector(0.f, 0.f, 110.f),
			FVector RightControllerLocation = FVector(0.f, 0.f, 110.f));

	//PawnRotation - Usefull for static mouse rotation durinf devellopement
	UFUNCTION(BlueprintCallable, Category = "VR")
		void rotatePawn(float RotationRate, float XAxisInput, float YAxisIntput);

	// check if HMD is worn
	UFUNCTION(BlueprintCallable, Category = "VR")
		bool isHMDWorn();

	//print debug Message
	UFUNCTION(BlueprintCallable, Category = "VR")
		void printDebugMessage(FString Message, bool OverWriteExisting, float Duration, FColor Color);




protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	bool HitResult;
	
};
