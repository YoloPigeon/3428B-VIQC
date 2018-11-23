#pragma config(Sensor, port6,  Gyro,           sensorVexIQ_Gyro)
#pragma config(Sensor, port7,  LED,            sensorVexIQ_LED)
#pragma config(Sensor, port8,  LineFollowerLeft, sensorVexIQ_ColorGrayscale)
#pragma config(Sensor, port9,  LineCrossSensor, sensorVexIQ_ColorGrayscale)
#pragma config(Sensor, port10, TestLED,        sensorVexIQ_LED)
#pragma config(Sensor, port11, LineFollowerRight, sensorVexIQ_ColorGrayscale)
#pragma config(Motor,  motor1,          Claw,          tmotorVexIQ, PIDControl, encoder)
#pragma config(Motor,  motor2,          XBottom,       tmotorVexIQ, PIDControl, reversed, encoder)
#pragma config(Motor,  motor3,          YLeft,         tmotorVexIQ, PIDControl, encoder)
#pragma config(Motor,  motor4,          YRight,        tmotorVexIQ, PIDControl, reversed, encoder)
#pragma config(Motor,  motor5,          XTop,          tmotorVexIQ, PIDControl, encoder)
#pragma config(Motor,  motor12,         Arm,           tmotorVexIQ, PIDControl, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

enum Compass {North = 0, East = 1, South = 2, West = 3};

#define Height0 -10//\.
#define Height1 55//  \.
#define Height2 110//  >Arm Index Heights
#define Height3 260// /.
#define Height4 390///.
#define ArmLiftSpeed 90//The speed the arm lifts and lowers
#define PickupYShift 105//Wheel rotations after line to get to wall
#define DropYShift 340//Wheel rotations after line to get to scoring pole
#define GyroDrift 19//Ammount the gyro drifts after 1 minute////////////////////////////////////changes every day, MUST ADJUST BEFORE USE

#define AutoRotateFullSpeed 95//Rotate speed while in autonomous mode
#define AutoTravelFullSpeed 90//Travel speed while in autonomous mode
#define AutoTravelMinimumSpeed 20//Minimum travel speed while in autonomous mode
#define AutoRotateMinimumSpeed 5//Minimum rotate speed while in autonomous mode

#define LineSearchSpeed 50//Speed while finding a line
#define LineSenseThreshold 225//SHOULD BE 150 Greyscale value bellow which a line is sensed when stopping at a line
#define LineFollowThreshold 450//Greyscale value bellow which a line is sensed when following a line
#define LineFollowMaxXShift 9//Line follow max corretion

#define LineFollowDeadband 1//Deadband when locating on a line
#define ObstacleDeadband 10//Deadband while stoping at a specified distance from an obstacle
#define RotateDeadband 1//Deadband while rotating to a specified angle
#define ObstableProportionalBand 20//Proportional band when stoping at a specified distance from an obstacle
#define RotateProportionalBand 90//Proportional band when rotating to a specified angle

#define RuntimeTimer T1//Counts total number of milliseconds since program has started
#define StateTimer T2//Time a sequence has been in the current state
#define XShiftDoneTimer T3//Time XMotor velocity has to be 0
#define YShiftDoneTimer T4//Time YMotor velocity has to be 0

float EastSpeed;
float AutoEastSpeed;
float NorthSpeed;
float AutoNorthSpeed;
float XShift;
float YShift;
float RotateSpeed;
float AutoRotateSpeed;
int Heading;
int State5;
bool ClawClose;
bool StopXShift=false;
bool StopYShift=false;
bool EStop=false;
bool SetSpeedQuarter;
bool ResetSpeedFull;
int ArmPosition=0;

/*************************************************************************************************************************
Emergency stop*/

//void EStop()
//{



/*************************************************************************************************************************
Controls proportional speeds*/

