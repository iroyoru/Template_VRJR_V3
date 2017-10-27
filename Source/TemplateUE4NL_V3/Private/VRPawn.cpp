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

}

// Called to bind functionality to input
void AVRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

