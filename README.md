
<div align="center">

# Computer Graphics

### C++ · DirectX 12 · HLSL

**A collection of practical computer graphics implementations and rendering experiments.**

</div>

---

## About the Project

This repository contains my Computer Graphics coursework and practical experiments focused on understanding and implementing real-time rendering techniques.

The project is written in **C++** using **DirectX 12**, with graphics functionality implemented through **HLSL shaders**.

The main goal is to explore how the graphics pipeline works, how rendering resources interact, and how different techniques can be combined to produce a final image.

---

## Technologies

| Technology | Purpose |
|---|---|
| C++ | Application and rendering logic |
| DirectX 12 | Graphics API |
| HLSL | Shader programming |
| Win32 | Application window and input |
| Visual Studio | Development environment |

---

## Implemented Systems and Experiments

### 01. Rendering Architecture

The project contains a DirectX 12 rendering framework with components responsible for graphics initialization, resource management and rendering.

Relevant project files:

- `D3D12Context.cpp`
- `RenderingSystem.cpp`
- `Application.cpp`
- `Shaders.hlsl`

Topics explored include:

- Graphics pipeline configuration
- Pipeline State Objects (PSO)
- Root Signatures
- Input Layouts
- Resource binding
- Render targets
- Depth and stencil operations
- Blend states

### 02. Deferred Rendering

Implementation of deferred rendering using a Geometry Buffer (G-Buffer).

The rendering process separates geometry data generation from the lighting pass.

Relevant files:

- `GBuffer.cpp`
- `DeferredGeometry.hlsl`
- `DeferredLighting.hlsl`
- `GBufferDebug.hlsl`

### 03. Shadow Mapping

Experiments with shadow rendering and depth-based shadow techniques.

Relevant file:

- `ShadowMap.hlsl`

### 04. Particle Effects

Shader-based effects used for rendering particles and environmental visuals.

Relevant files:

- `FireParticles.hlsl`
- `ParticleDust.hlsl`

### 05. Post-Processing

Experiments with image-space processing and composition of rendering results.

Relevant files:

- `PostProcessComposite.hlsl`
- `PostProcessTemplate.hlsl`

### 06. Terrain Rendering

Implementation of terrain geometry generation based on heightmap data.

The project includes a terrain loader capable of processing tiled heightmaps and combining them into a unified height field.

The terrain documentation also describes quadtree organization and level-of-detail visualization.

Relevant files:

- `TerrainHeightmap.cpp`
- `TerrainHeightmap.h`
- `TERRAIN_TILES.md`

### 07. Water Rendering

HLSL-based rendering experiments involving water surfaces.

Relevant file:

- `Water.hlsl`

---

## Additional Topics Studied

Alongside the practical implementation, I explored:

- Temporal Anti-Aliasing (TAA)
- Motion Vectors
- Temporal Reprojection
- History Sampling and Accumulation
- Ghosting and History Clamping
- Graphics Frame Analysis using RenderDoc

These topics are part of my Computer Graphics studies and are not necessarily all included in the current published repository.

---

## Project Structure

```text
Computer-Graphics/
│
└── Proto_CG/
    ├── CG_Sem2/
    │   ├── Application.cpp
    │   ├── D3D12Context.cpp
    │   ├── RenderingSystem.cpp
    │   ├── GBuffer.cpp
    │   ├── TerrainHeightmap.cpp
    │   │
    │   ├── Shaders.hlsl
    │   ├── DeferredGeometry.hlsl
    │   ├── DeferredLighting.hlsl
    │   ├── ShadowMap.hlsl
    │   ├── FireParticles.hlsl
    │   ├── ParticleDust.hlsl
    │   ├── Water.hlsl
    │   └── PostProcessComposite.hlsl
    │
    └── TERRAIN_TILES.md
```

---

# На русском

## О проекте

Репозиторий содержит учебный проект и практические работы по дисциплине **«Компьютерная графика»**.

Основной стек: **C++, DirectX 12 и HLSL**.

Цель работ — изучение графического конвейера, принципов построения систем рендеринга, работы с графическими ресурсами и реализации различных методов визуализации.

## Рассмотренные и реализованные направления

**Архитектура рендеринга**
- Настройка графического конвейера DirectX 12.
- Pipeline State Object (PSO).
- Root Signature и Input Layout.
- Render Targets, Depth/Stencil, Blend States.
- Привязка и использование графических ресурсов.

**Отложенный рендеринг**
- Работа с G-Buffer.
- Разделение геометрического прохода и расчёта освещения.
- Отладочная визуализация буферов.

**Шейдеры и визуальные эффекты**
- Shadow Mapping.
- Эффекты частиц: огонь и пыль.
- Постобработка изображения.
- Визуализация водной поверхности.

**Генерация и визуализация ландшафта**
- Загрузка карт высот.
- Объединение нескольких heightmap-тайлов.
- Построение геометрии ландшафта.
- Работа с quadtree и уровнями детализации.

## Дополнительно изученные темы

В рамках изучения компьютерной графики также разбирала Temporal Anti-Aliasing (TAA), векторы движения, репроекцию предыдущего кадра, накопление истории и способы уменьшения артефактов изображения.

Для исследования графических кадров и отдельных этапов рендеринга использовала RenderDoc.

---

<div align="center">

**Computer Graphics · Real-Time Rendering · Shader Programming**

</div>
