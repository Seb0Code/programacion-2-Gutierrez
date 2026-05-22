# REPÚBLICA BOLIVARIANA DE VENEZUELA
## UNIVERSIDAD RAFAEL URDANETA
## FACULTAD DE INGENIERÍA
## ESCUELA DE INGENIERÍA DE COMPUTACIÓN

**ASIGNATURA:** PROGRAMACIÓN 2  
**PROFESOR:** ING. VICTOR KNEIDER  
**TRIMESTRE:** 2026-C

---

# PROYECTO 1: SISTEMA DE GESTIÓN DE TORNEOS DEPORTIVOS

## DESCRIPCIÓN GENERAL

Desarrolla un sistema de gestión de torneos deportivos que permita administrar equipos, jugadores y resultados de partidos. El sistema debe demostrar dominio avanzado de **punteros, memoria dinámica y estructuras** en C++, implementando operaciones CRUD completas, manejo robusto de memoria y una **separación clara entre la capa de lógica y la capa de presentación** desde el primer proyecto.

## OBJETIVOS DE APRENDIZAJE

1. Implementar estructuras de datos dinámicas con punteros
2. Gestionar memoria dinámica de forma eficiente y segura
3. Aplicar operaciones CRUD (Crear, Leer, Actualizar, Eliminar)
4. Desarrollar funciones que retornan punteros y arrays de punteros
5. Separar la lógica de negocio de la presentación al usuario
6. Implementar redimensionamiento automático de arrays dinámicos
7. Aplicar buenas prácticas de programación y documentación

## TECNOLOGÍAS REQUERIDAS

- **Lenguaje:** C++ (estándar C++11 o superior)
- **Compilador:** g++ o cualquier compilador compatible
- **Gestión de memoria:** Manual (new/delete)

---

## ESPECIFICACIONES TÉCNICAS

### 1. ESTRUCTURAS DE DATOS OBLIGATORIAS

Se propone una estructura base con libertad para añadir atributos según criterio propio.

#### 1.1 Estructura Torneo (Configuración del Sistema)

La estructura `Torneo` **no tiene CRUD propio**. Se inicializa una única vez al arrancar el sistema y actúa como contexto global (equivale al rol de `Tienda` en sistemas comerciales).

```cpp
struct Torneo {
    char nombre[100];       // Nombre del torneo (ej: "Liga Apertura 2026")
    char deporte[50];       // Deporte (ej: "Fútbol", "Baloncesto")
    char formato[20];       // "GRUPOS" o "ELIMINATORIA"
    char fechaInicio[11];   // Formato: YYYY-MM-DD
    char fechaFin[11];      // Formato: YYYY-MM-DD
};
```

#### 1.2 Estructura Equipo

```cpp
struct Equipo {
    int  id;                // Identificador único (autoincremental)
    char nombre[100];       // Nombre del equipo
    char ciudad[100];       // Ciudad de origen
    char entrenador[100];   // Nombre del entrenador
    int  puntos;            // Puntos acumulados en el torneo
    int  victorias;         // Partidos ganados
    int  empates;           // Partidos empatados
    int  derrotas;          // Partidos perdidos
    int  puntosAFavor;       // Total de puntos anotados
    int  puntosEnContra;     // Total de puntos recibidos
    char fechaRegistro[11]; // Formato: YYYY-MM-DD
};
```

#### 1.3 Estructura Jugador

```cpp
struct Jugador {
    int  id;                // Identificador único (autoincremental)
    int  idEquipo;          // ID del equipo al que pertenece
    char nombre[100];       // Nombre completo
    char cedula[20];        // Cédula de identidad
    char posicion[20];      // "PORTERO", "DEFENSA", "MEDIOCAMPISTA", "DELANTERO"
    int  edad;              // Edad del jugador
    int  numeroDorsal;      // Número en la camiseta (1-99)
    char fechaRegistro[11]; // Formato: YYYY-MM-DD
};
```

#### 1.4 Estructura Partido (CASO ESPECIAL: representa el evento central del sistema)

```cpp
struct Partido {
    int  id;                  // Identificador único (autoincremental)
    int  idEquipoLocal;       // ID del equipo local
    int  idEquipoVisitante;   // ID del equipo visitante
    int  puntosLocal;          // puntos del equipo local (0 si no jugado)
    int  puntosVisitante;      // puntos del equipo visitante (0 si no jugado)
    char fecha[11];           // Formato: YYYY-MM-DD
    char estado[12];          // "PROGRAMADO", "JUGADO", "CANCELADO"
    char descripcion[200];    // Notas adicionales (opcional)
};
```

