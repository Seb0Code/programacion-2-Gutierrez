# REPÚBLICA BOLIVARIANA DE VENEZUELA
## UNIVERSIDAD RAFAEL URDANETA
## FACULTAD DE INGENIERÍA
## ESCUELA DE INGENIERÍA DE COMPUTACIÓN

**ASIGNATURA:** PROGRAMACIÓN 2  
**PROFESOR:** ING. VICTOR KNEIDER  
**TRIMESTRE:** 2026-C

---

# PROYECTO 3: POO Y MODULARIZACIÓN DEL SISTEMA DE TORNEOS

## OBJETIVOS DE APRENDIZAJE

- Aplicar principios de Programación Orientada a Objetos (POO) en C++
- Convertir estructuras (`struct`) a clases (`class`) con encapsulamiento
- Separar declaraciones (`.hpp`) de implementaciones (`.cpp`)
- Organizar el proyecto en múltiples archivos por dominio
- Dominar constructores, destructores y métodos de acceso
- Aplicar templates para eliminar código duplicado en persistencia
- Comprender el flujo de compilación multi-archivo con Makefile

---

## FILOSOFÍA DE ESTA ETAPA

**No reinicies desde cero. Evoluciona tu Proyecto 2 aplicando los conceptos que se describen a continuación.**

### ¿Qué cambia y qué se mantiene?

**✅ SE MANTIENE:**
- Toda la funcionalidad de archivos binarios del Proyecto 2
- Acceso aleatorio con `seekg()` y `seekp()`
- Headers en archivos binarios (`ArchivoHeader`)
- Persistencia de datos, CRUD completo, backup e integridad referencial
- La separación de capas (lógica / presentación) establecida desde el Proyecto 1

**🔄 EVOLUCIONA:**
- `struct` → `class` con encapsulamiento completo
- Código concentrado en pocos archivos → módulos organizados por dominio
- Funciones sueltas → métodos de clase con responsabilidad única
- Acceso directo a campos → getters y setters con validación
- Funciones de archivo repetidas por tipo → template genérico en `GestorArchivos`
- Menús en `main.cpp` → clase `Interfaz` dedicada

---

## 1. ESTRUCTURA DEL PROYECTO

El proyecto debe organizarse en carpetas por dominio. Cada entidad vive en su propia carpeta junto con sus operaciones de negocio. La persistencia, la interfaz y las utilidades son módulos independientes:

```
ProyectoTorneos_v3/
│
├── torneo/
├── equipos/
├── jugadores/
├── partidos/
├── interfaz/
├── persistencia/
├── utilidades/
│
├── main.cpp
├── Makefile
│
└── datos/
    ├── torneo.bin
    ├── equipos.bin
    ├── jugadores.bin
    └── partidos.bin
```

Dentro de cada carpeta de dominio (ej. `equipos/`) se espera al menos:
- Un archivo `.hpp` y `.cpp` para la clase de entidad
- Un archivo `.hpp` y `.cpp` para las operaciones de negocio de ese dominio

---

## 2. PRINCIPIOS DE POO A APLICAR

### 2.1 Encapsulamiento

Convierte cada `struct` del Proyecto 2 en una `class`. Todos los atributos deben ser **privados**. El acceso a los datos solo puede hacerse a través de métodos públicos.

Un setter no es simplemente asignar el valor: debe verificar que el dato sea válido antes de aceptarlo. Por ejemplo, no tiene sentido asignar una edad negativa a un jugador, ni un número de dorsal fuera del rango permitido.

**Antes (Proyecto 2):**
```cpp
Equipo e;
e.victorias = -1;        // ❌ Sin ninguna validación posible
```

**Ahora (Proyecto 3):**
```cpp
Equipo e;
// ✅ La clase decide si el valor es válido antes de aceptarlo
```

### 2.2 Abstracción

