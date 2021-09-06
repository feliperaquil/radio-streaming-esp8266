#include <Arduino.h>

#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include "AudioFileSourceICYStream.h"
#include "AudioFileSourceBuffer.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2SNoDAC.h"

/* Pasos extra: 
1. Agregar link extra en Archivo -> Preferencias: http://arduino.esp8266.com/stable/package_esp8266com_index.json  
2. En Herramientas -> Placa -> Gestion de tarjetas, instalar ESP8266 (si no aparece, link está malo)
3. Elegir la tarjeta: Herramientas -> Placa -> Generic ESP8266 Module
4. Actualizar ESP8266 a 160MHz antes
*/

// Configuración de Wi-Fi:
const char *SSID = "SoyMomoRD";
const char *PASSWORD = "soymomo2";

//Radioemisoras para elegir:
char *URL1="http://stream.live.vc.bbcmedia.co.uk/bbc_radio_fourfm";
char *URL2="http://studioone-stream.iowapublicradio.org/StudioOne.mp3";
char *URL3="http://rockthecradle.stream.publicradio.org/radioheartland.mp3";

char* URL_list[3] = {URL1, URL2, URL3};

//Radioemisora elegida:
int index_radio = 0; //indice de radio elegida desde URL_list
char *URL = URL_list[index_radio]; //parte con URL1
const int buttonPin = 2;     //Posicion del boton


AudioGeneratorMP3 *mp3;
AudioFileSourceICYStream *file;
AudioFileSourceBuffer *buff;
AudioOutputI2SNoDAC *out;

// Llamada cuando evento de metadata ocurre (ID3 tag, ICY block,...)
void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string)
{
const char *ptr = reinterpret_cast<const char *>(cbData);
(void) isUnicode; 
// Notar que el tipo y string podrian estar en PROGMEM, asi que hay que copiarlos a RAM para hacer printf.
char s1[32], s2[64];
strncpy_P(s1, type, sizeof(s1));
s1[sizeof(s1)-1]=0;
strncpy_P(s2, string, sizeof(s2));
s2[sizeof(s2)-1]=0;
Serial.printf("METADATA(%s) '%s' = '%s'\n", ptr, s1, s2);
Serial.flush();
}

// Llamada cuando hay un warning o error (como underflow de un buffer)
void StatusCallback(void *cbData, int code, const char *string)
{
const char *ptr = reinterpret_cast<const char *>(cbData);
// Note that the string may be in PROGMEM, so copy it to RAM for printf
char s1[64];
strncpy_P(s1, string, sizeof(s1));
s1[sizeof(s1)-1]=0;
Serial.printf("STATUS(%s) '%d' = '%s'\n", ptr, code, s1);
Serial.flush();
}


void setup()
{
pinMode(buttonPin, INPUT);

Serial.begin(115200);
delay(1000);
Serial.println("Conectando a WiFi");

WiFi.disconnect();
WiFi.softAPdisconnect(true);
WiFi.mode(WIFI_STA);

WiFi.begin(SSID, PASSWORD);

// Try forever
while (WiFi.status() != WL_CONNECTED) {
Serial.println("...Conectando a WiFi");
delay(1000);
}
Serial.println("Conectado");

audioLogger = &Serial;
file = new AudioFileSourceICYStream(URL);
file->RegisterMetadataCB(MDCallback, (void*)"ICY");
buff = new AudioFileSourceBuffer(file, 8192);
buff->RegisterStatusCB(StatusCallback, (void*)"buffer");
out = new AudioOutputI2SNoDAC();
mp3 = new AudioGeneratorMP3();
mp3->RegisterStatusCB(StatusCallback, (void*)"mp3");
mp3->begin(buff, out);
}

void loop()
{
static int lastms = 0;
if (mp3->isRunning()) {
if (digitalRead(buttonPin)==HIGH){ //boton pulsado
// cambiar radio
mp3->stop();
index_radio += 1; 
URL = URL_list[index_radio%3]; //cambia el indice de la radio
file = new AudioFileSourceICYStream(URL); //nuevo elemento stream, con la radio nueva
file->RegisterMetadataCB(MDCallback, (void*)"ICY");
buff = new AudioFileSourceBuffer(file, 8192);
buff->RegisterStatusCB(StatusCallback, (void*)"buffer");
out = new AudioOutputI2SNoDAC();
mp3 = new AudioGeneratorMP3();
mp3->RegisterStatusCB(StatusCallback, (void*)"mp3");
mp3->begin(buff, out); //cambiar (efectivamente) la radio
} else {
  // continuar igual, midiendo el tiempo que la radio lleva en la misma emisora
if (millis()-lastms > 1000) {
lastms = millis();
Serial.printf("Running for %d ms...\n", lastms);
Serial.flush();
}
}
}
if (!mp3->loop()) mp3->stop();
else {
Serial.printf("MP3 done\n");
delay(1000);
}
}

//Nota: No hay debouncer en este código, pero debería estar