#### 1.5 Estructura Principal: SistemaDeportivo

```cpp
struct SistemaDeportivo {
    Torneo torneo;              // Datos del torneo (único, no es array)

    // Arrays dinámicos de entidades
    Equipo*  equipos;
    int      numEquipos;
    int      capacidadEquipos;

    Jugador* jugadores;
    int      numJugadores;
    int      capacidadJugadores;

    Partido* partidos;
    int      numPartidos;
    int      capacidadPartidos;

    // Contadores para IDs autoincrementales
    int siguienteIdEquipo;
    int siguienteIdJugador;
    int siguienteIdPartido;
};
```

---

### 2. ARQUITECTURA OBLIGATORIA: DOS CAPAS

Este proyecto exige separar explícitamente la **capa de lógica** de la **capa de presentación**. Esta separación será la base de los proyectos 2 y 3.

```
┌─────────────────────────────────────────────────┐
│               CAPA DE PRESENTACIÓN              │
│  Funciones void  │  cin / cout  │  Menús        │
│  Llaman a la lógica, muestran resultados        │
├─────────────────────────────────────────────────┤
│               CAPA DE LÓGICA                    │
│  Funciones que retornan Entidad* o bool         │
│  Operan sobre SistemaDeportivo*                 │
│  NO usan cin / cout                             │
└─────────────────────────────────────────────────┘
```

#### Regla fundamental

> **Las funciones de lógica NO deben contener `cin` ni `cout`.**  
> **Las funciones de presentación NO deben modificar datos directamente.**

#### Ejemplo de flujo correcto

```
menuRegistrarEquipo()           ← presentación: solicita datos al usuario
    │
    ├─ valida que nombre no esté vacío
    │
    └─ agregarEquipo(s, nombre, ciudad, entrenador)   ← lógica
            │
            ├─ verifica nombre duplicado
            ├─ redimensiona si es necesario
            ├─ asigna ID autoincremental y fecha actual
            └─ retorna Equipo* (éxito) o nullptr (fallo)
    │
    └─ mostrarEquipo(resultado)     ← presentación: imprime con cout
```

---

### 3. FUNCIONES OBLIGATORIAS

#### 3.1 Funciones de Inicialización y Liberación

```cpp
// Inicializa el sistema con los datos del torneo
void inicializarSistema(SistemaDeportivo* s, Torneo torneo);

// Libera toda la memoria dinámica
void liberarSistema(SistemaDeportivo* s);
```

**Requisitos:**
- Capacidad inicial de cada array: 4 elementos
- Todos los contadores en 0
- Los IDs comienzan en 1

---

#### 3.2 Funciones de Redimensionamiento

```cpp
void redimensionarEquipos(SistemaDeportivo* s);
void redimensionarJugadores(SistemaDeportivo* s);
void redimensionarPartidos(SistemaDeportivo* s);
```

**Requisito:** Duplicar la capacidad cuando `num == capacidad`. Copiar datos, liberar el array anterior y actualizar el puntero.

---

#### 3.3 Funciones de Lógica — EQUIPOS

Estas funciones operan sobre los datos. **No usan cin/cout.**

```cpp
// Agrega un equipo al sistema
// Retorna puntero al equipo creado, o nullptr si el nombre ya existe
Equipo* agregarEquipo(SistemaDeportivo* s, const char* nombre,
                      const char* ciudad, const char* entrenador);

// Retorna puntero al equipo con ese ID, o nullptr si no existe
Equipo* buscarEquipoPorID(SistemaDeportivo* s, int id);

// Retorna array de punteros a equipos cuyo nombre contiene la subcadena
// Escribe la cantidad de resultados en *cantidad
// El llamador es responsable de liberar el array con delete[]
Equipo** buscarEquiposPorNombre(SistemaDeportivo* s, const char* subcadena,
                                int* cantidad);

// Retorna array con punteros a todos los equipos activos
// Escribe la cantidad en *cantidad
// El llamador es responsable de liberar el array con delete[]
Equipo** listarEquipos(SistemaDeportivo* s, int* cantidad);

// Retorna array de equipos ordenados por puntos (tabla de posiciones)
// Criterio: puntos DESC → diferencia de puntos DESC → puntos a favor DESC
// El llamador es responsable de liberar el array con delete[]
Equipo** generarTablaPosiciones(SistemaDeportivo* s, int* cantidad);

// Actualiza los datos de un equipo (excepto estadísticas de juego)
// Retorna true si se actualizó, false si el ID no existe
bool actualizarEquipo(SistemaDeportivo* s, int id, Equipo equipoActualizado);

// Elimina un equipo solo si no tiene partidos asociados
// Retorna true si se eliminó, false si no existe o tiene partidos
bool eliminarEquipo(SistemaDeportivo* s, int id);
```

