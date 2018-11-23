#pragma config(Sensor, port3,  Main_Gyro,      sensorVexIQ_Gyro)
#pragma config(Motor,  motor1,          Left,          tmotorVexIQ, PIDControl, encoder)
#pragma config(Motor,  motor6,          Right,         tmotorVexIQ, PIDControl, reversed, encoder)
#pragma config(Motor,  motor11,         ArmLeft,       tmotorVexIQ, PIDControl, encoder)
#pragma config(Motor,  motor10,         ArmRight,      tmotorVexIQ, PIDControl, reversed, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

task main()
{

	resetMotorEncoder(ArmLeft);  //Resets Left Arm Motor Encoder to 0
	resetMotorEncoder(ArmRight); //Resets Right Arm Motor Encoder to 0

	while(true) //while the program is running do this:
	{

		//Display Code
		displayTextLine(0, "Arm=%d", ((getMotorEncoder(ArmLeft)) + (getMotorEncoder(ArmRight)))/2); //Displays Average of motor encoders position
		displaySensorValues(3, Main_Gyro);
		//Autonomous Code
		moveMotorTarget(Left, 720, 50);
		moveMotorTarget(Right, 720, 50);
		wait(1, seconds);
		moveMotorTarget(Left, -366.5, 50);
		moveMotorTarget(Right,366.5, 50);
		wait(1, seconds);
		moveMotorTarget(Left, 360, 50);
		moveMotorTarget(Right, 360, 50);
		wait(1, seconds);
		moveMotorTarget(Left, 1080, 50);
		moveMotorTarget(Right, 1080, 50);
		wait(1, seconds);
		moveMotorTarget(Left, 366.5, 50);
		moveMotorTarget(Right, -366.5, 50);
		wait(1, seconds);
		moveMotorTarget(Left, 1080, 50);
		moveMotorTarget(Right, 1080, 50);
		setMultipleMotors(50,ArmLeft, ArmRight);
		wait(1.5, seconds);
		moveMotorTarget(Left, -360, 50);
		moveMotorTarget(Right, -360, 50);
		break;
	}
}
//End of Program
