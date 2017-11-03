// Fill out your copyright notice in the Description page of Project Settings.

#include "VRPawn.h"
#include "IHeadMountedDisplay.h"
#include "MotionControllerComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"



// Sets default values
AVRPawn::AVRPawn(const class FObjectInitializer & PCIP) : Super(PCIP)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	enableGravity = false;

	oculusLocationOffset = FVector(0.f, 0.f, 150.f);
	
	// set BaseEyeHeight => attribut class parent Pawn
	this->BaseEyeHeight = 0.f;

	// Add SceneComponent for headset positioning, set to -110 to ensure headset start at floor
	RootComponent = PCIP.CreateDefaultSubobject<USceneComponent>(this, TEXT("SceneRoot"));
	scene = PCIP.CreateDefaultSubobject<USceneComponent>(this, TEXT("VRBaseScene"));
	scene->SetRelativeLocation(FVector(0.f, 0.f, -110.f));
	scene->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	// Add CameraComponent 
	camera = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("Camera"));
	camera->AttachToComponent(scene, FAttachmentTransformRules::KeepRelativeTransform);
	camera->SetFieldOfView(110.f);

	// Add CapsuleCollision, set default VR half height and radius values
	capsuleCollision = PCIP.CreateDefaultSubobject<UCapsuleComponent>(this, TEXT("CapsuleComponent"));
	capsuleCollision->SetCapsuleHalfHeight(96.f);
	capsuleCollision->SetCachedMaxDrawDistance(22.f);
	capsuleCollision->SetRelativeLocation(FVector(0.f, 0.f, -110.f));
	capsuleCollision->AttachToComponent(camera, FAttachmentTransformRules::KeepRelativeTransform);

	// Add MotionControllerLeft, set default 
	motionControllerLeft = PCIP.CreateDefaultSubobject<UMotionControllerComponent>(this, TEXT("MotionController_Left"));
	motionControllerLeft->Hand = EControllerHand::Left;
	motionControllerLeft->AttachToComponent(scene, FAttachmentTransformRules::KeepRelativeTransform);
	motionControllerLeft->SetRelativeLocation(FVector(0.f, 0.f, 110.f));

	// Add MotionControllerRight
	motionControllerRight = PCIP.CreateDefaultSubobject<UMotionControllerComponent>(this, TEXT("MotionController_Right"));
	motionControllerRight->Hand = EControllerHand::Right;
	motionControllerRight->AttachToComponent(scene, FAttachmentTransformRules::KeepRelativeTransform);
	motionControllerRight->SetRelativeLocation(FVector(0.f, 0.f, 110.f));
}

void AVRPawn::overridePawnValues(float PawnBaseEyeHeight, float FOV, float CapsuleHalfHeight, float CapsuleRadius, FVector CapsuleRelativeLocation, FVector SceneLocation, FVector LeftControllerLocation, FVector RightControllerLocation)
{
	//set Pawn Base Height
	this->BaseEyeHeight = PawnBaseEyeHeight;
	
	//set Camera FOV
	camera->SetFieldOfView(FOV);

	//set Capsule Collision
	capsuleCollision->SetCapsuleHalfHeight(CapsuleHalfHeight);
	capsuleCollision->SetCapsuleRadius(CapsuleRadius);
	capsuleCollision->SetRelativeLocation(CapsuleRelativeLocation);

	//set Scene location
	scene->SetRelativeLocation(SceneLocation);

	//set MotionController
	motionControllerLeft->SetRelativeLocation(LeftControllerLocation);
	motionControllerRight->SetRelativeLocation(RightControllerLocation);
}

// Pawn Rotation - useful for static mouse rotation during developement
void AVRPawn::rotatePawn(float RotationRate, float XAxisInput, float YAxisIntput)
{
	if (XAxisInput != 0.f)
		this->AddActorLocalRotation(FRotator(0.f, XAxisInput*RotationRate, 0.f));

	if (YAxisIntput != 0.f)
		this->AddActorLocalRotation(FRotator(0.f, YAxisIntput*RotationRate, 0.f));
}

// check if HMD is worn
bool AVRPawn::isHMDWorn()
{
	if (GEngine->HMDDevice.IsValid()) {
		if (GEngine->HMDDevice->GetHMDWornState() == EHMDWornState::Worn) {
			return true;
		}
			
	}

	return false;
}

// print Debug Message
void AVRPawn::printDebugMessage(FString Message, bool OverWriteExisting, float Duration, FColor Color)
{
	int32 key;
	if (OverWriteExisting) {
		key = 0;
	}
	else {
		key = 1;
	}

	GEngine->AddOnScreenDebugMessage(key, Duration, Color, Message);
}



// Called when the game starts or when spawned
void AVRPawn::BeginPlay()
{
	Super::BeginPlay();

	// Ajust Pawn spawn target offset based on HMD
	if (GEngine->HMDDevice.IsValid()) {

		//Override height offset for Occulus Rift
		switch (GEngine->HMDDevice->GetHMDDeviceType())
		{
		case EHMDDeviceType::DT_OculusRift:
			this->SetActorLocation(this->GetActorLocation() + oculusLocationOffset);
			GEngine->HMDDevice->SetTrackingOrigin(EHMDTrackingOrigin::Floor);
			break;
		default:
			break;
		}

		// set tracking origin ( Occulus & Vive )
		GEngine->HMDDevice->SetTrackingOrigin(EHMDTrackingOrigin::Floor);

	}

	
	
}

// Called every frame
void AVRPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Get Current Position of Camera
	FVector cameraPosition = camera->GetComponentTransform().GetLocation();

	//Apply Gravity if enabled and camera is positioned at head of player
	if (enableGravity &&camera->IsValidLowLevel() && cameraPosition.Z > this->GetActorLocation().Z) {
		
		// set line trace for gravity variable
		FHitResult RayHit(EForceInit::ForceInit);
		FCollisionQueryParams RayTraceParams(FName(TEXT("GravityRayTrace")), true, this->GetOwner());

		// initialyze Gravity Trace Hit Result var
		RayTraceParams.bTraceComplex = true;
		RayTraceParams.bTraceAsyncScene = true;
		RayTraceParams.bReturnPhysicalMaterial = false;

		HitResult = GetWorld()->LineTraceSingleByChannel(RayHit
			,cameraPosition
			,cameraPosition + FVector(0.f, 0.f, FMath::Abs(gravityVariable.floorTraceRange*-1.f))
			,ECollisionChannel::ECC_Visibility, RayTraceParams);

		// check if we need to float the Pawn over uneven terrain
		if (gravityVariable.respondToUneveTerrain &&
			HitResult &&
			RayHit.GetComponent()->CanCharacterStepUpOn == ECanBeCharacterBase::ECB_Yes &&
			(RayHit.Distance+gravityVariable.floorTraceTolerance)<gravityVariable.floorTraceRange) 
		{
			this->TeleportTo(this->GetActorLocation() + FVector(0.f, 0.f, gravityVariable.floorTraceRange - RayHit.Distance),
				this->GetActorRotation());
		}
		
		//apply gravity

		if (HitResult ||
			RayHit.GetComponent()->CanCharacterStepUpOn != ECanBeCharacterBase::ECB_Yes)
		{
			this->TeleportTo(this->GetActorLocation() + (gravityVariable.gravityDirection * gravityVariable.gravityStrength),
				this->GetActorRotation());
		}

	}

}

// Called to bind functionality to input
void AVRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

