
#define MOTOR_LEFT_PWR_PIN    11    /** Connected to L293 pin 1*/
#define MOTOR_LEFT_A_PIN      10    /** Connected to L293 pin 2 */
#define MOTOR_LEFT_B_PIN      9     /** Connected to L293 pin 7 */

#define MOTOR_RIGHT_PWR_PIN    12    /** Connected to L293 pin 9*/
#define MOTOR_RIGHT_A_PIN      8     /** Connected to L293 pin 10 */
#define MOTOR_RIGHT_B_PIN      7     /** Connected to L293 pin 15 */

typedef enum
{
  MOTOR_STOP,
  MOTOR_FWD,
  MOTOR_REV
}motor_state_t;

typedef struct
{
  bool    a_state;
  bool    b_state;
  uint8_t a_pin;
  uint8_t b_pin;
  uint8_t power;
  uint8_t power_pin;
}motor_t;

motor_t m_motor_left = {
  .a_state    = false,
  .b_state    = false,
  .a_pin      = MOTOR_LEFT_A_PIN,
  .b_pin      = MOTOR_LEFT_B_PIN,
  .power      = 0,
  .power_pin  = MOTOR_LEFT_PWR_PIN
};

motor_t m_motor_right = {
  .a_state    = false,
  .b_state    = false,
  .a_pin      = MOTOR_RIGHT_A_PIN,
  .b_pin      = MOTOR_RIGHT_B_PIN,
  .power      = 0,
  .power_pin  = MOTOR_RIGHT_PWR_PIN
};

void motor_write(motor_t * p_motor)
{
  digitalWrite(p_motor->a_pin, p_motor->a_state);
  digitalWrite(p_motor->b_pin, p_motor->b_state);
  analogWrite(p_motor->power_pin, p_motor->power);
}

void motor_config(motor_t * p_motor, motor_state_t state, uint8_t pwr)
{
    switch(state)
    {
      case MOTOR_STOP:
      default:
        p_motor->a_state  = false;
        p_motor->b_state  = false;
        p_motor->power    = 0;
      break;

      case MOTOR_FWD:
        p_motor->a_state  = false;
        p_motor->b_state  = true;
        p_motor->power    = pwr;
      break;

      case MOTOR_REV:
        p_motor->a_state  = true;
        p_motor->b_state  = false;
        p_motor->power    = pwr;
      break;
    }
    motor_write(p_motor);
}

void setup() {

  Serial.begin(115200);

  pinMode(m_motor_left.a_pin, OUTPUT);
  pinMode(m_motor_left.b_pin, OUTPUT);
  pinMode(m_motor_left.power_pin, OUTPUT);

  pinMode(m_motor_right.a_pin, OUTPUT);
  pinMode(m_motor_right.b_pin, OUTPUT);
  pinMode(m_motor_right.power_pin, OUTPUT);
  
  motor_config(&m_motor_right, MOTOR_STOP, 0);
  motor_config(&m_motor_left, MOTOR_STOP, 0);
  
  
}

void loop() {

  motor_config(&m_motor_left, MOTOR_FWD, 255);
  motor_config(&m_motor_right, MOTOR_FWD, 255);
  delay(1000);
  motor_config(&m_motor_left, MOTOR_STOP, 0);
  motor_config(&m_motor_right, MOTOR_STOP, 0);
  delay(2500);
  motor_config(&m_motor_left, MOTOR_REV, 255);
  motor_config(&m_motor_right, MOTOR_REV, 255);
  delay(1000);
  motor_config(&m_motor_left, MOTOR_STOP, 0);
  motor_config(&m_motor_right, MOTOR_STOP, 0);
  delay(2500);



  

}
