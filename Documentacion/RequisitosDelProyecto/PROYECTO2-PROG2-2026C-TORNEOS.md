# REPÚBLICA BOLIVARIANA DE VENEZUELA
## UNIVERSIDAD RAFAEL URDANETA
## FACULTAD DE INGENIERÍA
## ESCUELA DE INGENIERÍA DE COMPUTACIÓN

**ASIGNATURA:** PROGRAMACIÓN 2  
**PROFESOR:** ING. VICTOR KNEIDER  
**TRIMESTRE:** 2026-C

---

# PROYECTO 2: SISTEMA DE GESTIÓN DE TORNEOS CON PERSISTENCIA Y ACCESO ALEATORIO

## DESCRIPCIÓN GENERAL

Evoluciona tu sistema de gestión de torneos del Proyecto 1 implementando **persistencia de datos mediante archivos binarios** y **acceso aleatorio**. El sistema debe demostrar dominio de operaciones de lectura/escritura binaria, navegación eficiente usando `seekg()/seekp()`, y gestión de relaciones entre múltiples archivos binarios **sin depender de la carga total en memoria (RAM)**.

---

## OBJETIVOS DE APRENDIZAJE

1. Implementar persistencia de datos con archivos binarios
2. Dominar operaciones de lectura/escritura binaria con `<fstream>`
3. Aplicar acceso aleatorio y cálculo de offsets con `seekg()` y `seekp()`
4. Comprender el mapeo entre estructuras en memoria y su representación en disco
5. Gestionar índices, posiciones y borrado lógico en archivos binarios
6. Implementar integridad referencial entre múltiples archivos mediante IDs
7. Mantener la separación de capas (lógica / presentación) del Proyecto 1

---

## FILOSOFÍA DE ACCESO ALEATORIO

### Concepto Fundamental

Este proyecto **NO carga todos los datos en memoria**. A diferencia del Proyecto 1, donde todo el sistema vivía en la RAM usando arreglos dinámicos, aquí los datos permanecen en disco y **solo se carga un registro a la vez cuando se necesita procesar**.

### Ciclo de Vida de una Operación

Para cualquier acción sobre un registro, tu programa debe seguir estrictamente este flujo:

```
1. Abrir el archivo binario correspondiente
2. Calcular la posición exacta en bytes del registro buscado
3. Desplazar el cursor (seekg / seekp)
4. Leer o escribir ÚNICAMENTE ese registro
5. Cerrar el archivo inmediatamente
```

### Ventajas de este enfoque

- **Eficiencia de Memoria:** El uso de RAM es mínimo sin importar si hay 10 o 100.000 registros
- **Persistencia Inmediata:** Si el programa crashea, los datos ya escritos están seguros en disco

---

## 1. MODELO DE PERSISTENCIA Y ARQUITECTURA

El sistema debe crear **un archivo binario independiente por cada estructura principal**:

| Archivo | Contenido |
|---------|-----------|
| `torneo.bin` | Un único registro con los datos del torneo |
| `equipos.bin` | Todos los equipos registrados |
| `jugadores.bin` | Todos los jugadores registrados |
| `partidos.bin` | Todos los partidos (programados, jugados, cancelados) |

### 1.1 Estructura Interna de los Archivos

Cada archivo binario tiene un **Header** al inicio seguido de los registros:

```
[HEADER: 16 bytes]   →  Metadata administrativa del archivo
[REGISTRO 0]
[REGISTRO 1]
...
[REGISTRO N]
```

**Estructura obligatoria del Header:**

```cpp
struct ArchivoHeader {
    int cantidadRegistros;  // Total histórico (incluyendo eliminados lógicamente)
    int proximoID;          // Siguiente ID a asignar (autoincremental)
    int registrosActivos;   // Registros con eliminado == false
    int version;            // Control de versión del archivo (iniciar en 1)
};
```

El Header evita recorrer todo el archivo para saber cuántos registros hay o cuál es el próximo ID.

> **Nota sobre `torneo.bin`:** Este archivo almacena un único registro de tipo `Torneo` y no requiere Header con IDs, ya que nunca tendrá más de un elemento. Solo necesita existir y poder leerse/escribirse.

---

## 2. ADAPTACIÓN DE ESTRUCTURAS PARA PERSISTENCIA

