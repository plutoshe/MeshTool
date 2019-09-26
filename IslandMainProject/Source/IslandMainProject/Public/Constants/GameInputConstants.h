// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class FName;

class GameInputConstants
{
	public:
		
		// Misc
		static const FName CONFIRM;

		// Move Keyboard
		static const FName MOVE_UP;
		static const FName MOVE_DOWN;
		static const FName MOVE_LEFT;
		static const FName MOVE_RIGHT;
		static const FName MOVE_ASCEND;
		static const FName MOVE_DESCEND;

		// Build Cycle Piece
		static const FName CYCLE_PIECE_LEFT;
		static const FName CYCLE_PIECE_RIGHT;

		// Rotate Keyboard
		static const FName ROTATE_C;
		static const FName ROTATE_CC;

		// Axis
		static const FName X_CAMERA_AXIS;
		static const FName Y_CAMERA_AXIS;

		static const FName X_MOVE_AXIS;
		static const FName Y_MOVE_AXIS;
		static const FName Z_MOVE_AXIS;

		static const FName ROTATE_MOVE_AXIS;
};

