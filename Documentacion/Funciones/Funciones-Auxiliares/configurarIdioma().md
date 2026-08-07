## Descripción:

##### Establece la configuración regional (locale) del sistema y ajusta los parámetros de codificación de la terminal para garantizar la correcta visualización de caracteres especiales del idioma español (como acentos, eñes, diéresis y signos de apertura ¡, ¿).

## Propósito:

##### Resolver el problema de incompatibilidad de codificación entre diferentes sistemas operativos (Windows, Linux y macOS), unificando la salida hacia el estándar UTF-8 de manera automática y transparente para el usuario.

## Funcionamiento interno:

### Localización global (std::locale):

##### Sincroniza el flujo de ejecución del programa con el idioma y la región configurados nativamente en el sistema operativo del usuario.

### Inyección en flujo (std::cout.imbue):

##### Aplica de forma específica las reglas de esa localización al canal de salida estándar (std::cout).

### Ajuste de Consola en Windows (SetConsoleCP / SetConsoleOutputCP):

##### Detecta mediante macros si el programa corre en entornos Windows y, de ser así, fuerza a la consola (CMD/PowerShell) a cambiar su clásica e incompatible tabla de códigos (Code Page) activa a la página 65001, correspondiente al estándar global UTF-8. En sistemas Unix/Linux o macOS este paso se omite de forma segura ya que dichos sistemas operan nativamente en UTF-8.