Para guardar estructuras en archivos binarios **no pueden contener punteros, arreglos dinámicos ni `std::string`**. Su tamaño (`sizeof`) debe ser estático y predecible en tiempo de compilación.

### 2.1 Reglas Generales de Adaptación

- Usar `char[]` estáticos, nunca `std::string`
- Usar `int[]` estáticos para relaciones (IDs de otros registros)
- Agregar `bool eliminado` a toda entidad (borrado lógico obligatorio)
- Agregar `time_t fechaCreacion` y `time_t fechaUltimaModificacion`
- Eliminar todos los punteros (`Equipo*`, etc.)

### 2.2 Estructura Torneo (sin cambios funcionales)

```cpp
struct Torneo {
    char nombre[100];
    char deporte[50];
    char formato[20];
    char fechaInicio[11];
    char fechaFin[11];
    // Metadata de control
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};
```

### 2.3 Estructura Equipo (adaptada para archivos)

```cpp
struct Equipo {
    // Datos básicos
    int  id;
    char nombre[100];
    char ciudad[100];
    char entrenador[100];

    // Estadísticas del torneo
    int  puntos;
    int  victorias;
    int  empates;
    int  derrotas;
    int  golesAFavor;
    int  golesEnContra;

    // Relaciones: IDs de partidos en que participó este equipo
    int  partidosIDs[50];       // Máximo 50 partidos por equipo
    int  cantidadPartidos;

    // Metadata de control
    bool     eliminado;
    time_t   fechaCreacion;
    time_t   fechaUltimaModificacion;
};
```

### 2.4 Estructura Jugador (adaptada para archivos)

```cpp
struct Jugador {
    // Datos básicos
    int  id;
    int  idEquipo;
    char nombre[100];
    char cedula[20];
    char posicion[20];
    int  edad;
    int  numeroDorsal;

    // Estadísticas individuales (se actualizan al registrar goles en partidos)
    int  golesAnotados;
    int  tarjetasAmarillas;
    int  tarjetasRojas;

    // Metadata de control
    bool     eliminado;
    time_t   fechaCreacion;
    time_t   fechaUltimaModificacion;
};
```

### 2.5 Estructura Gol (embebida en Partido)

En el Proyecto 1, un partido registraba únicamente el marcador final (golesLocal, golesVisitante). A partir de esta versión, **el partido debe almacenar el detalle de cada gol**: quién anotó, en qué minuto y para qué equipo. Esto ejerce el mismo concepto que una transacción con múltiples líneas de producto.

```cpp
struct Gol {
    int  idJugador;       // ID del jugador que anotó (0 = desconocido / gol en contra)
    int  minuto;          // Minuto del partido en que se anotó (1 - 120)
    char equipo[12];      // "LOCAL" o "VISITANTE"
};
```

### 2.6 Estructura Partido (adaptada con detalle de goles)

```cpp
struct Partido {
    // Datos básicos
    int  id;
    int  idEquipoLocal;
    int  idEquipoVisitante;
    char fecha[11];
    char estado[12];       // "PROGRAMADO", "JUGADO", "CANCELADO"
    char descripcion[200];

    // Marcador global (para acceso rápido sin recorrer el array de goles)
    int  golesLocal;
    int  golesVisitante;

    // Detalle de goles (NUEVO en Proyecto 2)
    Gol  goles[22];        // Máximo 22 goles por partido
    int  numGoles;

    // Metadata de control
    bool     eliminado;
    time_t   fechaCreacion;
    time_t   fechaUltimaModificacion;
};
```

> **Importante:** `sizeof(Partido)` incluye los 22 slots de `Gol` siempre, aunque el partido tenga 0 goles. Esto garantiza que todos los registros en el archivo ocupen exactamente el mismo espacio, lo que hace posible el acceso aleatorio por cálculo matemático.

---

## 3. LÓGICA DE OPERACIONES CON ARCHIVOS (CRUD)

### 3.1 Inicialización y Gestión de Headers

Implementar las siguientes funciones aisladas para operar sobre el header de cualquier archivo:

```cpp
// Crea el archivo si no existe y escribe un ArchivoHeader en cero
// Retorna true si el archivo quedó listo para usar
bool inicializarArchivo(const char* nombreArchivo);

// Abre el archivo, lee los primeros sizeof(ArchivoHeader) bytes y retorna el header
ArchivoHeader leerHeader(const char* nombreArchivo);

// Sobrescribe únicamente la sección del header (posición 0) sin tocar los registros
// Retorna true si se actualizó correctamente
bool actualizarHeader(const char* nombreArchivo, ArchivoHeader header);
```

Al iniciar el programa, deben inicializarse los 4 archivos si no existen:

```cpp
bool inicializarSistemaArchivos() {
    return inicializarArchivo("datos/torneo.bin")    &&
           inicializarArchivo("datos/equipos.bin")   &&
           inicializarArchivo("datos/jugadores.bin") &&
           inicializarArchivo("datos/partidos.bin");
}
```

### 3.2 Acceso Aleatorio y Cálculo de Offsets

La fórmula matemática para ubicar el registro en índice físico `i` sin recorrer el archivo es:

```
posición = sizeof(ArchivoHeader) + (i * sizeof(TuEstructura))
```

> **Importante:** El índice físico (`i = 0, 1, 2...`) NO es igual al ID del registro, especialmente si hay borrados lógicos entre medio. Necesitas una función de búsqueda que reciba un ID y retorne el índice físico donde está guardado.

```cpp
// Retorna el índice físico (0-based) del registro con ese ID
// Retorna -1 si no se encuentra o está eliminado
int buscarIndicePorID(const char* nombreArchivo, int id, int tamanoRegistro);
```

Esta función recorre el archivo registro por registro comparando el campo `id`. Aunque es secuencial, opera con **un solo registro en RAM** a la vez.

### 3.3 Creación de un Registro

```
1. Leer el header → obtener proximoID
2. Asignar ese ID al nuevo registro y marcar eliminado = false
3. Posicionar el cursor al final del archivo (seekp con ios::end)
4. Escribir la estructura completa
5. Actualizar header: cantidadRegistros++, registrosActivos++, proximoID++
6. Guardar el header actualizado
```

### 3.4 Actualización y Borrado Lógico

```
1. Buscar el índice físico del registro mediante su ID
2. Calcular la posición en bytes: sizeof(Header) + (indice * sizeof(Estructura))
3. Posicionar el cursor de escritura (seekp) en ese byte exacto
4. Escribir la estructura modificada sobrescribiendo los datos anteriores
   (Para borrado: misma operación pero con eliminado = true)
5. Si fue borrado: actualizar registrosActivos-- en el header
```

---

## 4. FUNCIONES OBLIGATORIAS DE PERSISTENCIA

Todas las operaciones de archivo deben estar separadas de la lógica de negocio. La estructura recomendada es tener funciones específicas por entidad, agrupadas en un módulo de persistencia.

### 4.1 Funciones para Equipos

```cpp
// Guarda un nuevo equipo al final del archivo
// Asigna ID automáticamente desde el header
// Retorna true si se guardó correctamente
bool guardarEquipo(Equipo& equipo);

// Lee el equipo con ese ID desde el archivo
// Escribe el resultado en 'resultado'
// Retorna true si se encontró y no está eliminado
bool leerEquipoPorID(int id, Equipo& resultado);

// Sobrescribe el equipo en su posición física original
// Retorna true si se actualizó correctamente
bool actualizarEquipo(Equipo& equipo);

// Marca el equipo como eliminado (borrado lógico)
// Actualiza el header (registrosActivos--)
// Retorna true si se marcó correctamente
bool eliminarEquipoLogico(int id);

// Retorna la cantidad de equipos activos
int contarEquiposActivos();
```

> Replicar este patrón de 5 funciones para `Jugador` y `Partido`.

### 4.2 Operación Compuesta: Registrar Resultado de Partido

Esta operación involucra **múltiples accesos a disco coordinados** para mantener coherencia. Si algún paso falla, los demás no deben ejecutarse.

