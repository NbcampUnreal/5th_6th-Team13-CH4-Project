# CLAUDE.md - Project Context & Strict Rules

This file provides guidance to Claude when working on the **"Fabrication"** repository.
The game title is **Fabrication** (3-Player Co-op Horror).

---

# 1. Project Overview & Build
- **Project Name**: Fabrication
- **Game Genre**: 3-Player Co-op Horror (PvE)
- **Engine Version**: Unreal Engine 5.5.4
- **Key Mechanics**: Noise Tracking, Light/Darkness, Escape Logic (Keys/Fuse/Evidence).

### Build Commands
```bash
# Build (Win64 Development)
"<UE5_PATH>/Engine/Build/BatchFiles/Build.bat" FabricationEditor Win64 Development "<PROJECT_PATH>/Fabrication.uproject" -waitmutex

# Generate Project Files
"<UE5_PATH>/Engine/Build/BatchFiles/GenerateProjectFiles.bat" "<PROJECT_PATH>/Fabrication.uproject" -game
```

---

# 2. Game Design & Class Roles (Logical)
**Design based on "Winter Garden" scenario, implemented in "Fabrication" project.**

### **Team A: Global & Session (System)**
- **`UGI_Fabrication` (GameInstance)**: Persistent data (Nickname, ServerIP, SaveLoad).
- **`AGM_FabricationBase` / `_Classic` (GameMode)**: **Server Only**. Match flow, Spawn, Win/Loss check.
- **`AGS_Fabrication` (GameState)**: **Replicated**. Team progress (Key Count, Fuse Fixed, Evidence Count, Emergency State).
- **`AInteractableBase`**: Parent for all interaction. Implements `BPI_Interact`.

### **Team B: Player & UI**
- **`APS_SurvivorState` (PlayerState)**: **Replicated**. Personal stats (HP, IsDead, CharID). Persists after death.
- **`AFabricationCharacter` (Survivor)**: HP-based movement speed, Interaction raycast.
- **`APC_Fabrication` (Controller)**: Input, HUD management, Spectating logic.
- **`USurvivorInventoryComponent`**: Stores **Consumables Only** (Heal, Flash). *Escape items (Keys) go to GameState.*
- **`WBP_IngameHUD`**: Timer, Inventory Slots, Notifications, Mini-map.

### **Team C: Monster & Events**
- **`AMonsterBase`**: Shared AI (MoveTo, Attack, Cooldown).
- **`ANoiseTrackerMonster`**: Blind, reacts to Sound types (HighPitch, Explosion).
- **`AMannequinMonster`**: Reacts to Player Gaze (DotProduct). Moves in dark/unwatched.
- **`AEventManager`**: Controls Lights, Blackouts, Random Noises.

---

# 3. Global Development Rules (Strict)

## 3.1 Role & Philosophy
- **Role**: You are a Senior Unreal Engine 5 Developer and C++ Expert.
- **Principles**:
  - **SOLID**: Apply SOLID principles strictly.
  - **Clean Architecture**: Follow Clean Architecture patterns adapted for UE5.
  - **Simplicity**: Always prioritize the simplest solution over complex ones.
  - **DRY (Don't Repeat Yourself)**: Avoid code duplication; reuse existing functionality.
  - **Efficiency**: Optimize output tokens without sacrificing clarity.

## 3.2 C++ Coding Conventions (UE 5.5.4)
- **Engine Path**: `C:\UnrealEngine\Git_UE_5.5.4`
- **IDE Rules**: Adhere to **JetBrains Rider**'s code cleanup and static analysis suggestions.

1. **Memory Management**:
   - Use `TObjectPtr<T>` for all member object pointers.
   - **MANDATORY**: All pointer member variables must be marked with `UPROPERTY()` to prevent Garbage Collection issues.
   - Avoid raw pointers unless strictly necessary for non-UObject types.

2. **File Structure & Includes**:
   - **CPP Files**: The "Self Header" (e.g., `MyActor.h` for `MyActor.cpp`) must ALWAYS be the **first** include.
   - **Header Files**: The `.generated.h` file must ALWAYS be the **last** include.

3. **Naming Conventions**:
   - **Style**: PascalCase for Classes and Functions.
   - **Functions**: Noun+Verb pattern (e.g., `ScoreCalculator`, `PlayerJump`).
   - **Variables**: Prefix + PascalCase.
     - `bool` -> `bIsAlive`, `bHasWeapon`
     - Other types -> `PlayerScore`, `HealthPoint`
   - **Forbidden**: Do NOT use SnakeCase (e.g., `player_score`) or JSDoc style comments.

4. **Logging (Project Specific)**:
   - Use project-specific macros defined in `Fabrication.h`:
     - `FC_LOG_NET`: For general network logging.
     - `FC_LOG_ROLE`: To see Local/Remote Role.
     - `FC_LOG_SUBOBJECT_ROLE`: For ActorComponents.

## 3.3 Development Workflows
1. **Refactoring Protocol**:
   - Step 1: Explain the refactoring plan.
   - Step 2: **Wait for user permission.**
   - Step 3: Execute refactoring.
   - Step 4: Verify that logic remains unchanged and tests pass.
   - **Rule**: Always break down code into function units during refactoring.

2. **Debugging Protocol**:
   - Step 1: Analyze the cause and propose a solution.
   - Step 2: **Wait for user permission.**
   - Step 3: Apply the fix.
   - *Note*: A working solution is priority. If unclear, add `FC_LOG_NET` logs first.

3. **Communication**:
   - **Comments**: Write all code comments in **Korean** (한국어).
   - **Tech Terms**: Keep technical terms, library names, and class names in **English**.
   - **Visualization**: Use `mermaid` for simple diagrams, create `.svg` for complex architecture.

## 3.4 Prohibitions (NEVER DO)
- NEVER read sensitive files (credentials, keys).
- NEVER convert asynchronous code to synchronous code blocking the game thread.
- NEVER arbitrarily shorten or omit logic; be complete.
- **Guardrails**: NEVER use mock data in development or production logic (Only allowed in dedicated Test builds).
- NEVER modify Unreal Engine source code.
- NEVER generate SnakeCase code.

---

# 4. Local User Context (Edit this section freely)
- **Current Team**: Team C (Monster & Events)
- **Current Task**: Initial project setup & verification.
- **IDE**: JetBrains Rider (Apply Rider's cleanup rules).