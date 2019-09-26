// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInputConstants.h"
#include "NameTypes.h"

// Constants for Inputs to avoid magic strings

// TODO something better than static FNames?

// Misc
const FName GameInputConstants::CONFIRM = FName("Confirm");

// Build Move Keyboard
const FName GameInputConstants::MOVE_UP = FName("MoveUp");
const FName GameInputConstants::MOVE_DOWN = FName("MoveDown");
const FName GameInputConstants::MOVE_LEFT = FName("MoveLeft");
const FName GameInputConstants::MOVE_RIGHT = FName("MoveRight");
const FName GameInputConstants::MOVE_ASCEND = FName("MoveAscend");
const FName GameInputConstants::MOVE_DESCEND = FName("MoveDescend");

// Build Rotate Keyboard
const FName GameInputConstants::ROTATE_C = FName("RotateC");
const FName GameInputConstants::ROTATE_CC = FName("RotateCC");

// Build Cycle Piece
const FName GameInputConstants::CYCLE_PIECE_LEFT = FName("CyclePieceLeft");
const FName GameInputConstants::CYCLE_PIECE_RIGHT = FName("CyclePieceRight");

// Axis
const FName GameInputConstants::X_CAMERA_AXIS = FName("XCameraAxis");
const FName GameInputConstants::Y_CAMERA_AXIS = FName("YCameraAxis");

const FName GameInputConstants::X_MOVE_AXIS = FName("XMoveAxis");
const FName GameInputConstants::Y_MOVE_AXIS = FName("YMoveAxis");
const FName GameInputConstants::Z_MOVE_AXIS = FName("ZMoveAxis");

const FName GameInputConstants::ROTATE_MOVE_AXIS = FName("RotateMoveAxis");