#### 3.4 Funciones de Presentación — EQUIPOS

Estas funciones manejan cin/cout y llaman a las funciones de lógica.

```cpp
void menuRegistrarEquipo(SistemaDeportivo* s);
void menuBuscarEquipo(SistemaDeportivo* s);
void menuActualizarEquipo(SistemaDeportivo* s);
void menuEliminarEquipo(SistemaDeportivo* s);
void menuListarEquipos(SistemaDeportivo* s);
void mostrarEquipo(Equipo* equipo);
void mostrarListaEquipos(Equipo** equipos, int cantidad);
void mostrarTablaPosiciones(SistemaDeportivo* s);
```

---

#### 3.5 Funciones de Lógica — JUGADORES

```cpp
// Agrega un jugador al equipo indicado
// Valida que el equipo exista, cédula no duplicada, dorsal no duplicado en el equipo
// Retorna puntero al jugador creado, o nullptr si falla alguna validación
Jugador* agregarJugador(SistemaDeportivo* s, int idEquipo, const char* nombre,
                        const char* cedula, const char* posicion,
                        int edad, int numeroDorsal);

// Retorna puntero al jugador con ese ID, o nullptr si no existe
Jugador* buscarJugadorPorID(SistemaDeportivo* s, int id);

// Retorna array de punteros a jugadores cuyo nombre contiene la subcadena
// Escribe la cantidad en *cantidad
// El llamador libera el array con delete[]
Jugador** buscarJugadoresPorNombre(SistemaDeportivo* s, const char* subcadena,
                                   int* cantidad);

// Retorna array con todos los jugadores pertenecientes al equipo indicado
// Escribe la cantidad en *cantidad
// El llamador libera el array con delete[]
Jugador** listarJugadoresPorEquipo(SistemaDeportivo* s, int idEquipo,
                                   int* cantidad);

// Retorna array con todos los jugadores del sistema
// El llamador libera el array con delete[]
Jugador** listarJugadores(SistemaDeportivo* s, int* cantidad);

// Actualiza datos editables de un jugador (no permite cambiar idEquipo)
// Retorna true si se actualizó, false si el ID no existe
bool actualizarJugador(SistemaDeportivo* s, int id, Jugador jugadorActualizado);

// Elimina un jugador del sistema
// Retorna true si se eliminó, false si no existe
bool eliminarJugador(SistemaDeportivo* s, int id);
```

#### 3.6 Funciones de Presentación — JUGADORES

```cpp
void menuRegistrarJugador(SistemaDeportivo* s);
void menuBuscarJugador(SistemaDeportivo* s);
void menuActualizarJugador(SistemaDeportivo* s);
void menuEliminarJugador(SistemaDeportivo* s);
void menuListarJugadores(SistemaDeportivo* s);

// Muestra jugador con el nombre del equipo (no solo el ID)
void mostrarJugador(Jugador* jugador, SistemaDeportivo* s);
void mostrarListaJugadores(Jugador** jugadores, int cantidad,
                           SistemaDeportivo* s);
```

---

#### 3.7 Funciones de Lógica — PARTIDOS

Los partidos tienen dos operaciones distintas que **deben implementarse como funciones separadas**:

1. **Programar** un partido: registra la fecha y los equipos, estado = `"PROGRAMADO"`, puntos en 0.
2. **Registrar resultado**: actualiza los puntos y el estado a `"JUGADO"`, y actualiza las estadísticas de ambos equipos automáticamente.

