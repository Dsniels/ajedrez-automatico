# Ajedrez Automático

Este proyecto implementa un sistema automatizado que utiliza una cámara ESP32, Arduino y una pantalla LCD y  comunicación serial para crear una experiencia de interacción con un tablero de ajedrez.

---

## Requisitos Previos

Asegúrate de tener instalados los siguientes componentes:

### Software:
- [Arduino IDE](https://www.arduino.cc/en/software) 
- [.NET 9 SDK](https://dotnet.microsoft.com/en-us/download/dotnet/9.0) 

### Hardware:
- Una placa **ESP32 - Cam**.
- Una pantalla LCD 16x2.
- Un **Arduino UNO**

- Cableado necesario para conectar los componentes.

---

## Configuración del Entorno

### 1. Configurar Arduino IDE para ESP32

1. **Agregar el soporte para ESP32**:
   - Abre el Arduino IDE.
   - Ve a `Archivo > Preferencias`.
   - En el campo de "Gestor de URLs Adicionales de Tarjetas", agrega:
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Haz clic en `OK`.

2. **Instalar las placas ESP32**:
   - Ve a `Herramientas > Placa > Gestor de Tarjetas`.
   - Busca `esp32` en el buscador.
   - Instala el paquete proporcionado por **Espressif Systems**.

3. **Seleccionar la placa correcta**:
   - Ve a `Herramientas > Placa` y selecciona tu modelo de ESP32 (por ejemplo, `AI Thinker ESP32-CAM`).

4. **Configurar el puerto COM**:
   - Conecta tu ESP32 al ordenador.
   - Ve a `Herramientas > Puerto` y selecciona el puerto correspondiente.

5. **Instalar bibliotecas necesarias**:
   - Ve a `Herramientas > Administrar Bibliotecas...` y busca:
     - `LiquidCrystal` (para la pantalla LCD).
   - Instala las bibliotecas.

---

### 2. Configurar .NET para Comunicación Serial

1. **Instalar .NET 9 SDK**:
   - Descarga el SDK de .NET 9 
   - Sigue las instrucciones de instalación específicas para tu sistema operativo.

2. **Verificar la instalación**:
   - Abre una terminal y ejecuta:
     ```bash
     dotnet --version
     ```
   - Deberías ver algo como `9.x.x`.

### 3. Configurar Azure AI Foundry

1. **Crear una cuenta en Azure**:
   - Accede a [Azure Portal](https://portal.azure.com/) y crea una cuenta si no tienes una.

2. **Configurar Azure AI Foundry**:
   - En el portal de Azure, navega a **Azure AI Foundry**.
   - Crea una nueva instancia de Foundry y asegúrate de obtener:
     - La **API Key**.
     - El **nombre del deployment**.
     - El **endpoint** asociado a tu Foundry.

3. **Actualizar el archivo `CameraWebServer.ino`**:
   - Abre el archivo `CameraWebServer/CameraWebServer.ino`.
   - Configura los valores de Foundry en las siguientes variables:
     ```cpp
     const char* endpoint = "https://tu-endpoint-en-azure.com";
     const char* apiKey = "TU_API_KEY";
     const char* deploymentName = "NOMBRE_DEL_DEPLOYMENT";
     ```

---
---

## Instalación del Proyecto

1. **Clonar el repositorio**:
   ```bash
   git clone https://github.com/Dsniels/ajedrez-automatico.git
   cd ajedrez-automatico
   ```

2. **Subir el código a la placa ESP32**:
   - Abre el archivo `CameraWebServer/CameraWebServer.ino` en el Arduino IDE.
   - Sube el código a la placa ESP32.

3. **Subir el código de la pantalla LCD al Arduino**:
   - Abre `Pantalla/pantalla.ino` en el Arduino IDE.
   - Conecta el Arduino UNO a la computadora
   - Sube el código a la placa microcontroladora conectada a la pantalla LCD.

---

## Ejecución

1. **Iniciar el servidor de cámara**:
   - Una vez que cargues el código en el ESP32, abre el Monitor Serie en el Arduino IDE.
   - Anota la dirección IP asignada a la cámara (por ejemplo: `192.168.x.x`).
   - Abre un navegador web y accede a esa dirección para visualizar la transmisión en vivo.

2. **Realizar Conexiones**:
   - Conecta los pines de la pantalla LCD según el archivo `pantalla.ino`:
     - `RS`: Pin digital **12**
     - `E`: Pin digital **11**
     - `D4`: Pin digital **5**
     - `D5`: Pin digital **4**
     - `D6`: Pin digital **3**
     - `D7`: Pin digital **2**
   - Conecta el botón al pin digital **7**, asegurándote de configurarlo como `INPUT_PULLUP` en el código. 
    -   Pocisionar la camara encima del tablero de ajedrez, por medio de la transimision en vivo puedes ver si el tablero se ve por la camara
    - Una vez realizadas las conexiones, asegurate de conectar el ESP32-Cam y el arduino a la
3. **Ejecutar el programa de comunicación serial**:
   - Navega a la carpeta `SerialComunication` y ejecuta:
     ```bash
     dotnet run
     ```
    - Presciona el boton para mandar tomar la foto y mandarla al modelo
    - Despues de que la IA Procese la Imagen se mostrara en la pantalla LCD el movimiento que decidio.

    - Asegúrate de que los datos entre la cámara y la pantalla LCD fluyan correctamente. Puedes hacerlo viendo los logs del programa de dotnet en la computadora o viendo que la pantalla LCD muestre datos cada vez que se realiza una accion (presionar el boton o recibir respuesta de la IA).


    - Realiza tus movimientos y presiona el boton cada vez que sea turno del contrincante 


---

