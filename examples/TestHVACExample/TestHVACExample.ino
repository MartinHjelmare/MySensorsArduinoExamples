/*
* Documentation: http://www.mysensors.org
* Support Forum: http://forum.mysensors.org
*/

#define MY_RADIO_NRF24
#define CHILD_ID_HVAC 0

#include <MySensors.h>

// Change and uncomment only your heatpump model
//#include <FujitsuHeatpumpIR.h>
//#include <PanasonicCKPHeatpumpIR.h>
#include <PanasonicHeatpumpIR.h>
//#include <CarrierHeatpumpIR.h>
//#include <MideaHeatpumpIR.h>
//#include <MitsubishiHeatpumpIR.h>
//#include <SamsungHeatpumpIR.h>
//#include <SharpHeatpumpIR.h>
//#include <DaikinHeatpumpIR.h>

//Some global variables to hold the states
int POWER_STATE;
int TEMP_STATE = 20;
int FAN_STATE;
int MODE_STATE;
int VDIR_STATE;
int HDIR_STATE;
String HA_MODE_STATE = "Off";
String HA_FAN_STATE = "Auto";

IRSender irSender(3);  // IR led on Arduino digital pin 3, using Arduino PWM

//Change to your Heatpump
HeatpumpIR *heatpumpIR = new PanasonicNKEHeatpumpIR();

/*
new PanasonicDKEHeatpumpIR()
new PanasonicJKEHeatpumpIR()
new PanasonicNKEHeatpumpIR()
new CarrierHeatpumpIR()
new MideaHeatpumpIR()
new FujitsuHeatpumpIR()
new MitsubishiFDHeatpumpIR()
new MitsubishiFEHeatpumpIR()
new SamsungHeatpumpIR()
new SharpHeatpumpIR()
new DaikinHeatpumpIR()
*/

bool initialValueSent = false;
bool initialValueSentArray[] = {false, false, false};

MyMessage msgHVACSetPointC(CHILD_ID_HVAC, V_HVAC_SETPOINT_COOL);
MyMessage msgHVACSpeed(CHILD_ID_HVAC, V_HVAC_SPEED);
MyMessage msgHVACFlowState(CHILD_ID_HVAC, V_HVAC_FLOW_STATE);

void presentation() {
  sendSketchInfo("Heatpump", "2.1");
  present(CHILD_ID_HVAC, S_HVAC, "Thermostat");
}

void setup() {
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!initialValueSent) {
    Serial.println("Sending initial value");
    sendNewStateToGateway();
    Serial.println("Requesting initial value from controller");
    request(CHILD_ID_HVAC, V_HVAC_SETPOINT_COOL);
    wait(2000, C_SET, V_HVAC_SETPOINT_COOL);
    request(CHILD_ID_HVAC, V_HVAC_SPEED);
    wait(2000, C_SET, V_HVAC_SPEED);
    request(CHILD_ID_HVAC, V_HVAC_FLOW_STATE);
    wait(2000, C_SET, V_HVAC_FLOW_STATE);
  }
}

void receive(const MyMessage &message) {
  if (message.isAck()) {
     Serial.println("This is an ack from gateway");
     return;
  }
  if (!initialValueSent) {
    Serial.println("Receiving initial value from controller");
  }

  Serial.print("Incoming message for: ");
  Serial.print(message.sensor);

  String recvData = message.data;
  recvData.trim();

  Serial.print(", New status: ");
  Serial.println(recvData);
  switch (message.type) {
    case V_HVAC_SPEED:
      Serial.println("V_HVAC_SPEED");
      if(recvData.equalsIgnoreCase("auto")) FAN_STATE = 0;
      else if(recvData.equalsIgnoreCase("min")) FAN_STATE = 1;
      else if(recvData.equalsIgnoreCase("normal")) FAN_STATE = 2;
      else if(recvData.equalsIgnoreCase("max")) FAN_STATE = 3;
      HA_FAN_STATE = recvData;
      initialValueSentArray[1] = true;
      sendFanStateToGateway();
    break;

    case V_HVAC_SETPOINT_COOL:
      Serial.println("V_HVAC_SETPOINT_COOL");
      TEMP_STATE = message.getFloat();
      Serial.println(TEMP_STATE);
      initialValueSentArray[0] = true;
      sendTempStateToGateway();
    break;

    case V_HVAC_FLOW_STATE:
      Serial.println("V_HVAC_FLOW_STATE");
      if (recvData.equalsIgnoreCase("coolon")) {
        POWER_STATE = 1;
        MODE_STATE = MODE_COOL;
      }
      else if (recvData.equalsIgnoreCase("heaton")) {
        POWER_STATE = 1;
        MODE_STATE = MODE_HEAT;
      }
      else if (recvData.equalsIgnoreCase("autochangeover")) {
        POWER_STATE = 1;
        MODE_STATE = MODE_AUTO;
      }
      else if (recvData.equalsIgnoreCase("off")){
        POWER_STATE = 0;
      }
      HA_MODE_STATE = recvData;
      initialValueSentArray[2] = true;
      sendModeStateToGateway();
      break;
  }
  for (int i = 0; i < 3; i++) {
    if (!initialValueSentArray[i]) {
      initialValueSent = false;
      break;
    }
    initialValueSent = true;
  }
  sendHeatpumpCommand();
}

void sendNewStateToGateway() {
  send(msgHVACSetPointC.set(TEMP_STATE));
  send(msgHVACSpeed.set(HA_FAN_STATE));
  send(msgHVACFlowState.set(HA_MODE_STATE));
}

void sendTempStateToGateway() {
  send(msgHVACSetPointC.set(TEMP_STATE));
}

void sendFanStateToGateway() {
  send(msgHVACSpeed.set(HA_FAN_STATE));
}

void sendModeStateToGateway() {
  send(msgHVACFlowState.set(HA_MODE_STATE));
}

void sendHeatpumpCommand() {
  Serial.println("Power = " + (String)POWER_STATE);
  Serial.println("Mode = " + (String)MODE_STATE);
  Serial.println("Fan = " + (String)FAN_STATE);
  Serial.println("Temp = " + (String)TEMP_STATE);

  heatpumpIR->send(
    irSender, POWER_STATE, MODE_STATE, FAN_STATE, TEMP_STATE, VDIR_AUTO,
    HDIR_AUTO);
}