```
Pasos obligatorios al registrar el resultado de un partido:

1. Leer el Partido desde partidos.bin → validar que existe y está en "PROGRAMADO"
2. Leer el Equipo Local desde equipos.bin
3. Leer el Equipo Visitante desde equipos.bin
4. Determinar resultado (victoria local / empate / victoria visitante)
5. Actualizar estadísticas de ambos equipos en memoria
6. Agregar el ID del partido al arreglo partidosIDs[] de cada equipo
7. Actualizar el estado del Partido a "JUGADO", registrar goles y detalle de goles
8. Actualizar las estadísticas de los jugadores anotadores en jugadores.bin
9. Escribir Partido actualizado en partidos.bin
10. Escribir Equipo Local actualizado en equipos.bin
11. Escribir Equipo Visitante actualizado en equipos.bin
```

### 4.3 Operación Compuesta: Cancelar Partido Jugado

```
1. Leer el Partido → validar que existe y está en "JUGADO"
2. Leer Equipo Local y Equipo Visitante desde sus archivos
3. Revertir estadísticas: deshacer victorias/empates/derrotas/goles de ambos
4. Revertir goles de los jugadores anotadores registrados en partido.goles[]
5. Cambiar estado del Partido a "CANCELADO", limpiar goles
6. Escribir los 3 registros actualizados en sus respectivos archivos
```

---

## 5. FUNCIONES DE LÓGICA ADAPTADAS (Sin arrays dinámicos en memoria)

A diferencia del Proyecto 1, las funciones de lógica ya no retornan `Entidad*` porque no hay arrays dinámicos en RAM. El patrón cambia: las funciones **reciben una referencia** donde escribir el resultado, y retornan `bool` para indicar éxito o fallo.

### 5.1 Funciones de Búsqueda Individual

```cpp
// Busca un equipo por ID en equipos.bin
// Escribe el resultado en 'equipo' si se encuentra
// Retorna true si existe y no está eliminado, false en caso contrario
bool obtenerEquipoPorID(int id, Equipo& equipo);

// Mismo patrón para Jugador y Partido
bool obtenerJugadorPorID(int id, Jugador& jugador);
bool obtenerPartidoPorID(int id, Partido& partido);
```

### 5.2 Funciones de Búsqueda Múltiple

Cuando se necesita retornar múltiples resultados se usa un **array estático temporal**, ya que los datos viven en disco, no en RAM.

```cpp
// Llena el array 'resultados' con equipos cuyo nombre contiene 'subcadena'
// 'maxResultados' limita cuántos se cargan en memoria a la vez
// Retorna la cantidad de equipos encontrados
int buscarEquiposPorNombre(const char* subcadena,
                           Equipo resultados[], int maxResultados);

// Llena el array con jugadores del equipo indicado
int listarJugadoresPorEquipo(int idEquipo,
                              Jugador resultados[], int maxResultados);

// Llena el array con los partidos en que participó el equipo
int listarPartidosPorEquipo(int idEquipo,
                             Partido resultados[], int maxResultados);

// Llena el array con partidos filtrados por estado
int listarPartidosPorEstado(const char* estado,
                             Partido resultados[], int maxResultados);
```

> Usar tamaños máximos razonables (ej. `MAX_RESULTADOS = 100`) definidos como constantes.

---

## 6. MANTENIMIENTO E INTEGRIDAD (NUEVAS FUNCIONALIDADES)

### 6.1 Verificación de Integridad Referencial

Implementar la función `verificarIntegridadReferencial()` que detecte referencias rotas entre archivos:

**¿Qué debe detectar?**

```
a) Recorrer jugadores.bin:
   → Por cada jugador activo, verificar que su idEquipo exista en equipos.bin y no esté eliminado
   → Si no existe: reportar "ROTO: Jugador ID X apunta a Equipo ID Y (no existe)"

b) Recorrer partidos.bin:
   → Por cada partido activo, verificar que idEquipoLocal exista y no esté eliminado
   → Verificar que idEquipoVisitante exista y no esté eliminado
   → Si alguno no existe: reportar el error

c) Recorrer partidos.bin (detalle de goles):
   → Por cada gol con idJugador > 0, verificar que ese jugador exista y no esté eliminado
   → Reportar cualquier referencia rota
```

Al finalizar, imprimir un reporte de salud:

```
╔═══════════════════════════════════════════════════════╗
║         REPORTE DE INTEGRIDAD REFERENCIAL             ║
╠═══════════════════════════════════════════════════════╣
║  Equipos verificados   : 6                            ║
║  Jugadores verificados : 24                           ║
║  Partidos verificados  : 10                           ║
║                                                       ║
║  Referencias rotas encontradas: 0                     ║
║  Estado: ✓ SISTEMA ÍNTEGRO                            ║
╚═══════════════════════════════════════════════════════╝
```

