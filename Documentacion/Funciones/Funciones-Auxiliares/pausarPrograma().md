Descripción: Detiene temporalmente la ejecución del programa hasta que el usuario presiona la tecla Enter. Reemplaza de forma eficiente y segura a la función no estándar system("pause").

Propósito: Ofrecer una solución nativa y multiplataforma (Windows, Linux, macOS) para congelar la pantalla de la terminal, evitando que la aplicación se cierre abruptamente al finalizar un proceso.

Funcionamiento interno:

std::cin.clear(): Limpia cualquier estado de error que haya podido ocurrir en lecturas previas de la consola.

std::cin.ignore(...): Vacía por completo el búfer de entrada de datos (stdin). Esto es crucial si previamente se usó std::cin >>, ya que descarta el carácter de nueva línea (\n) residual que haría que la pausa se saltara automáticamente.

std::cin.get(): Bloquea el hilo de ejecución del programa esperando la lectura de un carácter (en este caso, la pulsación de Enter), reanudando el programa inmediatamente después.
