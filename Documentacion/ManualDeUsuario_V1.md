# 📖 Manual de Usuario
## Sport G&C Tournaments — Sistema de Gestión de Torneos Deportivos

---

## 1. INICIO DEL PROGRAMA

Al ejecutar el programa, lo primero que verás es el formulario de **configuración del torneo**. Debes completar estos datos antes de acceder al sistema:

### 1.1 Nombre del Torneo
- Ingresa el nombre completo del torneo (ejemplo: `Liga Apertura 2026`).
- Solo se admiten letras, números y espacios. No puede estar vacío.

### 1.2 Deporte
- El sistema mostrará la lista de deportes disponibles:
  ```
  FUTBOL, BALONCESTO, TENIS, VOLEIBOL, RUGBY, BEISBOL, HOCKEY, HANDBALL, SOFTBOL
  ```
- Escribe exactamente el nombre del deporte (no distingue mayúsculas/minúsculas).
- El deporte determina las posiciones válidas para los jugadores.

### 1.3 Formato del Torneo
- Selecciona una opción numérica:
  - `1` → Formato de Grupos (todos contra todos)
  - `2` → Formato de Eliminatoria Directa

### 1.4 Fechas del Torneo
- **Fecha de Inicio:** ingresa en formato `YYYY-MM-DD` (ejemplo: `2026-03-01`).
- **Fecha de Finalización:** debe ser igual o posterior a la fecha de inicio.
- El sistema valida automáticamente que la fecha sea real (controla meses, días y años bisiestos).

Después de confirmar los datos, aparecerá el **Menú Principal**.

---

## 2. MENÚ PRINCIPAL

```
╔════════════════════════════════════════════════════════════╗
║                  Sport G&C Tournaments                     ║
║  Torneo:  Liga Apertura 2026                               ║
║  Deporte: FUTBOL | Formato: GRUPOS                         ║
╠════════════════════════════════════════════════════════════╣
║  1. Gestión de Equipos                                     ║
║  2. Gestión de Jugadores                                   ║
║  3. Gestión de Partidos                                    ║
║  4. Tabla de Posiciones                                    ║
║  0. Salir                                                  ║
╚════════════════════════════════════════════════════════════╝
```

Escribe el número de la opción y presiona **Enter**.
Al elegir `0`, el sistema pedirá confirmación (`S/N`) antes de cerrar.

---

## 3. GESTIÓN DE EQUIPOS (Opción 1)

### Submenú de Equipos
```
1. Registrar equipo
2. Buscar equipo
3. Actualizar equipo
4. Listar equipos
5. Eliminar equipo
0. Volver al menú principal
```

---

### 3.1 Registrar Equipo
Datos que debes ingresar, **uno por pantalla**:

| Campo | Requisito |
|-------|-----------|
| Nombre del Equipo | Solo letras y espacios. No puede repetirse en el torneo. |
| Nombre del Entrenador | Solo letras y espacios. No puede estar ya registrado. |
| Fecha de Registro | Formato `YYYY-MM-DD`. Debe ser **antes** del inicio del torneo y máximo 6 meses atrás. |
| Ciudad de Origen | Solo letras y espacios. |

Al finalizar el formulario se te pedirá **confirmación (S/N)**:
- `S` → El equipo queda registrado y el sistema muestra su ID asignado.
- `N` → El registro se cancela y no se guardan datos.

> ⚠️ Si un nombre ya existe en el sistema, se te avisará y se te pedirá que ingreses uno diferente.

---

### 3.2 Buscar Equipo
Dos modalidades disponibles:

**Por ID:** ingresa el número de ID exacto del equipo.

**Por nombre parcial:** escribe parte del nombre (ejemplo: `"mara"` encontrará `"Deportivo Maracaibo"`). El sistema mostrará todas las coincidencias.

---

### 3.3 Actualizar Equipo
1. Ingresa el ID del equipo que deseas modificar.
2. El sistema verifica que ese ID exista.
3. Ingresa el nuevo nombre, nuevo entrenador y nueva ciudad.
4. Confirma los cambios con `S/N`.

> ℹ️ No se pueden modificar las estadísticas (puntos, victorias, etc.) de forma manual; se actualizan automáticamente al registrar partidos.

---

### 3.4 Listar Equipos
Muestra todos los equipos registrados en formato de tabla con: N°, ID, Nombre, Ciudad y Puntos acumulados.

---

