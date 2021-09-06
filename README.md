# radio-streaming-esp8266

Notas:
1. Rx de ESP8266 debería ir con la entrada R en la parte L-tierra-R del amplificador, mientras que las tierras de ambas irian juntas (GND de ESP8266 y tierra de L-tierra-R del amplificador) -> https://www.instructables.com/PAM8403-6W-STEREO-AMPLIFIER-TUTORIAL/
2. Es necesario instalar la librería para ESP8266. Para eso (dentro de arduino IDE):
- Agregar link extra en Archivo -> Preferencias: http://arduino.esp8266.com/stable/package_esp8266com_index.json  
- En Herramientas -> Placa -> Gestion de tarjetas, instalar ESP8266 (si no aparece, link está malo)
- Elegir la tarjeta: Herramientas -> Placa -> Generic ESP8266 Module
- Actualizar ESP8266 a 160MHz (Herramientas -> CPU Frequency)
3. La forma en que lo hice, fue armando una lista de radioemisoras (sus links de streaming) y haciendo correr un indice a través de esta lista dentro del loop principal, para que la radio cambie. No incluí un debouncer para el botón, aunque creo que debería ir (puede ser que la radio se cambie de forma "misteriosa" por lo que no tendría un buen funcionamiento)
4. Dentro del while loop del programa principal, incluyo una comprobación del botón mientras exista reproducción (que se cumpla: mp3->isRunning()), de tal forma que el programa debería cambiar la radio si el botón está presionado y medir el tiempo (herencia del tutorial) si es que no lo está. Respecto a estos dos modos y considerando que el tutorial en el cual me baso efectivamente funcionaba:
- Si el botón está presionado, el streaming para (mp3->stop()) y se realiza una nueva conexión con la URL siguiente (la radio siguiente). Tengo dudas si es necesario volver a crear objetos AudioFileSourceBuffer y AudioOutputI2SNoDAC, pero los agregué por si acaso.
- Si el botón no está presionado, el streaming continua y el programa mide el tiempo que la radio ha estado funcionando, imprimiendolo al serial. Notar que este no es el tiempo que lleva la ultima radioemisora sintonizada. 
5. Sospecho que pueden haber problemas con el buffering, pero no tengo como comprobarlo.