float ProportionalPositionControl(float MaximumSpeed, float MinimumSpeed, float Deadband, float ProportionalBand, float TargetPosition, float Position)
{
	float Error=TargetPosition-Position;

	if(Error<Deadband*-1 && Error>(ProportionalBand+Deadband)*-1)//Error within negative proportional band and outside deadband.
	{
		return(((abs(Error)-Deadband)/ProportionalBand*(MaximumSpeed-MinimumSpeed)+MinimumSpeed)*-1);
	}
	if(Error>Deadband && Error<(ProportionalBand+Deadband))//Error within positive proportional band and outside deadband.
	{
		return((Error-Deadband)/ProportionalBand*(MaximumSpeed-MinimumSpeed)+MinimumSpeed);
	}
	if(Error>=ProportionalBand+Deadband)//Error Outside Positive Proportional band
	{
		return(MaximumSpeed);
	}
	if(Error<=(ProportionalBand+Deadband)*-1)//Error Outside Negative Proportional band
	{
		return(MaximumSpeed*-1);
	}
	return 0;
}

/*************************************************************************************************************************
Keeps robot centred on line*/

void LineFollow()
{
	if(XShift==0 && (getColorValue(LineFollowerLeft)<LineFollowThreshold || getColorValue(LineFollowerRight)<LineFollowThreshold))
	{
		XShift=ProportionalPositionControl(LineFollowMaxXShift, 0, LineFollowDeadband, 250, 0, (getColorValue(LineFollowerRight)-getColorValue(LineFollowerLeft)));
	}
}

/*************************************************************************************************************************
Rotate until facing desired direction.*/

bool GoToHeading(float TargetHeading, int Heading)
{
	float Error=TargetHeading-Heading;
	if(Error>180)
	{
		Error=Error-360;
	}
	if(Error<-180)
	{
		Error=Error+360;
	}
	displayTextLine(0, "Error=%d", Error);

	AutoRotateSpeed=ProportionalPositionControl(AutoRotateFullSpeed, AutoRotateMinimumSpeed, RotateDeadband, RotateProportionalBand, 0, Error)*-1;
	if(AutoRotateSpeed==0)
	{
		return(false);
	}
	else
	{
		return(true);
	}
}

/*************************************************************************************************************************
Drive a certain direction until on a line.*/

bool GoToLine(Compass Direction, tSensors ColorSensor)
{
	setTouchLEDRGB(TestLED, 255, 255, 0);
	switch(Direction)
	{
		case North:
			if(getColorValue(ColorSensor)>LineSenseThreshold)//Value can be modified
			{
				YShift=540;
			}
			else
			{
				YShift=0;
				StopYShift=true;
				return(false);
			}
			break;

		case East:
			if(getColorValue(ColorSensor)>LineSenseThreshold)
			{
				AutoEastSpeed=LineSearchSpeed;
			}
			else
			{
				AutoEastSpeed=0;
				return(false);
			}
			break;

		case South:
			if(getColorValue(ColorSensor)>LineSenseThreshold)
			{
				YShift=540;
			}
			else
			{
				YShift=0;
				StopYShift=true;
				return(false);
			}
			break;

		case West:
			if(getColorValue(ColorSensor)>LineSenseThreshold)
			{
				AutoEastSpeed=LineSearchSpeed*-1;
			}
			else
			{
				AutoEastSpeed=0;
				return(false);
			}
			break;
		}
	return(true);
}

/*************************************************************************************************************************
Rotate and drive east.*/

bool RotateAndDriveEast(int TargetHeading)
{
	bool GoingToHeading;
	bool GoingToLine;

	GoingToHeading=GoToHeading(TargetHeading, Heading);
	if(!GoingToHeading)
	{
		GoingToLine=GoToLine(East, LineFollowerRight);
		if(!GoingToLine)
		{
			return(false);
		}
	}
	return(true);
}

/*************************************************************************************************************************
Drives to load position and picks up rings off starting peg.*/

