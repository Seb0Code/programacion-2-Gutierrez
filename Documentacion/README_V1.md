# 🏆 Sport G&C Tournaments

### Sistema de Gestión de Torneos Deportivos

**Asignatura:** Programación 2 — Universidad Rafael Urdaneta  
**Profesor:** Ing. Victor Kneider  
**Trimestre:** 2026-C

---

## 📋 Descripción del Proyecto

**Sport G&C Tournaments** es un sistema de gestión de torneos deportivos desarrollado en C++ con estándar C++11. Permite administrar de forma completa un torneo deportivo: registro de equipos y jugadores, programación de partidos, registro de resultados y seguimiento de la tabla de posiciones.

El sistema admite **9 disciplinas deportivas** con validaciones específicas para cada una, e implementa gestión manual de memoria dinámica a través de arrays redimensionables con `new`/`delete`, sin uso de STL (`vector`, `list`, etc.).

---

## ✨ Características Principales

- **Gestión completa de Equipos:** registro, búsqueda (por ID o nombre parcial), listado, actualización y eliminación.
- **Gestión completa de Jugadores:** registro, búsqueda, listado general y por equipo, actualización y eliminación.
- **Gestión completa de Partidos:** programación, registro de resultados, búsqueda (por ID o equipo), listado (por estado), cancelación con reversión automática de estadísticas.
- **Tabla de Posiciones** ordenada por: puntos → diferencia de puntos → puntos a favor → victorias.
- **Validaciones robustas:** fechas en formato `YYYY-MM-DD`, edades (14–50), dorsales (1–99), cédulas (7–10 dígitos), posiciones según deporte y unicidad de datos.
- **Multi-deporte:** Fútbol, Baloncesto, Tenis, Voleibol, Rugby, Béisbol, Hockey, Handball, Softball.
- **Separación en dos capas** (Lógica / Presentación) como base para proyectos futuros.
- **Redimensionamiento automático** de arrays dinámicos (duplica capacidad al llenarse).
- **Compatibilidad multiplataforma:** Windows (UTF-8 + limpieza de pantalla nativa) y Linux/macOS.

---

## 🏗️ Estructura del Código

El proyecto está contenido en un único archivo `main.cpp`, organizado por `namespace` de la siguiente forma:

```
main.cpp
│
├── namespace Auxiliares          ← Utilidades generales
│   ├── configurarIdioma()        ← Configura encoding UTF-8
│   ├── waitfor(ms)               ← Pausa temporal
│   ├── limpiarPantalla()         ← Limpia la consola
│   ├── toMayus() / toMinus()     ← Conversión de mayúsculas/minúsculas
│   ├── ingresarDatos<T>()        ← Lectura genérica con validador
│   ├── ingresarCadena()          ← Lectura de cadenas con validador
│   └── pausarPrograma()          ← Espera a que el usuario presione Enter
│
├── namespace Validadores         ← Validaciones de negocio
│   ├── [arrays de deportes y posiciones]
│   ├── Positivo(), IDvalido(), Edad(), Dorsal()
│   ├── FechaValida(), ValidarFechaFin()
│   ├── fechaValidaRegistroDePartidos()
│   ├── fechaValidaRegistroDeJugadorOEquipo()
│   ├── Cedulas(), Nombres(), Posicion()
│   └── existeDeporte(), nombreTorneo()
│
├── namespace Logica              ← CAPA DE LÓGICA (sin cin/cout)
│   ├── inicializarSistemaDeportivo()
│   ├── liberarSistema()
│   ├── definirFormato()
│   ├── namespace redimensionar
│   │   ├── rEquipos()
│   │   ├── rJugadores()
│   │   └── rPartidos()
│   ├── namespace equipos
│   │   ├── agregarEquipo()       ← Retorna Equipo*
│   │   ├── buscarEquipoPorID()   ← Retorna Equipo*
│   │   ├── buscarEquipoPorSubCadena() ← Retorna Equipo**
│   │   ├── listarEquipos()       ← Retorna Equipo**
│   │   ├── TablaDePosiciones()   ← Retorna Equipo** ordenado
│   │   ├── actualizarEquipo()    ← Retorna bool
│   │   └── eliminarEquipo()      ← Retorna bool
│   ├── namespace jugadores
│   │   ├── agregarJugador()      ← Retorna Jugador*
│   │   ├── buscarJugadorPorID()  ← Retorna Jugador*
│   │   ├── buscarJugadoresPorNombre() ← Retorna Jugador**
│   │   ├── listarJugadoresPorEquipo() ← Retorna Jugador**
│   │   ├── listarJugadores()     ← Retorna Jugador**
│   │   ├── actualizarJugador()   ← Retorna bool
│   │   └── eliminarJugador()     ← Retorna bool
│   └── namespace partidos
│       ├── programarPartido()    ← Retorna Partido*
│       ├── registrarResultado()  ← Retorna Partido*
│       ├── buscarPartidoPorID()  ← Retorna Partido*
│       ├── listarPartidos()      ← Retorna Partido**
│       ├── listarPartidosPorEquipo() ← Retorna Partido**
│       ├── listarPartidosPorSuEstado() ← Retorna Partido**
│       └── cancelarPartido()     ← Retorna bool
│
└── namespace Presentacion        ← CAPA DE PRESENTACIÓN (con cin/cout)
    ├── namespace menu            ← Renderizado de menús
    ├── namespace equipos         ← Flujos de interacción para equipos
    ├── namespace Jugadores       ← Flujos de interacción para jugadores
    ├── namespace partidos        ← Flujos de interacción para partidos
    └── mensajeSalida/Default/MenuPrincipal/MenuAnterior()
```