### 6.2 Respaldo de Datos (Backup)

Implementar `crearBackup()` que:

1. Genere un nombre de carpeta con la fecha y hora actual (ej: `backup_2026-03-15_14-32`)
2. Copie byte a byte los 4 archivos `.bin` a esa carpeta
3. Informe al usuario el resultado y la ruta del backup

```cpp
bool crearBackup();
```

### 6.3 Reportes Analíticos

Implementar un submenú de **Reportes** con las siguientes opciones:

#### Reporte 1: Tabla de Posiciones (desde archivo)

Leer todos los equipos activos desde `equipos.bin` y mostrarlos ordenados por:
criterio 1 → puntos DESC, criterio 2 → diferencia de goles DESC, criterio 3 → goles a favor DESC.

#### Reporte 2: Tabla de Goleadores

Leer todos los jugadores activos desde `jugadores.bin` y mostrar los 10 con más `golesAnotados`, junto con el nombre de su equipo (que debe leerse desde `equipos.bin`).

```
╔══════════════════════════════════════════════════════╗
║               TABLA DE GOLEADORES                    ║
╠════╦══════════════════╦══════════════╦═══════╗
║ #  ║ Jugador          ║ Equipo       ║ Goles ║
╠════╬══════════════════╬══════════════╬═══════╣
║  1 ║ Carlos Mendez    ║ Dep. Mbo     ║   7   ║
║  2 ║ Pedro Rojas      ║ Caracas FC   ║   5   ║
╚════╩══════════════════╩══════════════╩═══════╝
```

#### Reporte 3: Historial de Partido (Ficha Técnica)

Dado el ID de un partido, leer su registro desde `partidos.bin` y mostrar el detalle completo: nombres de equipos (desde `equipos.bin`), marcador, y por cada gol registrado en `partido.goles[]` mostrar el nombre del jugador (desde `jugadores.bin`), el minuto y el equipo.

```
╔══════════════════════════════════════════════════════════╗
║              FICHA TÉCNICA DEL PARTIDO                   ║
╠══════════════════════════════════════════════════════════╣
║  Partido ID  : 3          Fecha: 2026-03-15              ║
║  Estado      : JUGADO                                    ║
╠══════════════════════════════════════════════════════════╣
║  Deportivo Maracaibo   2  -  1   Caracas FC              ║
╠══════════════════════════════════════════════════════════╣
║  GOLES:                                                  ║
║  [LOCAL]     Min. 23 - Carlos Mendez                     ║
║  [LOCAL]     Min. 67 - Luis Torres                       ║
║  [VISITANTE] Min. 80 - Pedro Rojas                       ║
╚══════════════════════════════════════════════════════════╝
```

---

## 7. EXPERIENCIA DE USUARIO

### 7.1 Formato de Datos

Todos los datos mostrados deben estar correctamente formateados en tablas con caracteres de borde. Los IDs internos nunca deben mostrarse solos: siempre acompañados del nombre de la entidad correspondiente (leer desde el archivo).

**Incorrecto:**
```
Partido 3 | EquipoLocal: 1 | EquipoVisitante: 2
```

**Correcto:**
```
Partido 3 | Deportivo Maracaibo vs Caracas FC
```

### 7.2 Practicidad

Cuando se muestre información de un jugador, siempre debe incluirse el nombre de su equipo (no solo el ID). Cuando se muestre un partido, los nombres de ambos equipos deben leerse desde `equipos.bin` y mostrarse completos.

---

## 8. MENÚ ACTUALIZADO DEL SISTEMA

```
╔═══════════════════════════════════════════╗
║    SISTEMA DE GESTIÓN DE TORNEOS          ║
║    Liga Apertura 2026 - Fútbol            ║
╠═══════════════════════════════════════════╣
║  1. Gestión de Equipos                    ║
║  2. Gestión de Jugadores                  ║
║  3. Gestión de Partidos                   ║
║  4. Tabla de Posiciones                   ║
║  5. Reportes                              ║
║  6. Mantenimiento                         ║
║  0. Salir                                 ║
╚═══════════════════════════════════════════╝
```