Los métodos deben ocultar la complejidad interna. Quien llama a un método no necesita saber cómo está implementado por dentro, solo qué hace y qué retorna.

Por ejemplo, un método que actualiza las estadísticas de un equipo tras un partido no debería requerir que el llamador calcule manualmente los puntos: ese cálculo es responsabilidad de la clase.

### 2.3 Responsabilidad Única

Cada clase tiene **una razón para existir**. Asignar responsabilidades correctamente es uno de los objetivos centrales de este proyecto.

| Módulo | Responsabilidad |
|--------|----------------|
| Clases de entidad | Representar y encapsular los datos de cada entidad |
| Archivos de operaciones | Lógica de negocio y flujos de cada dominio |
| `GestorArchivos` | Toda operación de lectura y escritura en archivos binarios |
| `Interfaz` | Menús, navegación y loop principal del programa |
| `Validaciones` | Validaciones de formato reutilizables |
| `Formatos` | Formateo de salida consistente en todo el sistema |
| `main.cpp` | Solo inicializar el sistema y delegar a `Interfaz` |

**⚠️ ANTIPATRÓN — NUNCA hacer esto:**
```cpp
class Equipo {
    void guardarEnArchivo();    // ❌ Responsabilidad de GestorArchivos
    void mostrarMenu();         // ❌ Responsabilidad de Interfaz
    void pedirDatosAlUsuario(); // ❌ Responsabilidad de operacionesEquipos
};
```

### 2.4 Composición de Clases

`Partido` contiene internamente una colección de instancias de `Gol`. `Gol` debe ser también una clase con sus propios atributos privados, constructores y métodos de validación.

Esto significa que cuando un `Partido` se copia (por constructor de copia o asignación), sus goles deben copiarse correctamente junto con él. Esa es una responsabilidad de diseño que debes resolver.

---

## 3. REQUERIMIENTOS POR CLASE

Para cada clase de entidad del sistema (`Torneo`, `Equipo`, `Jugador`, `Gol`, `Partido`) se requiere:

**Constructores:**
- Constructor por defecto (sin parámetros)
- Constructor parametrizado con los datos principales de la entidad
- Constructor de copia

**Destructor.**

**Getters y Setters:**
- Un getter por cada atributo. Los getters no modifican el objeto (`const`).
- Un setter por cada atributo modificable, con la validación correspondiente.

**Métodos propios de la entidad:**
- Al menos un método que verifique si los datos del objeto son completos y válidos.
- Métodos de presentación: uno para información básica (una línea) y otro para información completa (ficha detallada).
- Un método estático que retorne el tamaño en bytes de la clase (`sizeof`).

**La clase NO debe conocer archivos ni menús.**

---

## 4. MÓDULO DE PERSISTENCIA CON TEMPLATES

### El problema

En el Proyecto 2, las funciones de archivo eran prácticamente idénticas para cada entidad: abrir el archivo, calcular el offset, leer o escribir exactamente `sizeof(T)` bytes, cerrar el archivo. La única diferencia entre guardar un equipo y guardar un jugador era el tipo y la ruta.

Escribir esa lógica tres veces separadas no es una buena práctica.

### La solución

Centraliza todas las operaciones de archivo en una clase `GestorArchivos` que use **templates (plantillas)** para operar de forma genérica sobre cualquier tipo de entidad.

Las operaciones básicas que el template debe cubrir son: guardar un nuevo registro, leer un registro por su posición física en el archivo, y sobrescribir un registro en una posición existente. La búsqueda por ID, el borrado lógico y las operaciones compuestas (registrar resultado, cancelar partido) también viven en esta clase, aunque no necesariamente como templates.

Una restricción importante: **los métodos template deben implementarse completamente en el archivo `.hpp`**, no en un `.cpp`. Investiga por qué el compilador lo exige así.

Con este diseño, guardar cualquier entidad se convierte en una sola llamada genérica, independientemente del tipo:

