# CONVENTIONS.md

## Unreal Engine C++ Project Conventions

This document outlines the coding standards and conventions for developing C++ components in our Unreal Engine project. Following these guidelines ensures code consistency, maintainability, and collaboration efficiency.

## 1. Code Structure and Organization

### 1.1 File Naming Convention
- **Class files**: `ClassName.h` and `ClassName.cpp`
- **Blueprint class files**: `ClassName.generated.h` (auto-generated)
- **Module files**: `ModuleName.Build.cs`, `ModuleName.h`, `ModuleName.cpp`
- **Asset files**: Use descriptive names with underscores (e.g., `BP_Enemy_Character`, `Mat_Player_Sprite`)

### 1.2 Directory Structure
```
Source/
├── ProjectName/
│   ├── Public/          # Header files
│   ├── Private/         # Implementation files
│   └── Modules/         # Custom modules
└── ProjectName.Target.cs
```

### 1.3 Module Organization
- Create separate modules for distinct functionality (e.g., `Gameplay`, `UI`, `Networking`)
- Use `MODULE_NAME.Build.cs` for build configuration

## 2. Class and Variable Naming

### 2.1 Class Names
- PascalCase (e.g., `CharacterController`, `HealthComponent`)
- Prefix with project-specific abbreviation (e.g., `PZ_` for Project Z)
- Use descriptive names that indicate purpose

### 2.2 Variable Names
- PascalCase
- **Public variables**: `bIsEnabled`, `NumPlayers`, `MaxHealth`
- **Private variables**: no prefix
- **Protected variables**: no prefix
- **Static variables**: `gStaticVar` (prefix with g)

### 2.3 Function Names
- PascalCase for public functions: `GetPlayerHealth()`, `SetCharacterPosition()`
- camelCase for private/internal functions: `updateAnimation()`, `checkCollision()`
- Prefix with `Is`, `Has`, `Can` for boolean functions: `IsAlive()`, `HasWeapon()`

## 3. Code Style and Formatting

### 3.1 Indentation
- Use 4 spaces (not tabs)
- Consistent indentation throughout all files

### 3.2 Braces
```cpp
// Correct
if (condition)
{
    // code here
}

// Not recommended
if (condition) {
    // code here
}
```

### 3.3 Line Length
- Maximum 120 characters per line
- Break long lines for readability

### 3.4 Spacing
```cpp
// Correct spacing
int variable = 5;
if (condition == true)
{
    // code
}

// Incorrect spacing
int variable=5;
if(condition==true)
{
    //code
}
```

## 4. Unreal Engine Specific Conventions

### 4.1 UCLASS Declaration
```cpp
UCLASS()
class PROJECTNAME_API AMyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    // Constructor
    AMyCharacter(const FObjectInitializer& ObjectInitializer);

    // Functions
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USceneComponent* RootSceneComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UCameraComponent* CameraComponent;
};
```

### 4.2 UPROPERTY Declaration
```cpp
UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
float AggroRadius;

UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
UHealthComponent* HealthComponent;

UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gameplay")
TSubclassOf<ACharacter> EnemyClass;
```

### 4.3 Blueprint Interaction
- Use `UPROPERTY(BlueprintReadWrite)` for variables you want to modify in Blueprints
- Use `UPROPERTY(EditDefaultsOnly)` for configuration values that shouldn't change at runtime
- Use `UPROPERTY(BlueprintReadOnly)` for read-only properties

## 5. Documentation Standards

### 5.1 Class Documentation
```cpp
/**
 * @brief Manages player health and damage logic
 * 
 * This component handles all health-related functionality including
 * taking damage, healing, and death conditions.
 */
UCLASS()
class PROJECTNAME_API UHealthComponent : public UActorComponent
{
    GENERATED_BODY()
```

### 5.2 Function Documentation
```cpp
/**
 * @brief Applies damage to the character
 * 
 * @param DamageAmount The amount of damage to apply
 * @param DamageType The type of damage being applied
 * @return float The actual damage dealt after modifiers
 */
float ApplyDamage(float DamageAmount, TSubclassOf<UDamageType> DamageType);
```

### 5.3 Comment Style
- Use `//` for single-line comments
- Use `/** */` for documentation blocks
- Comment complex logic and algorithms
- Avoid obvious comments (e.g., `i++; // increment i`)

## 6. Memory Management

### 6.1 Object Lifecycle
- Use `UPROPERTY` for automatic garbage collection
- Properly initialize pointers in constructors
- Avoid manual memory management unless absolutely necessary
- Use smart pointers where applicable

### 6.2 Event Handling
```cpp
// Use proper event system
OnHealthChanged.AddDynamic(this, &AMyCharacter::OnHealthChangedCallback);
```

## 7. Performance Considerations

### 7.1 Tick Function Usage
- Only override `Tick()` if necessary
- Use `PrimaryActorTick.bCanEverTick = true` sparingly
- Optimize tick functions to be lightweight

### 7.2 Data Structures
- Prefer `TArray` over `std::vector` for UE compatibility
- Use `TMap` for key-value lookups
- Consider `TSet` for unique collections

## 8. Best Practices

### 8.1 Error Handling
```cpp
if (IsValid(TargetActor))
{
    TargetActor->TakeDamage(DamageAmount);
}
else
{
    UE_LOG(LogTemp, Warning, TEXT("Target actor is invalid"));
}
```

### 8.2 Null Checks
Always check for null pointers:
```cpp
if (Component != nullptr)
{
    Component->SomeFunction();
}
```

### 8.3 Configuration Management
- Use `UPROPERTY(EditDefaultsOnly)` for configuration values
- Create separate configuration classes for complex settings
- Avoid hardcoding values in source code

### 8.4 Debugging
```cpp
#if WITH_EDITOR
    UE_LOG(LogTemp, Warning, TEXT("Debug message: %s"), *SomeVariable.ToString());
#endif
```

## 9. Version Control

### 9.1 Commit Messages
- Use present tense: "Add character movement system"
- Be descriptive but concise
- Reference related issues or tickets

### 9.2 Branch Naming
- `feature/feature-name`
- `bugfix/issue-description`
- `hotfix/critical-fix`

## 10. Testing and Quality Assurance

### 10.1 Unit Tests
- Create unit tests for core logic components
- Test edge cases and error conditions
- Use UE's testing framework (`UTEST` macros)

### 10.2 Performance Testing
- Profile performance-critical sections
- Monitor memory usage in the editor
- Optimize for both editor and runtime performance

This document serves as a living guideline that should evolve with project needs while maintaining consistency across all team members' contributions to the Unreal Engine C++ project.