#### Submenú: Reportes (nuevo en P2)

```
╔═══════════════════════════════════════════╗
║              REPORTES                     ║
╠═══════════════════════════════════════════╣
║  1. Tabla de posiciones                   ║
║  2. Tabla de goleadores (Top 10)          ║
║  3. Ficha técnica de partido              ║
║  0. Volver                                ║
╚═══════════════════════════════════════════╝
```

#### Submenú: Mantenimiento (nuevo en P2)

```
╔═══════════════════════════════════════════╗
║           MANTENIMIENTO                   ║
╠═══════════════════════════════════════════╣
║  1. Verificar integridad referencial      ║
║  2. Crear backup de datos                 ║
║  0. Volver                                ║
╚═══════════════════════════════════════════╝
```

---

## 9. CASOS DE USO OBLIGATORIOS

#### Caso de Uso 1: Persistencia entre Ejecuciones

```
1. Registrar 3 equipos y 6 jugadores
2. Cerrar el programa completamente
3. Volver a ejecutarlo
4. Verificar que todos los datos siguen presentes y con los mismos IDs
```

#### Caso de Uso 2: Acceso Aleatorio Verificado

```
1. Registrar 6 equipos (IDs del 1 al 6)
2. Eliminar lógicamente el equipo con ID 3
3. Buscar el equipo con ID 5 → debe encontrarse en 1 sola operación (no recorrido)
4. Demostrar (con un mensaje de debug) la posición en bytes donde se leyó:
   sizeof(Header) + (4 * sizeof(Equipo))
```

#### Caso de Uso 3: Registro de Resultado con Detalle de Goles

```
1. Programar partido: Deportivo Maracaibo (local) vs Caracas FC (visitante)
2. Registrar resultado 2-1 con el siguiente detalle:
   - Gol LOCAL  en minuto 23, jugador Carlos Mendez
   - Gol LOCAL  en minuto 67, jugador Luis Torres
   - Gol VISIT. en minuto 80, jugador Pedro Rojas
3. Verificar que las estadísticas de los 3 jugadores se actualizaron en jugadores.bin
4. Ver ficha técnica del partido y confirmar que los goles aparecen con nombre y minuto
```

#### Caso de Uso 4: Cancelación con Reversión

```
1. Cancelar el partido del Caso de Uso 3
2. Verificar que el estado cambió a "CANCELADO"
3. Verificar que los goles de Carlos Mendez, Luis Torres y Pedro Rojas se revirtieron
4. Verificar que los puntos de ambos equipos volvieron a su valor anterior
```

#### Caso de Uso 5: Integridad Referencial

```
1. Crear equipo con ID 4, crear jugador asociado a ID 4
2. Eliminar lógicamente el equipo ID 4
3. Ejecutar verificarIntegridadReferencial()
4. Verificar que el reporte detecta la referencia rota del jugador
```

#### Caso de Uso 6: Backup y Restauración

```
1. Registrar datos de prueba (mínimo 3 equipos, 2 partidos)
2. Ejecutar crearBackup()
3. Verificar que la carpeta de backup contiene los 4 archivos .bin
4. Confirmar que los archivos de backup tienen el mismo tamaño que los originales
```

#### Caso de Uso 7: Reportes desde Archivo

```
1. Jugar 5 partidos entre 4 equipos distintos con detalle de goles
2. Generar tabla de posiciones → verificar orden correcto
3. Generar tabla de goleadores → verificar que Carlos Mendez aparece primero (3 goles)
4. Mostrar ficha técnica de un partido jugado → verificar nombres y minutos
```

---

## 10. VALIDACIONES Y MANEJO DE ERRORES

Las validaciones del Proyecto 1 se mantienen. Se agregan las siguientes específicas de archivos:

| Situación | Manejo |
|-----------|--------|
| Archivo no existe al iniciar | Crearlo automáticamente con `inicializarArchivo()` |
| Fallo al abrir archivo | Mensaje de error claro, no continuar la operación |
| ID no encontrado en archivo | Retornar `false` / `-1`, la presentación muestra el error |
| Partido en estado incorrecto para la operación | Mensaje específico indicando el estado actual |
| `cantidadPartidos == 50` en un equipo | Advertir que se alcanzó el límite de historial |
| `numGoles == 22` en un partido | Advertir que se alcanzó el límite de goles registrables |

