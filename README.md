# Reference Counter Module

A minimal C++ Unreal Engine module for smart reference counting of Unreal objects and structures. This module provides thread-safe reference counting capabilities for Unreal Engine projects, allowing objects to implement safe manual reference management, memory cleanup, and lifecycle signaling.

## Features

- Thread-safe reference counting with atomic counters
- Shared references for structs and objects
- Automatic cleanup logic on all references released
- Easy integration with Unreal Engine modules and Blueprint system
- Virtual hooks for custom cleanup in derived types

## Tech Stack

- **Language**: C++
- **Framework**: Unreal Engine
- **Core Dependencies**: `CoreMinimal.h`, `Modules/ModuleManager.h`
- **Unreal Build System Compatible**

## Installation

**Prerequisites:**
- Unreal Engine (any modern version)
- Ability to add custom modules to your UE project

**Steps:**
1. Copy the files `ReferenceCounterModule.cpp`, `ReferenceCounterModule.h`, and `ReferenceCountable.h` into your project’s `Source/YourModule/` directory.
2. Add `ReferenceCounterModule` to your `.uproject` or `.uplugin`’s `Modules` section.
3. Make sure to regenerate project files and build with your IDE or Unreal Build Tool.

## Usage

#### Creating a reference-counted struct

```cpp
#include "ReferenceCountable.h"

FRefCountedStruct MyRefStruct;
// Use assignment to share reference counters:
FRefCountedStruct AnotherRef = MyRefStruct;

// Increment reference counter automatically
```

#### Manual Reference Release

```cpp
MyRefStruct.Release(); // Decrements reference count. Calls cleanup when zero.
```

#### Checking Reference State

```cpp
if (MyRefStruct.IsReleased())
{
    // The reference is released; cleanup has occurred
}
```

#### Overriding Cleanup in Derived Types

```cpp
struct FMyResource : public FRefCountedStruct
{
    virtual void OnAllReferencesReleased() override
    {
        // Custom cleanup logic
    }
};
```

## API Documentation

### FRefCountedStruct (from `ReferenceCountable.h`)
| Method                   | Description                                 |
|--------------------------|---------------------------------------------|
| `Initialize()`           | Initializes the struct                      |
| `Release()`              | Manually release one reference              |
| `GetRefCount() const`    | Gets current reference count                |
| `IsDestroying() const`   | Is the object being destroyed?              |
| `IsReleased() const`     | Are all references released?                |
| `IsValid() const`        | Is the reference counter in valid state?    |
| `OnAllReferencesReleased()` | Virtual: Implement custom cleanup logic      |

## Contributing

- Ensure code style is consistent with Unreal Engine C++ guidelines.
- Add unit tests for new features or bug fixes.
- Document public APIs and key behaviors inline.
- Create pull requests for review. All contributions are subject to code review.

## License

See [LICENSE.md](LICENSE.md) for detailed licensing information.
