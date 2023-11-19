// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();	
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetPhysicsHandle() != nullptr) {
		FVector TargetLocation = GetComponentLocation() + GetForwardVector() * HoldDistance;
		GetPhysicsHandle()->SetTargetLocationAndRotation(TargetLocation, GetComponentRotation());
	}
}

void UGrabber::Grab() {
	FHitResult HitResult;
	if (GetGrabberInReach(HitResult)) {
		UPrimitiveComponent* HitComponent = HitResult.GetComponent();
		UE_LOG(LogTemp, Display, TEXT("%s"), *(HitComponent->GetName()));
		HitComponent->WakeAllRigidBodies();
		GetPhysicsHandle()->GrabComponentAtLocationWithRotation(HitComponent, NAME_None, HitResult.ImpactPoint, GetComponentRotation());	
	}
}

void UGrabber::Release() {
	// get physics handle
	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();
	// releases grabbed component if grabbed component is not null 
	if (PhysicsHandle->GetGrabbedComponent() != nullptr) {
		PhysicsHandle->GetGrabbedComponent()->WakeAllRigidBodies();
		PhysicsHandle->ReleaseComponent();
	}
}

UPhysicsHandleComponent* UGrabber::GetPhysicsHandle() const {
	// gets physics handle from Owner
	UPhysicsHandleComponent* PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	// check if Physics Handle is null to avoid crashes
	if (PhysicsHandle == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("No Physics Handler found!"));
	}
	
	return PhysicsHandle;
}

bool UGrabber::GetGrabberInReach(FHitResult& OutHitResult) const {
	FVector Start = GetComponentLocation();
	FVector End = Start + GetForwardVector() * MaxGrabDistance;

	DrawDebugSphere(GetWorld(), End, 10, 10, FColor::Blue, false, 5);

	FCollisionShape Sphere = FCollisionShape::MakeSphere(GrabRadius);
	return GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, ECC_GameTraceChannel2, Sphere);
}