//***** Includes
#include <IRDaikinESP.h>
#include <IRKelvinator.h>
#include <IRMitsubishiAC.h>
#include <IRremoteESP8266.h>
#include <IRremoteInt.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <RCSwitch.h>


// Sensors:
// 1 - Kitchen
// 2 - Bedroom
// 3 - Living Room
#define sensor3

// Wifi

const char * WIFI_SSID = "SSID";
const char * WIFI_PASSWORD = "PASSWORD";

// MQTT: Server
const PROGMEM char * MQTT_SERVER = "LOCAL_IP";
#ifdef sensor
const PROGMEM char * MQTT_CLIENT_ID = "ESP8266Client";
#elif defined sensor2 
const PROGMEM char * MQTT_CLIENT_ID = "ESP8266Client2";
#elif defined sensor3
const PROGMEM char * MQTT_CLIENT_ID = "ESP8266Client3";
#endif
const PROGMEM char * MQTT_USER = "homeassistant";
const PROGMEM char * MQTT_PASSWORD = "PASSWORD";

#ifdef sensor
// MQTT: topics
// HDT
const PROGMEM char * MQTT_HUMIDITY_TOPIC = "sensor/humidity";
const PROGMEM char * MQTT_TEMPERATURE_TOPIC = "sensor/temperature";
// IR
const PROGMEM char * MQTT_IR_SEND_TOPIC = "sensor/ir_led_strip";
const PROGMEM char * MQTT_IR_RECEIVE_TOPIC = "sensor/ir_recv";
// MOTION
const PROGMEM char * MQTT_MOTION_TOPIC = "sensor/motion";
// RC315
const PROGMEM char * MQTT_RC315_READ_TOPIC = "sensor/rc_read";
const PROGMEM char * MQTT_RC315_SEND_TOPIC = "sensor/send_rc";
#elif defined sensor2
// MQTT: topics
// HDT
const PROGMEM char * MQTT_HUMIDITY_TOPIC = "sensor2/humidity";
const PROGMEM char * MQTT_TEMPERATURE_TOPIC = "sensor2/temperature";
// IR
const PROGMEM char * MQTT_IR_SEND_TOPIC = "sensor2/ir_led_strip";
const PROGMEM char * MQTT_IR_RECEIVE_TOPIC = "sensor2/ir_recv";
// MOTION
const PROGMEM char * MQTT_MOTION_TOPIC = "sensor2/motion";
// RC315
const PROGMEM char * MQTT_RC315_READ_TOPIC = "sensor2/rc_read";
const PROGMEM char * MQTT_RC315_SEND_TOPIC = "sensor2/send_rc";
#elif defined sensor3
// MQTT: topics
// HDT
const PROGMEM char * MQTT_HUMIDITY_TOPIC = "sensor3/humidity";
const PROGMEM char * MQTT_TEMPERATURE_TOPIC = "sensor3/temperature";
// IR
const PROGMEM char * MQTT_IR_SEND_TOPIC = "sensor3/ir_led_strip";
const PROGMEM char * MQTT_IR_RECEIVE_TOPIC = "sensor3/ir_recv";
// MOTION
const PROGMEM char * MQTT_MOTION_TOPIC = "sensor3/motion";
// RC315
const PROGMEM char * MQTT_RC315_READ_TOPIC = "sensor3/rc_read";
const PROGMEM char * MQTT_RC315_SEND_TOPIC = "sensor3/send_rc";
#endif

// TODO: state variables

// PINS
int ii = D1;
const PROGMEM uint8_t IR_SEND_PIN = 4;
const PROGMEM uint8_t IR_RCV_PIN = 2;
const PROGMEM uint8_t RC315_TX_PIN = 16;
const PROGMEM uint8_t RC315_RX_PIN = 13;
const PROGMEM uint8_t PIR_PIN = 5;
const PROGMEM uint8_t DHT_PIN = 14;
int RECV_PIN = IR_RCV_PIN;
#define DHTTYPE DHT11

// Globals
char rcbuf[200];
char irbuf[100];

long lastMsg = 0;
float hum = 0.0;
float diff = 1.0;
decode_results  results;

