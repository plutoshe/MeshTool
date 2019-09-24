// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/PlayerControllerBase.h"

APlayerControllerBase::APlayerControllerBase()
{
	// There is a bug regarding axis input for mouse cursor if you enable bShowMouseCursor, so it should be turned off
	bShowMouseCursor = false;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}