```cpp
// Programa un partido entre dos equipos
// Valida: ambos equipos existen, no son el mismo, no tienen partido ya programado entre sí
// Retorna puntero al partido creado, o nullptr si falla alguna validación
Partido* programarPartido(SistemaDeportivo* s, int idLocal, int idVisitante,
                          const char* fecha, const char* descripcion);

// Registra el resultado de un partido PROGRAMADO
// Actualiza estado a "JUGADO" y recalcula estadísticas de ambos equipos:
//   Victoria local  → local  +3 pts, +1 victoria  / visitante +1 derrota
//   Empate          → ambos  +1 pt,  +1 empate
//   Victoria visit. → visit. +3 pts, +1 victoria  / local     +1 derrota
//   Siempre actualizar puntosAFavor y puntosEnContra de ambos
// Retorna puntero al partido actualizado, o nullptr si no existe o ya fue jugado
Partido* registrarResultado(SistemaDeportivo* s, int idPartido,
                            int puntosLocal, int puntosVisitante);

// Retorna puntero al partido con ese ID, o nullptr si no existe
Partido* buscarPartidoPorID(SistemaDeportivo* s, int id);

// Retorna array de partidos en los que participó el equipo (como local o visitante)
// Escribe la cantidad en *cantidad
// El llamador libera el array con delete[]
Partido** buscarPartidosPorEquipo(SistemaDeportivo* s, int idEquipo,
                                  int* cantidad);

// Retorna array de partidos con ese estado ("PROGRAMADO", "JUGADO", "CANCELADO")
// El llamador libera el array con delete[]
Partido** listarPartidosPorEstado(SistemaDeportivo* s, const char* estado,
                                  int* cantidad);

// Retorna array con todos los partidos
// El llamador libera el array con delete[]
Partido** listarPartidos(SistemaDeportivo* s, int* cantidad);

// Cancela un partido: cambia estado a "CANCELADO"
// Si el partido ya fue JUGADO, revierte las estadísticas de ambos equipos
// Retorna true si se canceló, false si no existe o ya estaba cancelado
bool cancelarPartido(SistemaDeportivo* s, int idPartido);
```

#### 3.8 Funciones de Presentación — PARTIDOS

```cpp
void menuProgramarPartido(SistemaDeportivo* s);
void menuRegistrarResultado(SistemaDeportivo* s);
void menuBuscarPartido(SistemaDeportivo* s);
void menuListarPartidos(SistemaDeportivo* s);
void menuCancelarPartido(SistemaDeportivo* s);

// Muestra partido con los nombres de los equipos (no solo los IDs)
void mostrarPartido(Partido* partido, SistemaDeportivo* s);
void mostrarListaPartidos(Partido** partidos, int cantidad,
                          SistemaDeportivo* s);
```

---

### 4. MENÚS DEL SISTEMA

#### Menú Principal

```
╔═══════════════════════════════════════════╗
║    SISTEMA DE GESTIÓN DE TORNEOS          ║
║    Torneo: Liga Apertura 2026             ║
║    Deporte: Fútbol | Formato: GRUPOS      ║
╠═══════════════════════════════════════════╣
║  1. Gestión de Equipos                    ║
║  2. Gestión de Jugadores                  ║
║  3. Gestión de Partidos                   ║
║  4. Tabla de Posiciones                   ║
║  0. Salir                                 ║
╚═══════════════════════════════════════════╝

Seleccione una opción:
```

#### Submenú: Gestión de Equipos

```
╔═══════════════════════════════════════════╗
║          GESTIÓN DE EQUIPOS               ║
╠═══════════════════════════════════════════╣
║  1. Registrar equipo                      ║
║  2. Buscar equipo                         ║
║  3. Actualizar equipo                     ║
║  4. Listar equipos                        ║
║  5. Eliminar equipo                       ║
║  0. Volver al menú principal              ║
╚═══════════════════════════════════════════╝
```

#### Submenú: Gestión de Jugadores

```
╔═══════════════════════════════════════════╗
║        GESTIÓN DE JUGADORES               ║
╠═══════════════════════════════════════════╣
║  1. Registrar jugador                     ║
║  2. Buscar jugador                        ║
║  3. Actualizar jugador                    ║
║  4. Listar jugadores (todos)              ║
║  5. Listar jugadores por equipo           ║
║  6. Eliminar jugador                      ║
║  0. Volver al menú principal              ║
╚═══════════════════════════════════════════╝
```

#### Submenú: Gestión de Partidos