void PickupSequence()
{
	static bool FoundLine;
	static int PickupSequenceState;
	displayTextLine(3, "PickupSequenceState=%d", PickupSequenceState);

	switch(PickupSequenceState)
	{
	case 1://Idle
		if(ClawClose)
		{
			if(ArmPosition==2)
			{
				PickupSequenceState=2;
			}
			else
			{
				PickupSequenceState=5;
			}
		}
		break;

	case 2://Rotate & Drive East
		if(!RotateAndDriveEast(0))
		{
			PickupSequenceState=3;
			FoundLine=false;
		}
		break;

	case 3://Drive North
		LineFollow();
		if(!FoundLine)
		{
			if(!GoToLine(North, LineCrossSensor))
			{
				FoundLine=true;
				YShift=PickupYShift;
			}
		}
		else
		{
			if(YShift==0)
			{
				PickupSequenceState=4;
				resetTimer(StateTimer);
			}
		}
		break;

	case 4://Lower Arm
		ArmPosition=1;
		if(getTimerValue(StateTimer)>500)
		{
			PickupSequenceState=5;
			resetTimer(StateTimer);
		}
		break;

	case 5://Close Claw
		setMotorSpeed(Claw, 100);
		if(ArmPosition!=1)
		{
			PickupSequenceState=8;
		}
		else
		{
			if(getTimerValue(StateTimer)>350)
			{
				PickupSequenceState=6;
				YShift=-65;
			}
		}
		break;

	case 6://Drive South
		if(YShift==0)
		{
			PickupSequenceState=7;
		}
		break;

	case 7://Arm Raise
		ArmPosition=4;
		if(getMotorMoveComplete(Arm))
		{
			PickupSequenceState=8;
		}
		break;

	case 8://Done
		if(!ClawClose)
		{
			PickupSequenceState=1;
		}
		break;

	default:
		PickupSequenceState=1;
	}

	if(EStop)
	{
		if(PickupSequenceState!=8 && ArmPosition!=4)
		{
			setMotorTarget(Claw, 0, 75);
			ClawClose=false;
			if(ArmPosition==1)
			{
				ArmPosition=2;
			}
			PickupSequenceState=1;
		}
	}
}

/*************************************************************************************************************************
Drives to drop position and drops rings onto scoring pole.*/

void DropSequence()
{
	static bool FoundLine;
	static int DropSequenceState;
	displayTextLine(4, "DropSequenceState=%d", DropSequenceState);

	State5=DropSequenceState==5 || DropSequenceState==6;

	switch(DropSequenceState)
	{
	case 1://Idle
		if(!ClawClose)
		{
			if(ArmPosition==4)
			{
				DropSequenceState=2;
			}
			else
			{
				DropSequenceState=7;
			}
		}
		break;

	case 2://Rotate & Drive East
		if(!RotateAndDriveEast(180))
		{
			DropSequenceState=3;
			FoundLine=false;
		}
		break;

	case 3://Drive South
	LineFollow();
		if(!FoundLine)
		{
			if(!GoToLine(North, LineCrossSensor))
			{
				FoundLine=true;
				YShift=DropYShift;
			}
		}
		else
		{
			if(YShift==0)
			{
				DropSequenceState=4;
			}
		}
		break;

	case 4://Rotate until facing desired direction
		if(!GoToHeading(180, Heading))
		{
			SetSpeedQuarter=true;
			resetTimer(StateTimer);
			DropSequenceState=5;
		}
		break;

	case 5://Pause sequence until button pressed(During this time manual adjustment is used)
		if(getJoystickValue(BtnLUp)==1)
		{
			resetTimer(StateTimer);
			DropSequenceState=6;
		}
		break;

	case 6://Lower Claw
		ArmPosition=3;
		YShift=-90;
		ResetSpeedFull=true;
		if(getTimerValue(StateTimer)>500)
		{
			DropSequenceState=7;
		}
		break;

	case 7://Open Claw
		setMotorTarget(Claw, 0, 75);
		DropSequenceState=8;

	case 8://Done
		if(ClawClose)
		{
			DropSequenceState=1;
		}
		break;

	default:
		DropSequenceState=1;
	}

	if(EStop)
	{
		if(DropSequenceState!=1 && DropSequenceState!=8 && ArmPosition!=1 && ArmPosition!=2)
		{
			ArmPosition=4;
		}
		if(ClawClose)
		{
			DropSequenceState=1;
		}
		else
		{
			DropSequenceState=8;
		}
	}
}