---

## 🗂️ Estructuras de Datos y Relaciones

```
┌─────────────────────────────────────────────────┐
│                 SistemaDeportivo                 │
│  ┌──────────┐  ┌──────────┐  ┌───────────────┐  │
│  │  Torneo  │  │ Equipo[] │  │  Jugador[]    │  │
│  │ (único)  │  │ (dinám.) │  │  (dinámico)   │  │
│  └──────────┘  └────┬─────┘  └──────┬────────┘  │
│                     │               │            │
│                     │ id ←──────── idEquipo      │
│                     │                            │
│               ┌─────┴────────┐                  │
│               │  Partido[]   │                  │
│               │  (dinámico)  │                  │
│               │ idEquipoLocal│                  │
│               │ idEquipoVis. │                  │
│               └──────────────┘                  │
└─────────────────────────────────────────────────┘
```

**Relaciones:**

- `Jugador.IDequipo` → `Equipo.ID` (N jugadores por equipo)
- `Partido.IDEquipoLocal` y `Partido.IDEquipoVisitante` → `Equipo.ID` (2 equipos por partido)
- Un `Equipo` no puede eliminarse si tiene partidos asociados.

---

## 🔧 Arquitectura de Dos Capas

```
╔══════════════════════════════════════════════════════════╗
║                  CAPA DE PRESENTACIÓN                    ║
║  namespace Presentacion                                  ║
║  • Usa cin/cout                                          ║
║  • Solicita datos al usuario                             ║
║  • Llama a funciones de Lógica                           ║
║  • Verifica nullptr antes de usar el resultado           ║
║  • Libera arrays retornados (Entidad**) con delete[]     ║
╠══════════════════════════════════════════════════════════╣
║                   CAPA DE LÓGICA                         ║
║  namespace Logica                                        ║
║  • Sin cin/cout                                          ║
║  • Opera sobre SistemaDeportivo*                         ║
║  • Retorna Entidad* (éxito) o nullptr (fallo)            ║
║  • Retorna Entidad** con arreglos temporales             ║
║  • El llamador libera esos arreglos con delete[]         ║
╚══════════════════════════════════════════════════════════╝
```

**Ejemplo de flujo:**

```
Presentacion::equipos::RegistrarEquipos(s)
    │  valida datos de entrada (cin)
    └─→ Logica::equipos::agregarEquipo(s, nombre, ...)
            │  verifica duplicado, redimensiona si es necesario
            └─→ retorna Equipo* (éxito) o nullptr (fallo)
    │  muestra resultado al usuario (cout)
```

---

## ⚙️ Compilación y Ejecución

### Requisitos

- Compilador compatible con C++11 o superior (`g++`, `clang++`, MSVC)
- Sistema operativo: Windows, Linux o macOS

### Compilar en Linux/macOS

```bash
g++ -std=c++11 -Wall -Wextra -o torneo main.cpp
./torneo
```

### Compilar en Windows (MinGW)

```bash
g++ -std=c++11 -Wall -Wextra -o torneo.exe main.cpp
torneo.exe
```

### Compilar con optimizaciones

```bash
g++ -std=c++11 -O2 -o torneo main.cpp
```

> **Nota para Windows:** El sistema activa automáticamente `SetConsoleOutputCP(CP_UTF8)` para mostrar caracteres especiales del español correctamente en la terminal.

---

## 🧪 Casos de Uso Ejecutados

| #   | Caso de Uso                                                                                            | Estado |
| --- | ------------------------------------------------------------------------------------------------------ | ------ |
| 1   | Registro de equipo y jugadores; búsqueda y listado                                                     | ✅     |
| 2   | Programación de partido y registro de resultado                                                        | ✅     |
| 3   | Tabla de posiciones ordenada por múltiples criterios                                                   | ✅     |
| 4   | Cancelación de partido jugado con reversión de estadísticas                                            | ✅     |
| 5   | Búsquedas (jugador por nombre parcial, partidos por equipo/estado)                                     | ✅     |
| 6   | Validaciones de negocio (nombre duplicado, cédula duplicada, dorsal, autopartido, resultado duplicado) | ✅     |
| 7   | Cancelación de registro antes de confirmar (S/N)                                                       | ✅     |
| 8   | Redimensionamiento automático al agregar el 5.° equipo o jugador                                       | ✅     |

---

## 📦 Entregables

- `main.cpp` — Código fuente completo del sistema
- `README.md` — Este archivo
- `ManualDeUsuario.md` — Guía de uso paso a paso para el usuario final

---

## 👥 Autor

Desarrollado por Sebastián Gutiérrez como Proyecto 1 de Programación 2  
**Universidad Rafael Urdaneta — Facultad de Ingeniería**  
Trimestre 2026-C