```
╔═══════════════════════════════════════════╗
║        GESTIÓN DE PARTIDOS                ║
╠═══════════════════════════════════════════╣
║  1. Programar partido                     ║
║  2. Registrar resultado                   ║
║  3. Buscar partido                        ║
║  4. Listar partidos                       ║
║  5. Cancelar partido                      ║
║  0. Volver al menú principal              ║
╚═══════════════════════════════════════════╝
```

---

### 5. CASOS DE USO OBLIGATORIOS

El programa debe ser capaz de ejecutar los siguientes escenarios de forma completa y sin errores.

#### Caso de Uso 1: Registro Completo de Equipo y Jugadores

```
1. Registrar equipo "Deportivo Maracaibo" de la ciudad "Maracaibo"
2. Registrar 3 jugadores asociados a ese equipo
3. Buscar el equipo por nombre parcial ("mara" debe encontrarlo)
4. Listar jugadores del equipo
5. Mostrar el nombre del equipo en el listado de jugadores (no solo su ID)
```

#### Caso de Uso 2: Programación y Resultado de Partido

```
1. Registrar dos equipos: "Deportivo Maracaibo" y "Caracas FC"
2. Programar un partido entre ellos con estado "PROGRAMADO"
3. Verificar que no se puede programar otro partido entre los mismos equipos
4. Registrar resultado: 2 - 1 a favor de "Deportivo Maracaibo"
5. Verificar que los puntos, victorias y puntos de ambos equipos se actualizaron
```

#### Caso de Uso 3: Tabla de Posiciones

```
1. Jugar varios partidos entre 4 equipos distintos
2. Mostrar la tabla de posiciones ordenada por:
   puntos → diferencia de puntos → puntos a favor
3. Verificar que el ordenamiento es correcto
```

#### Caso de Uso 4: Cancelación de Partido Jugado

```
1. Cancelar el partido con resultado 2-1 del Caso de Uso 2
2. Verificar que el estado cambió a "CANCELADO"
3. Verificar que los puntos y estadísticas de ambos equipos se revirtieron
```

#### Caso de Uso 5: Búsquedas

```
1. Buscar jugador por nombre parcial ("car" debe encontrar "Carlos Mendez")
2. Buscar todos los partidos de un equipo específico (como local o visitante)
3. Listar todos los partidos con estado "PROGRAMADO"
```

#### Caso de Uso 6: Validaciones de Negocio

```
1. Intentar registrar un equipo con nombre duplicado → debe rechazarlo
2. Intentar registrar un jugador con cédula ya registrada → debe rechazarlo
3. Intentar registrar un jugador con dorsal ya usado en el mismo equipo → debe rechazarlo
4. Intentar programar un partido de un equipo contra sí mismo → debe rechazarlo
5. Intentar registrar resultado de un partido ya "JUGADO" → debe rechazarlo
6. Intentar eliminar un equipo que tiene partidos → debe advertir y confirmar
```

#### Caso de Uso 7: Cancelación Durante Registro

```
1. Iniciar registro de un jugador
2. Ingresar algunos datos
3. Ingresar "CANCELAR" antes de confirmar
4. Verificar que el jugador no fue creado
```

#### Caso de Uso 8: Redimensionamiento de Arrays

```
1. Registrar 5 equipos (capacidad inicial = 4, debe redimensionarse en el 5to)
2. Verificar que todos los equipos siguen accesibles correctamente
3. Registrar 5 jugadores en un mismo equipo (debe redimensionarse igualmente)
```

---

### 6. VALIDACIONES Y MANEJO DE ERRORES

#### 6.1 Validaciones de Entrada

| Campo | Validación |
|-------|-----------|
| Nombre (equipo/jugador) | No vacío, no duplicado |
| Ciudad / Entrenador | No vacíos |
| Cédula | No duplicada en todo el sistema |
| Posición | Solo "PORTERO", "DEFENSA", "MEDIOCAMPISTA" o "DELANTERO" |
| Edad | Entre 14 y 50 años |
| Número Dorsal | Entre 1 y 99, no duplicado dentro del mismo equipo |
| idEquipo (en jugador) | Debe existir en el sistema |
| idEquipoLocal/Visitante | Deben existir y ser distintos |
| puntos | Deben ser >= 0 |
| Fecha | Formato YYYY-MM-DD, valores válidos |
| Estado del partido | Solo operar sobre partidos en estado correcto |