```cpp
GestorArchivos::guardarRegistro(RUTA_EQUIPOS,   equipo);
GestorArchivos::guardarRegistro(RUTA_JUGADORES, jugador);
GestorArchivos::guardarRegistro(RUTA_PARTIDOS,  partido);
```

Para que esto funcione, todas las clases de entidad deben respetar una **interfaz mínima** que el template puede asumir. Identifica qué métodos necesita el template poder llamar sobre cualquier `T`, y asegúrate de que todas tus clases los tengan.

---

## 5. MÓDULO DE INTERFAZ Y main.cpp

### Clase Interfaz

Todos los menús del sistema deben estar encapsulados en una clase `Interfaz`. Esta clase contiene el loop principal del programa y delega cada acción al módulo de operaciones correspondiente.

Ni `main.cpp` ni las clases de entidad deben conocer la lógica de navegación del sistema.

### main.cpp

Con la clase `Interfaz`, `main.cpp` debe quedar reducido a su expresión mínima: inicializar el sistema de archivos y delegar todo el flujo a `Interfaz`. No debe contener menús, switches, cin/cout de datos ni lógica de negocio.

---

## 6. MÓDULO DE UTILIDADES

### Clase Validaciones

Clase con métodos **estáticos** para validaciones de formato reutilizables en cualquier módulo del sistema: validar que una fecha tenga formato correcto, que una cédula solo contenga dígitos, que un valor esté dentro de un rango, etc.

**Diferencia importante respecto a los setters:**
- `Validaciones` verifica **formato**: ¿tiene la forma correcta?
- Los setters de la entidad verifican **reglas de negocio**: ¿tiene sentido para este dominio?

Esta clase no debe poder instanciarse.

### Clase Formatos

Clase con métodos **estáticos** para formatear la salida de forma consistente en todo el sistema: convertir `time_t` a cadena legible, limpiar el buffer de entrada, pausar esperando Enter, buscar subcadenas ignorando mayúsculas/minúsculas, etc.

Esta clase tampoco debe poder instanciarse.

---

## 7. GUARDAS DE INCLUSIÓN Y COMPILACIÓN

### Guardas de inclusión

Todo archivo `.hpp` **debe** tener guardas de inclusión para evitar inclusiones múltiples:

```cpp
#ifndef NOMBRE_ARCHIVO_HPP
#define NOMBRE_ARCHIVO_HPP
// ... contenido ...
#endif
```

### Inclusiones en .cpp

Incluye únicamente lo que el archivo realmente necesita. El primer include de cada `.cpp` debe ser su propio `.hpp`.

### Makefile

El proyecto debe compilar con un solo comando: `make`. Debes entregar un `Makefile` funcional que compile todos los módulos y genere el ejecutable. Documenta también cómo compilar y ejecutar en el `README`.

---

## 8. DIFERENCIAS CLAVE CON PROYECTO 2

| Aspecto | Proyecto 2 | Proyecto 3 |
|---------|-----------|-----------|
| Estructura de datos | `struct` público | `class` encapsulada |
| Acceso a datos | `e.puntos = 3` directo | Solo a través de métodos |
| Organización | Pocos archivos grandes | Módulos por dominio |
| Lógica de negocio | Funciones sueltas | Archivos de operaciones por dominio |
| Persistencia | Funciones repetidas por tipo | `GestorArchivos` con templates |
| Menús | En `main.cpp` | Clase `Interfaz` |
| `main.cpp` | Cientos de líneas | Menos de 20 líneas |
| Validaciones | Manual antes de cada uso | Dentro de setters y clase `Validaciones` |
| Compilación | Un solo archivo | Multi-archivo con `Makefile` |
| `Gol` | Struct embebido en Partido | Clase propia con encapsulamiento |

---

## 9. ESTRATEGIA DE IMPLEMENTACIÓN SUGERIDA

