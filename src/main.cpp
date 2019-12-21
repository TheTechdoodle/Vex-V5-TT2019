#include "main.h"

/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button()
{
    static bool pressed = false;
    pressed = !pressed;
    if(pressed)
    {
        pros::lcd::set_text(2, "I was pressed!");
    }
    else
    {
        pros::lcd::clear_line(2);
    }
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize()
{
    pros::lcd::initialize();

    pros::lcd::register_btn1_cb(on_center_button);
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled()
{}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize()
{}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous()
{}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */

// The amount to turn with the 180 spin test function
// This number has its foundations in mathematical theory and application
// Read more here: https://www.teachervision.com/mathematics/problem-solving-guess-check
#define TURN_DEGREES 1300

void opcontrol()
{
    pros::Controller master(pros::E_CONTROLLER_MASTER);
    pros::Motor leftTopMotor(1);
    pros::Motor leftBottomMotor(2);
    pros::Motor rightTopMotor(3);
    pros::Motor rightBottomMotor(4);

    pros::Motor trayMotor(10);

    pros::Motor leftIntake(6);
    pros::Motor rightIntake(7);

    pros::Motor leftArmMotor(8);
    pros::Motor rightArmMotor(9);

    leftTopMotor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    leftBottomMotor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    rightTopMotor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    rightBottomMotor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    trayMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    leftIntake.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    rightIntake.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

    leftArmMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    rightArmMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

    bool cringe = false;

    while(true)
    {
        int forwardPower = master.get_analog(ANALOG_LEFT_Y);
        int turningPower = master.get_analog(ANALOG_RIGHT_X);

        leftTopMotor.move(forwardPower + turningPower);
        leftBottomMotor.move(forwardPower + turningPower);
        rightTopMotor.move((forwardPower - turningPower) * -1);
        rightBottomMotor.move((forwardPower - turningPower) * -1);

        if(master.get_digital(DIGITAL_L2))
        {
            //trayMotor.move_velocity(master.get_digital(DIGITAL_L1) ? 30 : 60);
            trayMotor.move(127);
        }
        else if(master.get_digital(DIGITAL_L1))
        {
            //trayMotor.move_velocity(master.get_digital(DIGITAL_L1) ? -30 : -60);
            trayMotor.move(-127);
        }
        else
        {
            //trayMotor.move_velocity(0);
            trayMotor.move(0);
        }

        // Flip the robot 180 degrees
        if(master.get_digital_new_press(DIGITAL_X))
        {
            leftTopMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
            leftBottomMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
            rightTopMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
            rightBottomMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
            double leftPos = leftTopMotor.get_position();
            double rightPos = rightTopMotor.get_position();

            leftTopMotor.move_relative(TURN_DEGREES, 200);
            leftBottomMotor.move_relative(TURN_DEGREES, 200);
            rightTopMotor.move_relative(TURN_DEGREES, 200);
            rightBottomMotor.move_relative(TURN_DEGREES, 200);

            while(!((leftTopMotor.get_position() < leftPos + TURN_DEGREES + 5) &&
                    (leftTopMotor.get_position() > leftPos + TURN_DEGREES - 5)))
            {
                pros::delay(2);
            }
            pros::delay(500);
            leftTopMotor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
            leftBottomMotor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
            rightTopMotor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
            rightBottomMotor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
        }

        if(master.get_digital(DIGITAL_UP))
        {
            // Move the arms up
            leftArmMotor.move_velocity(50);
            rightArmMotor.move_velocity(-50);
        }
        else if(master.get_digital(DIGITAL_DOWN))
        {
            // Move the arms down
            leftArmMotor.move_velocity(-50);
            rightArmMotor.move_velocity(50);
        }
        else
        {
            leftArmMotor.move_velocity(0);
            rightArmMotor.move_velocity(0);
        }

        if(master.get_digital(DIGITAL_R1))
        {
            // Move intake out
            leftIntake.move(50);
            rightIntake.move(-50);
        }
        else if(master.get_digital(DIGITAL_R2))
        {
            // Move intake in
            leftIntake.move(-127);
            rightIntake.move(127);
        }
        else
        {
            leftIntake.move(0);
            rightIntake.move(0);
        }

        if(master.get_digital(DIGITAL_B))
        {
            leftTopMotor.move(-50);
            leftBottomMotor.move(-50);
            rightTopMotor.move(50);
            rightBottomMotor.move(50);

            leftIntake.move(50);
            rightIntake.move(-50);
        }

        pros::lcd::print(1, "Left Y: %d", forwardPower);
        pros::lcd::print(2, "Right X: %d", turningPower);
        pros::lcd::print(3, "L-Voltage: %d", forwardPower + turningPower);
        pros::lcd::print(4, "R-Voltage: %d", forwardPower - turningPower);
        pros::lcd::print(5, "Left Arm Pos: %f", leftArmMotor.get_position());
        pros::lcd::print(6, "Right Arm Pos: %f", rightArmMotor.get_position());
        pros::lcd::print(7, "Tray Pos: %f", trayMotor.get_position());

        pros::delay(10);
    }
}