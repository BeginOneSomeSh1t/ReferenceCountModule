// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Helpers.h"
#include "ReferenceCountable.generated.h"

USTRUCT(BlueprintType)  
struct REFERENCECOUNTERMODULE_API FRefCountedStruct
{
    GENERATED_BODY()

public:
    FRefCountedStruct()
    {
        Initialize();
    }

    // Copy constructor - shares the same reference counters
    FRefCountedStruct(const FRefCountedStruct& Other)
    {
       if (this != &Other)
       {
           CopyFrom(Other);
           IncrementRefCounter();
       }
    }

    // Assignment operator - handles reference counting properly
    FRefCountedStruct& operator=(const FRefCountedStruct& Other)
    {
        if (this != &Other)
        {
            CopyFrom(Other);
            IncrementRefCounter();
        }
        return *this;
    }

    virtual ~FRefCountedStruct()
    {
        if (bReleased.load(std::memory_order_relaxed))
        {
            return;
        }
        
        const auto Future = AsyncHelpers::ExecuteOnGameThread([this]
        {
             ReleaseReference();
        });

        while (!Future.IsReady())
        {
            Future.WaitFor(FTimespan::FromSeconds(0.1f));
        }
    }

    void Release()
    {
        ReleaseReference();
    }

    // Get current reference count
    int32 GetRefCount() const
    {
        return RefCount ? RefCount->load(std::memory_order_relaxed) : 0;
    }

    // Check if object is being destroyed
    bool IsDestroying() const
    {
        return bIsDestroying ? bIsDestroying->load(std::memory_order_relaxed) : true;
    }

    bool IsReleased() const noexcept
    {
        return bReleased.load(std::memory_order_relaxed);
    }

    virtual bool IsValid() const noexcept
    {
        return RefCount.IsValid();
    }

protected:
    
    // Virtual function called when all references are released
    virtual void OnAllReferencesReleased()
    {
        // Override in derived classes to implement custom cleanup logic
        UE_LOG(LogTemp, Log, TEXT("FRefCountedStruct: All references released"));
    }

    void IncrementRefCounter()
    {
        if (RefCount && !IsDestroying())
        {
            RefCount->fetch_add(1, std::memory_order_relaxed);
        }
    }

private:
    void ReleaseReference()
    {
        if (!IsValid() || IsDestroying() || IsReleased())
        {
            return;
        }

        RefCount->fetch_sub(1, std::memory_order_relaxed);
        bReleased.store(true, std::memory_order_relaxed);
        
        if (RefCount->load(std::memory_order_relaxed) == 0)
        {
            bIsDestroying->store(true, std::memory_order_relaxed);
            OnAllReferencesReleased();
        }
    }

    void Initialize()
    {
        RefCount = MakeShareable(new std::atomic<int32>(1));
        bIsDestroying = MakeShareable(new std::atomic<bool>(false));
    }

    void CopyFrom(const FRefCountedStruct& Other)
    {
        RefCount = Other.RefCount;
        bIsDestroying = Other.bIsDestroying;
    }

    // Shared reference counter - all copies point to the same counter
    TSharedPtr<std::atomic<int32>> RefCount;
    
    // Shared destruction flag - all copies point to the same flag  
    TSharedPtr<std::atomic<bool>> bIsDestroying;

    std::atomic<bool> bReleased = false;
};