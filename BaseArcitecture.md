#### **Echelon 0: Foundation (`Plugins/SegFaultCore`)**

_Strictly No Game Logic. Only Contracts, Tags, and Network Primitives._

- **`Public/Tags/SegFaultNativeTags.h`**
    
    - _Role:_ Declares C++ native tags using `UE_DECLARE_GAMEPLAY_TAG_EXTERN`. Eliminates hardcoded strings (e.g., `InputTag_Weapon_Fire`).
        
- **`Public/Net/SegFaultNetSerializer.h`**
    
    - _Role:_ Custom Iris serializers. Contains `UE_NET_IMPLEMENT_NAMED_STRUCT` for highly optimized bit-packing of complex math structs (e.g., custom fixed-point vectors for trajectory validation).
        
- **`Public/Messages/SegFaultGameplayMessages.h`**
    
    - _Role:_ `USTRUCT` definitions for `UGameplayMessageSubsystem`. E.g., `FSegFaultWeaponFiredMessage`, `FSegFaultPlayerDiedMessage`. Ensures L1 modules communicate via broadcast, not direct pointers.
        
- **`Public/Interfaces/ISegFaultInventoryInterface.h`** & **`ISegFaultAbilitySystemInterface.h`**
    
    - _Role:_ Standard Unreal `UInterface`/`IInterface`. Allows Echelon 2 to query Echelon 1 systems without knowing their concrete C++ classes.
        
- **`Public/ItemSystem/SegFaultItemDefinition.h`**
    
    - _Role:_ Inherits from `UPrimaryDataAsset`. The core configuration file for _any_ item. Contains an array of `USegFaultItemFragment*`.
        
- **`Public/ItemSystem/SegFaultItemFragment.h`**
    
    - _Role:_ `UObject` base class. Purely data. Examples of inherited BP classes: `MeshFragment`, `StatsFragment`. Read-only at runtime.
        

---

#### **Echelon 1: Standalone Systems (`Plugins/`)**

_Self-contained. Compile in parallel. Network-ready._

**Plugin: `SegFaultMovement`**

- **`Public/SegFaultCharacterMovementComponent.h`**
    
    - _Role:_ Inherits from `UCharacterMovementComponent`. Overrides `FSavedMove_Character` for client prediction. Implements Trajectory generation required for **Motion Matching**. Integrates Iris state prediction.
        
- **`Public/SegFaultMovementNetworkData.h`**
    
    - _Role:_ Defines the exact compressed payloads sent to the server for movement validation.
        

**Plugin: `SegFaultGAS`**

- **`Public/SegFaultAbilitySystemComponent.h`**
    
    - _Role:_ Custom ASC. Intercepts `EnhancedInput` tags and routes them to active abilities.
        
- **`Public/Attributes/SegFaultCombatSet.h`**
    
    - _Role:_ `UAttributeSet` containing Health, Armor, Stamina. Replicated via Iris Push Model.
        
- **`Public/Abilities/SegFaultGameplayAbility.h`**
    
    - _Role:_ Base class. Enforces instantiation policies and hooks into the `UGameplayMessageSubsystem` for VFX/SFX triggers.
        

**Plugin: `SegFaultEquipment` (The Core Arsenal)**

- **`Public/Components/SegFaultEquipmentManagerComponent.h`**
    
    - _Role:_ The ActorComponent that lives on the Character. Contains an Iris `FFastArraySerializer` struct (`FSegFaultEquipmentList`) to replicate the currently equipped items efficiently.
        
- **`Public/Instances/SegFaultEquipmentInstance.h`**
    
    - _Role:_ `UObject`. Represents a spawned piece of equipment. Manages the lifecycle of the visual `AActor` (the mesh attached to the socket).
        
- **`Public/Weapons/SegFaultWeaponInstance.h`**
    
    - _Role:_ Inherits `USegFaultEquipmentInstance`. Maintains Lock-free/Push-Model state: `CurrentAmmo`, `Heat`, `Durability`.
        
- **`Public/Weapons/SegFaultRangedWeaponInstance.h`**
    
    - _Role:_ **CRITICAL.** Handles both Hitscan and Projectiles. Integrates **Server Rewind/Rollback** (lag compensation). Stores historical transforms of hitboxes. Validates client-reported hits against server history using `UE::Tasks` for async math.
        

**Plugin: `SegFaultInventory`**

- **`Public/Components/SegFaultInventoryManagerComponent.h`**
    
    - _Role:_ Uses `FFastArraySerializer` (`FSegFaultInventoryList`). Handles grid-based or slot-based math.
        
- **`Public/Instances/SegFaultInventoryItemInstance.h`**
    
    - _Role:_ `UObject`. The runtime representation of an item sitting in a backpack. Holds dynamic data (e.g., specific weapon wear/tear) while referencing the static `USegFaultItemDefinition`.
        

**Plugin: `SegFaultUI`**

- **`Public/Subsystems/SegFaultUIManagerSubsystem.h`**
    
    - _Role:_ `UGameInstanceSubsystem`. Manages UI layers (Game, Menu, Modal) using Lyra's Extension routing.
        
- **`Public/MVVM/SegFaultHealthViewModel.h`**
    
    - _Role:_ MVVM architecture. Listens to GAS attribute changes via Messages, exposes `float HealthPercent` to UMG without UMG casting to Character.
        

---

#### **Echelon 2: Game Application (`Source/SegFault`)**

_The Composition Root. Knows L0 and L1. Wires them together._

- **`Public/GameMode/SegFaultGameModeBase.h`**
    
    - _Role:_ Standard GameMode. Handles match states and parses `GameFeatures` URLs if needed.
        
- **`Public/Player/SegFaultPlayerController.h`**
    
    - _Role:_ Minimal logic. Mostly acts as the network owner for RPCs and the local host for the UI Subsystem.
        
- **`Public/Character/SegFaultModularCharacter.h`**
    
    - _Role:_ The blank slate. Inherits `AModularCharacter`. In the constructor, sets the default sub-object to `USegFaultCharacterMovementComponent`. Contains the `SkeletalMeshComponent` configured for **Motion Matching** via Animation Blueprints.
        
- **`Public/Character/SegFaultPawnExtensionComponent.h`**
    
    - _Role:_ **The Director.** Implements `IGameFrameworkInitStateInterface`. Coordinates the initialization sequence (Wait for Input -> Wait for GAS -> Wait for Inventory -> Ready).
        
- **`Public/Character/SegFaultHeroComponent.h`**
    
    - _Role:_ **The Input Binder.** This component is injected dynamically. It reads a `UDataAsset` containing `EnhancedInput` mappings, binds the hardware keys (Mouse 1) to `GameplayTags` (InputTag.Weapon.Fire), and forwards them to the `SegFaultPawnExtensionComponent` or ASC.
        

#### **Echelon 3: Game Features (Data-Driven Glue)**

_(No C++ headers strictly required here, mostly Data Assets, but if you need custom logic)_

- `GF_ShooterCore/Public/Actions/GameFeatureAction_AddInputContext.h` (Injects the `EnhancedInput` mapping context dynamically when the feature activates).