---

## 11. DIFERENCIAS CLAVE CON EL PROYECTO 1

| Aspecto | Proyecto 1 | Proyecto 2 |
|---------|-----------|-----------|
| **Almacenamiento** | RAM (arrays dinámicos) | Disco (archivos binarios) |
| **Carga de datos** | Todo en memoria al inicio | Un registro a la vez |
| **Persistencia** | Se pierde al cerrar | Sobrevive entre ejecuciones |
| **Retorno de funciones** | `Entidad*` o `Entidad**` | `bool` + parámetro por referencia |
| **Búsqueda** | Recorre array en RAM | Recorre archivo con un registro en RAM |
| **Partido** | Solo marcador global | Marcador + detalle de goles (struct Gol[]) |
| **Borrado** | Fisico (mover elementos) | Lógico (`eliminado = true`) |
| **Nuevas funciones** | — | Backup, integridad, reportes desde disco |

---

## 12. ENTREGABLES

### 12.1 Código Fuente

- Archivo `.cpp` principal (o modularizado en `.h/.cpp`)
- Nomenclatura descriptiva y comentarios explicando los cálculos de offsets en bytes

### 12.2 Archivos de Datos de Prueba

La entrega debe incluir los archivos `.bin` pre-poblados con al menos:

| Archivo | Mínimo |
|---------|--------|
| `equipos.bin` | 5 equipos activos |
| `jugadores.bin` | 15 jugadores (distribuidos entre equipos) |
| `partidos.bin` | 10 partidos (mezcla de JUGADO / PROGRAMADO) |

### 12.3 Documentación Técnica (`README_V2.md`)

- Diagrama de las estructuras y su tamaño exacto en bytes (`sizeof`)
- Fórmulas de cálculo de offset usadas
- Manual de usuario básico
- Instrucciones de compilación y ejecución

---

## 13. PREGUNTAS FRECUENTES

**P: ¿Por qué `Partido` tiene `numGoles` si también tiene `golesLocal` y `golesVisitante`?**  
R: `golesLocal` y `golesVisitante` son el marcador global para acceso rápido (sin recorrer el array). `numGoles` es el contador de entradas reales en `goles[]`. Ambos deben mantenerse sincronizados.

**P: ¿Qué pasa si `idJugador` en un `Gol` es 0?**  
R: Significa que el gol es en propia puerta o que el jugador anotador no fue registrado. La ficha técnica debe mostrarlo como "Jugador desconocido".

**P: ¿Por qué las funciones de búsqueda múltiple reciben un array fijo en lugar de retornar `Entidad**`?**  
R: Porque ya no hay arrays dinámicos en RAM. El arreglo estático temporal es la forma segura de manejar múltiples resultados cuando los datos viven en disco. El llamador declara el buffer y lo pasa como parámetro.

**P: ¿Debo verificar integridad en cada operación CRUD?**  
R: No. La verificación de integridad es una operación de mantenimiento bajo demanda. En cada operación CRUD solo validas lo mínimo necesario (ej: que el equipo exista antes de registrar un jugador).

**P: ¿Puedo usar `fopen/fclose` en lugar de `fstream`?**  
R: Debes usar `<fstream>` y los métodos de la librería estándar de C++ (`ifstream`, `ofstream`, `fstream`, `seekg`, `seekp`, `read`, `write`).

---

## NOTAS FINALES

Este proyecto evalúa tu capacidad para:
1. Mapear estructuras de C++ a bytes en disco y recuperarlas sin error
2. Calcular posiciones en archivos matemáticamente (no por búsqueda secuencial completa)
3. Coordinar múltiples operaciones de archivo en una sola acción de negocio
4. Mantener la separación de capas: las funciones de archivo no tienen `cout`, las de presentación no abren archivos directamente
5. Garantizar consistencia de datos ante operaciones compuestas (registrar resultado = actualizar partido + 2 equipos + N jugadores)

**¡Éxito en tu proyecto!**

---

**Ing. Victor Kneider**  
Profesor de Programación 2  
Universidad Rafael Urdaneta