#### 6.2 Manejo de nullptr

Toda función que retorne un puntero puede retornar `nullptr` como señal de fallo. La capa de presentación **siempre** debe verificar el resultado antes de usarlo.

```cpp
// Patrón obligatorio en la capa de presentación
Equipo* resultado = agregarEquipo(s, nombre, ciudad, entrenador);
if (resultado == nullptr) {
    cout << "ERROR: No se pudo registrar el equipo." << endl;
    return;
}
mostrarEquipo(resultado);
```

#### 6.3 Manejo de Arrays de Punteros

Toda función que retorne `Entidad**` entrega la propiedad del array al llamador. La capa de presentación debe liberar ese array después de usarlo.

```cpp
// Patrón obligatorio al usar funciones que retornan Entidad**
int cantidad = 0;
Equipo** tabla = generarTablaPosiciones(s, &cantidad);

if (tabla == nullptr || cantidad == 0) {
    cout << "No hay equipos registrados." << endl;
} else {
    mostrarListaEquipos(tabla, cantidad);
    delete[] tabla;   // ← obligatorio: liberar el array externo
}
```

> **Nota:** `delete[] tabla` libera solo el array de punteros, **no** los equipos apuntados (esos viven en `s->equipos`). Nunca uses `delete` sobre los elementos individuales del array retornado.

#### 6.4 Mensajes de Error Obligatorios

```cpp
"ERROR: Ya existe un equipo con el nombre 'Deportivo Maracaibo'."
"ERROR: La cédula '12345678' ya está registrada."
"ERROR: El dorsal 10 ya está en uso en el equipo 'Caracas FC'."
"ERROR: No existe ningún equipo con ID 7."
"ERROR: No se puede programar un partido de un equipo contra sí mismo."
"ERROR: El partido ID 3 ya tiene resultado registrado."
"ADVERTENCIA: El equipo tiene 2 partidos asociados. ¿Desea eliminarlo igualmente? (S/N)"
```

#### 6.5 Confirmaciones Obligatorias (S/N)

Solicitar confirmación antes de:
- Guardar nuevo registro
- Actualizar registro existente
- Eliminar registro
- Cancelar partido (especialmente si ya fue JUGADO)
- Salir del programa

---

### 7. GESTIÓN DE MEMORIA

#### 7.1 Reglas Obligatorias

1. **Inicialización:** Todos los arrays dinámicos con capacidad 4 usando `new`
2. **Liberación:** `liberarSistema()` debe liberar los 3 arrays con `delete[]`
3. **Redimensionamiento:** Duplicar capacidad cuando `num == capacidad`
4. **Sin fugas:** Cada `new` debe tener su correspondiente `delete`
5. **Arrays retornados:** La capa de presentación libera con `delete[]` el array de punteros, nunca los elementos individuales

#### 7.2 Ciclo de vida de un array retornado

```
agregarEquipo()         → new Equipo  en s->equipos[i]    (vive en el sistema)
listarEquipos()         → new Equipo* [cantidad]           (array temporal)
    mostrarListaEquipos()
    delete[] resultado                                      (libera solo el array)
                        → s->equipos[i] sigue intacto
liberarSistema()        → delete[] s->equipos              (libera los objetos reales)
```

---

### 8. FORMATO DE SALIDA

#### 8.1 Tabla de Posiciones

```
╔══════════════════════════════════════════════════════════════════════╗
║                      TABLA DE POSICIONES                             ║
║                      Liga Apertura 2026                              ║
╠════╦══════════════════╦═════╦═══╦═══╦═══╦════╦════╦════╗
║ #  ║ Equipo           ║ PTS ║ J ║ G ║ E ║ D  ║ GF ║ GC ║
╠════╬══════════════════╬═════╬═══╬═══╬═══╬════╬════╬════╣
║  1 ║ Deportivo Mbo    ║   6 ║ 2 ║ 2 ║ 0 ║  0 ║  5 ║  2 ║
║  2 ║ Caracas FC       ║   3 ║ 2 ║ 1 ║ 0 ║  1 ║  3 ║  3 ║
║  3 ║ Lara SC          ║   1 ║ 2 ║ 0 ║ 1 ║  1 ║  2 ║  5 ║
╚════╩══════════════════╩═════╩═══╩═══╩═══╩════╩════╩════╝

Referencia: PTS=Puntos  J=Jugados  G=Ganados  E=Empatados
            D=Derrotas  GF=puntos a Favor  GC=puntos en Contra
```