### 3.5 Eliminar Equipo
1. Ingresa el ID del equipo.
2. El sistema muestra los datos del equipo a eliminar.
3. Confirma con `S/N`.

> ⚠️ **Restricción:** No puedes eliminar un equipo si tiene partidos asociados (programados, jugados o cancelados). Primero cancela o elimina los partidos.
> Si el equipo tiene jugadores pero no tiene partidos, sus jugadores también serán eliminados automáticamente del sistema.

---

## 4. GESTIÓN DE JUGADORES (Opción 2)

### Submenú de Jugadores
```
1. Registrar jugador
2. Buscar jugador
3. Actualizar jugador
4. Listar jugadores (todos)
5. Listar jugadores por equipo
6. Eliminar jugador
0. Volver al menú principal
```

---

### 4.1 Registrar Jugador
Datos que debes ingresar:

| Campo | Requisito |
|-------|-----------|
| ID del Equipo | Debe existir en el sistema. |
| Nombre del Jugador | Solo letras y espacios. No puede repetirse en el sistema. |
| Cédula | Solo dígitos. Entre 7 y 10 caracteres. No puede estar ya registrada. |
| Fecha de Registro | Formato `YYYY-MM-DD`. Antes del inicio del torneo (máximo 6 meses atrás). |
| Dorsal | Número entre `1` y `99`. No puede repetirse dentro del mismo equipo. |
| Posición | Selecciona un número del menú de posiciones según el deporte del torneo. |
| Edad | Número entre `14` y `50`. |

Al confirmar con `S`, el jugador queda registrado y se muestra su ID asignado.

---

### 4.2 Buscar Jugador
Dos modalidades:

**Por ID:** ingresa el número de ID exacto.

**Por nombre parcial:** escribe parte del nombre y el sistema mostrará todos los jugadores que coincidan.

---

### 4.3 Actualizar Jugador
1. Ingresa el ID del jugador a modificar.
2. Ingresa el nuevo nombre, nueva edad, nueva posición y nuevo dorsal.
3. Confirma con `S/N`.

> ℹ️ El sistema valida que el nuevo dorsal no esté ocupado por otro jugador en el mismo equipo (pero sí permite mantener el mismo dorsal del jugador a actualizar).

---

### 4.4 Listar Jugadores (todos)
Muestra una tabla con todos los jugadores del sistema, incluyendo el **nombre del equipo** al que pertenecen.

---

### 4.5 Listar Jugadores por Equipo
Ingresa el ID del equipo y el sistema mostrará una tabla con todos sus jugadores, posición, edad y dorsal.

---

### 4.6 Eliminar Jugador
1. Ingresa el ID del jugador.
2. El sistema muestra sus datos.
3. Confirma con `S/N`.

> ⚠️ Solo se puede eliminar un jugador si aún no ha anotado puntos en el torneo.

---

## 5. GESTIÓN DE PARTIDOS (Opción 3)

### Submenú de Partidos
```
1. Programar partido
2. Registrar resultado
3. Buscar partido
4. Listar partidos
5. Cancelar partido
0. Volver al menú principal
```

---

### 5.1 Programar Partido

1. Ingresa el **ID del equipo local** (debe existir en el sistema).
2. Ingresa el **ID del equipo visitante** (debe ser distinto al local).
3. El sistema verifica que ambos equipos tengan el **mínimo de jugadores** requerido para el deporte.
4. Ingresa la **fecha del partido** (dentro del rango de fechas del torneo).
5. Ingresa una **descripción** o nota adicional.
6. Confirma con `S/N`.

> ⚠️ **Validaciones:**
> - No se puede programar un partido de un equipo contra sí mismo.
> - No se puede programar un segundo partido `PROGRAMADO` entre los mismos dos equipos (deben jugar el anterior primero o cancelarlo).

---

### 5.2 Registrar Resultado

1. Ingresa el **ID del partido** (debe estar en estado `PROGRAMADO`).
2. Ingresa los **puntos del equipo local**.
3. Ingresa los **puntos del equipo visitante**.
4. Confirma con `S/N`.

Al confirmar, el sistema:
- Cambia el estado del partido a `JUGADO`.
- Actualiza automáticamente victorias, empates, derrotas, puntos y puntos a favor/en contra de ambos equipos.

> ⚠️ En deportes como Baloncesto, Tenis, Voleibol, Béisbol y Softball **no se permiten empates**. Debes registrar el marcador final con prórroga.

---

### 5.3 Buscar Partido

