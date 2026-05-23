#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// ---------------------------------------------------------
// WiFi & Telegram Configuration
// ---------------------------------------------------------

// Initialize Telegram BOT
const char* ssid = "REPLACE_WITH_YOUR_WIFI_SSID";
const char* password = "REPLACE_WITH_YOUR_WIFI_PASSWORD";

// Initialize Telegram BOT
#define BOT_TOKEN "REPLACE_WITH_YOUR_BOT_TOKEN" // Get from @BotFather
#define CHAT_ID "REPLACE_WITH_YOUR_CHAT_ID"     // Get from @myidbot

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

// ---------------------------------------------------------
// Hardware Pins & Calibration
// ---------------------------------------------------------
const int ldrPin = 34;     // Analog input pin for the LDR voltage divider
const int buzzerPin = 21;  // Digital output pin for the Active Buzzer

const int threshold = 2;   // Reading is ~0 when the laser hits the LDR

// ---------------------------------------------------------
// State Variables
// ---------------------------------------------------------
bool alarmTriggered = false; // Latching state: true if alarm is currently active
unsigned long lastTimeBotRan = 0;
const unsigned long botRequestDelay = 1000; // Check for Telegram messages every 1 second
unsigned long lastSerialPrint = 0;

// Function to handle incoming Telegram messages
void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    // Only respond to messages from our authorized CHAT_ID
    if (chat_id != CHAT_ID) continue; 

    if (text == "/reset") {
      alarmTriggered = false;
      digitalWrite(buzzerPin, LOW);
      bot.sendMessage(chat_id, "✅ Alarm has been reset. System is armed and monitoring.", "");
      Serial.println("Alarm reset via Telegram.");
    }
    else if (text == "/status") {
      if (alarmTriggered) {
         bot.sendMessage(chat_id, "🚨 System Status: ALARM IS CURRENTLY TRIGGERED!", "");
      } else {
         bot.sendMessage(chat_id, "✅ System Status: Armed and Normal. No intruders.", "");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(ldrPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW); // Ensure buzzer is off

  // Connect to Wi-Fi
  Serial.print("Connecting to WiFi SSID: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  // Required for Telegram API over HTTPS
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); 

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Send a startup message
  bot.sendMessage(CHAT_ID, "✅ Laser Tripwire System Armed! ⚡\nCommands available: /status, /reset", "");
  Serial.println("System Armed.");
}

void loop() {
  // 1. Check for incoming Telegram messages (non-blocking)
  if (millis() - lastTimeBotRan > botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }

  // 2. Read the LDR sensor
  int ldrValue = analogRead(ldrPin);
  
  // Print LDR value every 1 second to avoid Serial spam
  if (millis() - lastSerialPrint > 1000) {
    Serial.print("LDR Reading: ");
    Serial.println(ldrValue);
    lastSerialPrint = millis();
  }
  
  // 3. Trigger Logic (Latches the alarm)
  if (ldrValue >= threshold && !alarmTriggered) {
    Serial.println("INTRUDER DETECTED! Beam broken!");
    
    alarmTriggered = true; // Latch the alarm ON
    
    // Send Telegram alert
    bot.sendMessage(CHAT_ID, "🚨 ALERT! Intruder detected! The laser beam was broken.\n\nSend /reset to silence the alarm.", "");
  }

  // 4. Actuate Hardware based on latched state
  if (alarmTriggered) {
    digitalWrite(buzzerPin, HIGH); // Buzzer stays ON
  } else {
    digitalWrite(buzzerPin, LOW);  // Buzzer stays OFF
  }
  
  delay(100); // Small delay for stability
}