/*************************************************************************************************************************
Sets the height of the arm and starts pickup and drop sequences.*/

task ArmMove()
{
	while(true)
	{
		switch(ArmPosition)
		{
		case 0://Floor
			setMotorTarget(Arm, Height0, ArmLiftSpeed);
			break;

		case 1://Pickup Dip Height
			setMotorTarget(Arm, Height1, ArmLiftSpeed);
			break;

		case 2://Start Peg Height
			setMotorTarget(Arm, Height2, ArmLiftSpeed);
			break;

		case 3://Ring Drop Height
			setMotorTarget(Arm, Height3, ArmLiftSpeed);
			break;

		case 4://Goal Post Position Height
			setMotorTarget(Arm, Height4, ArmLiftSpeed);
			break;
		}

		PickupSequence();
		DropSequence();
	}
}

/*************************************************************************************************************************
Sets the manual and auto motor speeds.*/

task DriveMotor()
{
	float XSpeed;
	float YSpeed;
	float TravelDirection;
	float TravelSpeed;
	float DriveAngle;
	bool XShiftOS=false;//OS=One Shot
	bool YShiftOS=false;//^

	while(true)
	{
		if(EStop)
		{
			AutoNorthSpeed=0;
			AutoEastSpeed=0;
			AutoRotateSpeed=0;
			XShift=0;
			YShift=0;
			XShiftOS=false;
			YShiftOS=false;
		}

		Heading=(getGyroDegreesFloat(Gyro)+((GyroDrift*getTimerValue(RuntimeTimer))/60000)+90)%360;
		if(Heading>360)
		{
			Heading=Heading-360;
		}

		TravelSpeed=sqrt(EastSpeed*EastSpeed + NorthSpeed*NorthSpeed);
		if(TravelSpeed>0)
		{
			if(NorthSpeed>=0)
			{
				TravelDirection=radiansToDegrees(asin(EastSpeed/TravelSpeed));
			}
			else
			{
				TravelDirection=180-radiansToDegrees(asin(EastSpeed/TravelSpeed));
			}
			DriveAngle=TravelDirection-Heading;
			XSpeed=sinDegrees(DriveAngle)*TravelSpeed;
			YSpeed=cosDegrees(DriveAngle)*TravelSpeed;
		}
		else
		{
			XSpeed=0;
			YSpeed=0;
		}

		if(StopXShift)
		{
			XShiftOS=false;
			StopXShift=false;
		}
		if(StopYShift)
		{
			YShiftOS=false;
			StopYShift=false;
		}

		if(XShift!=0 || YShift!=0)
		{
			if(XShift!=0)
			{
				if(!XShiftOS)
				{
					setTouchLEDRGB(TestLED, 0, 255, 255);
					resetTimer(XShiftDoneTimer);
					setMotorTarget(XTop, getMotorEncoder(XTop)+XShift, LineSearchSpeed);
					setMotorTarget(XBottom, getMotorEncoder(XBottom)+XShift, LineSearchSpeed);
					XShiftOS=true;
				}
				else
				{
					if(!getMotorZeroVelocity(XTop) || !getMotorZeroVelocity(XBottom))
					{
						resetTimer(XShiftDoneTimer);
					}
					if(getTimerValue(XShiftDoneTimer)>50)
					{
						setTouchLEDRGB(TestLED, 0, 0, 0);
						XShift=0;
						XShiftOS=false;
					}
				}
			}

			if(YShift!=0)
			{
				if(!YShiftOS)
				{
					setTouchLEDRGB(TestLED, 0, 0, 255);
					resetTimer(YShiftDoneTimer);
					setMotorTarget(YLeft, getMotorEncoder(YLeft)+YShift, LineSearchSpeed);
					setMotorTarget(YRight, getMotorEncoder(YRight)+YShift, LineSearchSpeed);
					YShiftOS=true;
				}
				else
				{
					if(!getMotorZeroVelocity(YLeft) || !getMotorZeroVelocity(YRight))
					{
						resetTimer(YShiftDoneTimer);
					}
					if(getTimerValue(YShiftDoneTimer)>50)
					{
						setTouchLEDRGB(TestLED, 0, 0, 0);
						YShift=0;
						YShiftOS=false;
					}
				}
			}
		}
		else
		{
			setMotorSpeed(XTop, XSpeed+RotateSpeed);
			setMotorSpeed(XBottom, XSpeed-RotateSpeed);
			setMotorSpeed(YLeft, YSpeed+RotateSpeed);
			setMotorSpeed(YRight, YSpeed-RotateSpeed);
		}
	}
}