**Por ID:** ingresa el número de ID del partido. Si existe, se muestra una ficha detallada con:
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
║ Notas: ...                                       ║
╚══════════════════════════════════════════════════╝
```

**Por equipo:** ingresa el ID del equipo para ver todos sus partidos (como local o visitante).

---

### 5.4 Listar Partidos

Dos opciones disponibles:

**Todos los partidos:** muestra todos los partidos registrados con ID, fecha, estado y marcador.

**Por estado:** elige entre `PROGRAMADO` (0), `JUGADO` (1) o `CANCELADO` (2) para filtrar la lista.

---

### 5.5 Cancelar Partido

1. Ingresa el **ID del partido** a cancelar.
2. El sistema muestra los datos del partido.
3. Confirma con `S/N`.

Al confirmar:
- Si el partido estaba `PROGRAMADO`: cambia a `CANCELADO` sin más efectos.
- Si el partido estaba `JUGADO`: cambia a `CANCELADO` **y revierte automáticamente** todas las estadísticas de ambos equipos (puntos, victorias, empates, derrotas, puntos a favor/en contra y partidos jugados).

> ⚠️ No se puede cancelar un partido que ya está `CANCELADO`.

---

## 6. TABLA DE POSICIONES (Opción 4)

Muestra la clasificación actual del torneo, ordenada por los siguientes criterios (en orden de prioridad):

1. **Puntos** (descendente)
2. **Diferencia de puntos** (puntos a favor − puntos en contra, descendente)
3. **Puntos a favor** (descendente)
4. **Victorias** (descendente)

```
╔═══════════════════════════════════════════════════════════════╗
║                    TABLA DE POSICIONES                        ║
║                    LIGA APERTURA 2026                         ║
╠════╦═══════════════════════════════════════════╦════╦═══╦═══╗
║ #  ║ Equipo                                    ║ PTS║ J ║ G ║
╠════╬═══════════════════════════════════════════╬════╬═══╬═══╣
║  1 ║ Deportivo Maracaibo                       ║  6 ║ 2 ║ 2 ║
║  2 ║ Caracas FC                                ║  3 ║ 2 ║ 1 ║
╚════╩═══════════════════════════════════════════╩════╩═══╩═══╝
Referencia: PTS=Puntos  J=Jugados  G=Ganados  E=Empatados
            D=Derrotas  GF=Puntos a Favor  GC=Puntos en Contra
```

---

## 7. SISTEMA DE PUNTOS

| Resultado | Puntos equipo ganador | Puntos equipo perdedor |
|-----------|-----------------------|------------------------|
| Victoria | +3 puntos | 0 puntos |
| Empate (si aplica) | +1 punto | +1 punto |
| Derrota | 0 puntos | 0 puntos |

---

## 8. NAVEGACIÓN GENERAL

- En cualquier campo numérico, si escribes texto el sistema te pedirá que lo vuelvas a ingresar.
- Todas las confirmaciones usan `S` (sí) o `N` (no) — cualquier otra letra cancela la acción.
- Puedes volver al menú anterior seleccionando la opción `0` en cualquier submenú.
- Para salir del programa, selecciona `0` en el menú principal y confirma con `S`.

---

## 9. MENSAJES DE ERROR FRECUENTES

| Mensaje | Causa |
|---------|-------|
| `Error, el nombre '...' ya está en uso` | Ya existe un equipo con ese nombre. |
| `La cédula '...' ya está registrada` | Otro jugador tiene esa cédula. |
| `El dorsal '...' ya está en uso en el equipo '...'` | Ese número de dorsal ya lo usa otro jugador del mismo equipo. |
| `El ID '...' no pertenece a ningún equipo registrado` | No existe un equipo con ese ID. |
| `No se puede programar un partido entre un mismo equipo` | Local y visitante son el mismo equipo. |
| `El partido ya fue JUGADO` | Intentas registrar el resultado de un partido ya jugado. |
| `Los equipos no cumplen con el mínimo de jugadores` | Uno o ambos equipos tienen menos jugadores de los requeridos para el deporte. |
| `Se produjo un error a la hora de eliminar el equipo` | El equipo tiene partidos asociados; no se puede eliminar directamente. |

---

## 10. REQUISITOS DEL SISTEMA

- **Sistema Operativo:** Windows 10+, Ubuntu 20.04+ o macOS 11+
- **Compilador:** g++ 7+ con soporte C++11 (o superior)
- **Terminal:** cualquier terminal con soporte UTF-8 (en Windows se recomienda Windows Terminal o PowerShell 7)