#### 8.2 Detalle de Partido

```
╔══════════════════════════════════════════════════╗
║              DETALLE DE PARTIDO                  ║
╠══════════════════════════════════════════════════╣
║ ID Partido  : 3                                  ║
║ Estado      : JUGADO                             ║
║ Fecha       : 2026-03-15                         ║
║                                                  ║
║ Deportivo Maracaibo  2 - 1  Caracas FC           ║
║      (Local)                  (Visitante)        ║
║                                                  ║
║ Notas: Final reñido, expulsión en el minuto 80   ║
╚══════════════════════════════════════════════════╝
```

#### 8.3 Listado de Jugadores

```
╔═══════════════════════════════════════════════════════════════════════╗
║                      LISTADO DE JUGADORES                             ║
╠════╦══════════════════╦══════════════╦═══════════════╦═════╦═════════╣
║ ID ║ Nombre           ║ Equipo       ║ Posición      ║ Edad║ Dorsal  ║
╠════╬══════════════════╬══════════════╬═══════════════╬═════╬═════════╣
║  1 ║ Carlos Mendez    ║ Dep. Mbo     ║ DELANTERO     ║  25 ║    9    ║
║  2 ║ Pedro Rojas      ║ Caracas FC   ║ PORTERO       ║  28 ║    1    ║
╚════╩══════════════════╩══════════════╩═══════════════╩═════╩═════════╝

Total de jugadores: 2
```

---

### 9. DOCUMENTACIÓN OBLIGATORIA

#### 9.1 README del Proyecto

Incluir archivo `README.md` con:
- Descripción del proyecto y el torneo configurado
- Instrucciones de compilación y ejecución
- Diagrama de las 3 estructuras y sus relaciones
- Explicación de la separación de capas (lógica vs presentación)
- Funcionalidades implementadas
- Casos de prueba ejecutados

---

### 10. ENTREGA

- Código fuente (`.cpp`, `.h`)
- `README.md`
- Manual de usuario (PDF)
- Capturas de pantalla de ejecución de los 8 casos de uso

---

### 11. PREGUNTAS FRECUENTES

**P: ¿Puedo usar vectores de STL en lugar de arrays dinámicos?**  
R: No. El objetivo es practicar gestión manual de memoria con punteros.

**P: ¿Por qué las funciones de lógica no pueden tener cout?**  
R: Porque esa función podría ser reutilizada en distintos contextos (menú, reporte, archivo). Si mezcla presentación, pierde flexibilidad. En el Proyecto 3 lo apreciarás cuando esa misma lógica migre a métodos de clase.

**P: ¿Quién libera el array retornado por `listarEquipos()`?**  
R: La función que lo llamó (capa de presentación). Solo liberas el array de punteros (`delete[] equipos`), nunca los equipos en sí.

**P: ¿Qué devuelvo si `buscarEquiposPorNombre` no encuentra nada?**  
R: Puedes retornar `nullptr` y escribir 0 en `*cantidad`, o retornar un array vacío. Documenta tu decisión y mantenla consistente.

**P: ¿La cancelación de un partido jugado debe revertir las estadísticas?**  
R: Sí. Si el partido ya fue JUGADO, cancelarlo debe revertir victorias, empates, derrotas, puntos a favor y puntos en contra de ambos equipos exactamente como si ese partido nunca se hubiera jugado.

**P: ¿Qué pasa si el usuario ingresa texto donde se espera un número?**  
R: Debes validar y manejar este error, solicitando nuevamente el dato correcto.

---

## NOTAS FINALES

Este proyecto evalúa tu capacidad para:
1. Gestionar memoria dinámica de forma segura y eficiente
2. Implementar estructuras de datos complejas con punteros
3. **Diseñar funciones con tipos de retorno significativos (no todo es `void`)**
4. **Separar la lógica de negocio de la interfaz de usuario desde el inicio**
5. Desarrollar algoritmos de búsqueda, ordenamiento y validación
6. Crear interfaces de usuario claras y funcionales
7. Documentar código de manera profesional

**¡Éxito en tu proyecto!**

---

**Ing. Victor Kneider**  
Profesor de Programación 2  
Universidad Rafael Urdaneta