WiFiClient espClient;
PubSubClient client(espClient);
IRsend irsend(IR_SEND_PIN);
IRrecv irrecv(RECV_PIN);
RCSwitch mySwitch = RCSwitch();
DHT dht(DHT_PIN, DHTTYPE, 11);

// forward declarations
void callback(char* topic, unsigned char * p_payload, unsigned int p_length);
void send_code(const char * code);

void setup_wifi() {

	delay(10);

	// We start by connecting to a WiFi network
	Serial.println();
	Serial.print("Connecting to ");
	Serial.println(WIFI_SSID);

	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}

	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());

}

void reconnect() {
	// Loop until we're reconnected
	while (!client.connected()) {
		Serial.print("Attempting MQTT connection...");
		// Attempt to connect
		if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
			Serial.println("connected");
			client.subscribe(MQTT_IR_SEND_TOPIC);
			client.subscribe(MQTT_RC315_SEND_TOPIC);
		}
		else {
			Serial.print("failed, rc=");
			Serial.print(client.state());
			Serial.println(" try again in 5 seconds");
			// Wait 5 seconds before retrying
			delay(5000);
		}
	}
}



void callback(char* p_topic, unsigned char * p_payload, unsigned int p_length)
{

	String payload;
	String topic = String(p_topic);

	for (uint8_t i = 0; i < p_length; i++) {
		payload.concat((char)p_payload[i]);
	}

	if (topic.equals(MQTT_RC315_SEND_TOPIC)) {
		uint8_t pulseLength = 350; //default
		uint8_t indexOfSemi = payload.indexOf(';');
		String payloadActual = payload;
		if (indexOfSemi > 0) {
			pulseLength = atoi(payload.substring(indexOfSemi + 1, payload.length()).c_str());
			payloadActual = payload.substring(0, indexOfSemi);
		}

		Serial.print("Sending RC315 Cmd: ");
		Serial.print(strtoul(payloadActual.c_str(), 0, 10));
		Serial.print(", pulse length: ");
		Serial.println(pulseLength);
		mySwitch.setPulseLength(pulseLength);
		mySwitch.send(strtoul(payloadActual.c_str(), 0, 10), 24);

		return;
	}
	if (topic.equals(MQTT_IR_SEND_TOPIC)) {
		Serial.print("Sending IR Cmd: ");
		unsigned long d = strtoul(payload.c_str(), 0, 16);
		Serial.println(d);
		irsend.sendNEC(d);
		return;
	}
}

// Setup

void setup() {

	Serial.begin(115200);

	// IR
	irsend.begin();
	irrecv.enableIRIn();

	// Wifi
	setup_wifi();

	// MQTT
	client.setServer(MQTT_SERVER, 1883);
	client.setCallback(callback);

	// Radio
	mySwitch.enableTransmit(RC315_TX_PIN);
	mySwitch.enableReceive(RC315_RX_PIN);

}

// Main Loop

void loop() {

	// MQTT
	if (!client.connected()) {
		reconnect();
	}
	client.loop();

	// Radio receive
	if (mySwitch.available()) {
		output(mySwitch.getReceivedValue(), mySwitch.getReceivedBitlength(), mySwitch.getReceivedDelay(), mySwitch.getReceivedRawdata(), mySwitch.getReceivedProtocol());
		client.publish(MQTT_RC315_READ_TOPIC, rcbuf, true);
		Serial.println(rcbuf);
		mySwitch.resetAvailable();
		rcbuf[0] = '\0';
	}

	// IR Receive
	if (irrecv.decode(&results)) {
		dumpInfo(&results);
		client.publish(MQTT_IR_RECEIVE_TOPIC, irbuf, true);
		Serial.println(irbuf);
		irbuf[0] = '\0';
		irrecv.resume();
	}

	// Sample per 0.5 second
	long now = millis();
	if (now - lastMsg > 500) {

		lastMsg = now;

		// Temp&Humidity
		float newTemp = dht.readTemperature();
		float newHum = dht.readHumidity();
		client.publish(MQTT_TEMPERATURE_TOPIC, String(newTemp).c_str(), true);
		client.publish(MQTT_HUMIDITY_TOPIC, String(newHum).c_str(), true);

		// Motion Detection
		char motion[2];
		itoa(digitalRead(PIR_PIN), motion, 2);
		client.publish(MQTT_MOTION_TOPIC, motion, true);

	}
}