**Fase 1:** Convierte la clase más simple (`Equipo`) de `struct` a `class`. Verifica que `sizeof(Equipo)` sigue siendo idéntico al del Proyecto 2. Si cambia, los archivos `.bin` serán incompatibles.

**Fase 2:** Implementa `GestorArchivos` con los métodos template básicos. Prueba guardando y leyendo un `Equipo`. Si funciona con uno, funcionará con todos.

**Fase 3:** Crea `operacionesEquipos` moviendo la lógica que antes estaba en `main.cpp`. Verifica que registrar, buscar y listar sigue funcionando.

**Fase 4:** Convierte `Gol` a clase primero, luego `Partido`. Verifica que `sizeof(Partido)` no cambió.

**Fase 5:** Replica el patrón para `Jugador` y sus operaciones.

**Fase 6:** Crea la clase `Interfaz` moviendo todos los menús. Reduce `main.cpp` a su mínima expresión.

**Fase 7:** Extrae las validaciones y formatos dispersos en el código a las clases `Validaciones` y `Formatos`.

---

## 10. ENTREGABLES

### 10.1 Código Fuente

Estructura de carpetas completa según la sección 1. La organización propuesta es una guía; puedes adaptarla siempre que mantengas la separación por dominio y el principio de responsabilidad única.

### 10.2 Archivos de Prueba

Incluir en `datos/` los archivos `.bin` con al menos:

| Archivo | Mínimo |
|---------|--------|
| `equipos.bin` | 5 equipos activos |
| `jugadores.bin` | 15 jugadores distribuidos |
| `partidos.bin` | 10 partidos (mezcla de JUGADO y PROGRAMADO) |

### 10.3 Documentación

- `Makefile` funcional
- `README_V3.md` con estructura del proyecto, instrucciones de compilación y manual de usuario básico

---

## 11. PREGUNTAS FRECUENTES

**P: ¿Por qué los templates van en el `.hpp` y no en el `.cpp`?**  
R: Es parte del objetivo de aprendizaje. Investígalo y documenta la razón en tu README.

**P: ¿Cambia el `sizeof` de una clase respecto al `struct` equivalente del P2?**  
R: No debería, si los atributos son exactamente los mismos. Puedes verificarlo con el método estático `obtenerTamano()`. Si cambia, tus archivos `.bin` del P2 serán incompatibles con el P3.

**P: ¿Los setters siempre deben retornar `bool`?**  
R: Depende. Un setter que no puede fallar (ej. un ID asignado internamente) puede ser `void`. Un setter con validación debe indicar si la asignación fue aceptada o rechazada.

**P: ¿Qué métodos necesita el template de `GestorArchivos` que todas las clases tengan?**  
R: Eso es parte del diseño que debes resolver. Piensa qué operaciones necesita hacer el template sobre un objeto de tipo `T` genérico: asignar un ID, preguntar si está eliminado... ¿Qué más?

**P: ¿`main.cpp` puede tener más de una línea de lógica?**  
R: Solo lo mínimo para inicializar el sistema de archivos antes de delegar. Si estás poniendo menús o switches en `main.cpp`, algo está mal en la distribución de responsabilidades.

---

## NOTAS FINALES

Este proyecto evalúa tu capacidad para:
1. Identificar responsabilidades y asignarlas a la clase correcta
2. Diseñar clases con encapsulamiento real, no solo agregar `private` y copiar los campos
3. Aprovechar templates para eliminar duplicación de código en persistencia
4. Organizar un proyecto multi-archivo que compile limpiamente con Makefile
5. Preservar la compatibilidad con los archivos `.bin` del Proyecto 2

La separación de capas que estableciste en el Proyecto 1 y sostuviste en el Proyecto 2 ahora se formaliza en la arquitectura de clases. Todo encaja.

**¡Éxito en tu proyecto!**

---

**Ing. Victor Kneider**  
Profesor de Programación 2  
Universidad Rafael Urdaneta