/*************************************************************************************************************************
Controls display and sets ArmPosition, NorthSpeed and EastSpeed values according to buttons and joysticks on remote.*/

task main()
{
	resetGyro(Gyro);
	resetTimer(T1);
	int JoystickCPos;
	int EStopPressTime;
	float SpeedMultiply=1;
	float TurnMultiply=0.6;
	setTouchLEDRGB(LED, 255, 0, 0);

	startTask(ArmMove);
	startTask(DriveMotor);

	while(true)
	{
		//displayTextLine(0, "Speed=%d  TurnSpeed=%d", SpeedMultiply*10, TurnMultiply*10);
		displayTextLine(0, "LineCrossSensor=%f", getColorValue(LineCrossSensor));
		displayTextLine(1, "Left=%d Right=%d", getColorValue(LineFollowerLeft), getColorValue(LineFollowerRight));
		displayTextLine(2, "Gyro=%d  Heading=%d", getGyroDegrees(Gyro), Heading);
		//displayTextLine(3, "PickupSequenceState=%d", PickupSequenceState);
		//displayTextLine(4, "DropSequenceState=%d", DropSequenceState);
		displayTextLine(5, "Arm Height=%d", ArmPosition);

		if(getTimerValue(RuntimeTimer)>70000 && getTimerValue(RuntimeTimer)<71000)
		{
			playNote(noteCSharp, 2, 100);
		}

		if(getJoystickValue(BtnLDown)==1)
		{
			EStopPressTime=getTimerValue(RuntimeTimer);
			EStop=true;
		}
		if(EStop && (getTimerValue(RuntimeTimer)-EStopPressTime)>100)
		{
			EStop=false;
		}


		if(getJoystickValue(BtnRUp)==1 && ArmPosition!=4)
		{
			ArmPosition++;
			if(ArmPosition==3)
			{
				ArmPosition=4;
			}
			if(ArmPosition==1)
			{
				ArmPosition=2;
			}
			sleep(160);
		}

		if(getJoystickValue(BtnRDown)==1 && ArmPosition!=0)
		{
			ArmPosition--;
			if(ArmPosition==1)
			{
				ArmPosition=0;
			}
			sleep(160);
		}

		if(getJoystickValue(BtnLUp)==1 && !State5)
		{
			ClawClose=!ClawClose;
			sleep(200);
		}

		if(SetSpeedQuarter==true)
		{
			SetSpeedQuarter=false;
			SpeedMultiply=0.4;
			TurnMultiply=0.3;
			setTouchLEDRGB(LED, 204, 0 , 204);
		}

		if(getJoystickValue(ChD)>90 || ResetSpeedFull==true)
		{
			ResetSpeedFull=false;
			SpeedMultiply=1;
			TurnMultiply=0.6;
			setTouchLEDRGB(LED, 255, 0, 0);
		}
		else
		{
			if(getJoystickValue(ChD)<-90)
			{
				SpeedMultiply=0.5;
				TurnMultiply=0.4;
				setTouchLEDRGB(LED, 0, 255, 0);
			}
		}

		JoystickCPos=getJoystickValue(ChC);
		if(JoystickCPos<15 && JoystickCPos>-15)
		{
			JoystickCPos=0;
		}

		RotateSpeed=((JoystickCPos*TurnMultiply)+AutoRotateSpeed);
		EastSpeed=((getJoystickValue(ChB)*SpeedMultiply)+AutoEastSpeed);
		NorthSpeed=((getJoystickValue(ChA)*SpeedMultiply)+AutoNorthSpeed);
	}
}
