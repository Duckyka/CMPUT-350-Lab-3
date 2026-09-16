#ifndef SHARED_PTR_HEADER
#define SHARED_PTR_HEADER

#include <utility>

class ControlBlockBase {
public:
    ControlBlockBase() // TODO: implement the default constructor.
    {
        refNumber = 1;
    }

    // dtor is virtual, so that we can call derived class's dtor from a ptr to this base class.
    virtual ~ControlBlockBase(); // TODO: implement the destructor.

    // pure virtual function; must be overriden by derived classes
    virtual void* managedAddress() = 0;

    // Delete copies, which also implicitly deletes moves.
    ControlBlockBase(const ControlBlockBase&) = delete;
    ControlBlockBase& operator=(const ControlBlockBase&) = delete;

    long increment()
    {
        // TODO: increment refcount by 1 and return result.
        refNumber = refNumber + 1;
        return refNumber;
    }

    long decrement()
    {
        // TODO: decrement refcount by 1 and return result.
        refNumber = refNumber - 1;
        return refNumber;
    }

    long refCount() const
    {
        // TODO: just return the refcount.
        return refNumber;
    }

private:
    // TODO: add field(s) which both control block types need to have
    int refNumber;
};


template <typename T>
class ControlBlock : public ControlBlockBase {

public: 
    ControlBlock(T* pointer = nullptr)
        :   tPtr { pointer }
    {
    }

    ~ControlBlock() override
    {
        if (&refCount == 0)
        {
            delete tPtr;
        }
    }

    void* managedAddress() override
    {
        return &tPtr;
    }
private: 
    T* tPtr;
};


template <typename T>
class SharedPtr {
public:
    SharedPtr(T* storedPointer = nullptr, T* controlBlockPointer = nullptr)
        : storedPtr { storedPointer }, 
        controlBlockPtr { controlBlockPointer }
    {
    }

    SharedPtr(T* storedPointer)
    {
        storedPtr = std::move(storedPointer);
        *controlBlockPtr = new ControlBlock<T>;
    }

    ~SharedPtr()
    {
        release();
    }

    //Copy Constructor
    SharedPtr(const SharedPtr& secondValue)
    {
        storedPtr = secondValue.storedPtr;
        controlBlockPtr.refCount() = secondValue.controlBlockPtr.refCount();
        controlBlockPtr.increment();
    }

    //Move Constructor
    SharedPtr(SharedPtr&& secondValue) noexcept
    {
        storedPtr = secondValue.storedPtr;
        controlBlockPtr = secondValue.controlBlockPtr;
        secondValue.storedPtr = nullptr;
        secondValue.controlBlockPtr = nullptr;
    }

    //Copy Assignment Operator
    SharedPtr& operator=(const SharedPtr& secondValue)
    {
        if (this != secondValue)
        {
            release();
            storedPtr = secondValue.storedPtr;
            *controlBlockPtr = *secondValue.controlBlockPtr;
            controlBlockPtr.increment();
        }
        return *storedPtr;
    }

    //Move Assignment Operator
    SharedPtr& operator=(SharedPtr&& secondValue) noexcept
    {
        if (this != &secondValue)
        {
            release();
            storedPtr = secondValue.storedPtr;
            controlBlockPtr = secondValue.controlBlockPtr;
            secondValue.storedPtr = nullptr;
            secondValue.controlBlockPtr = nullptr;
        }
        return *this;
    }

    //Dereference Operator
    T& operator*() { return *storedPtr; }

    //Arrow Operator
    T* operator->() { return storedPtr; }

    //Get() Function
    T* get() { return storedPtr; }

    //Equality Comparison Operator
    bool operator==(const SharedPtr<T>& secondValue)
        {
            if (this->get() == secondValue.get())
            { return true; }
            else
            { return false; }
        }

    //Boolean Conversion Operator
    operator bool()
        {
            if (*storedPtr == 0)
            { return false; }
            else
            { return true; }
        }

    //Swap() Function
    void swap(SharedPtr<T>& secondValue)
        {
            std::swap(storedPtr, secondValue.storedPtr);
        }

    //Reset() Function
    void reset()
        {
            T* oldPtr = storedPtr;
            storedPtr = nullptr;
            if (oldPtr)
            {
                delete oldPtr;
            }
        }

    //Other Reset()
    void reset(T* newPtr)
    {
        T* oldPtr = storedPtr;
        storedPtr = newPtr;
        if (oldPtr)
        {
            delete oldPtr;
        }
        controlBlockPtr = new ControlBlock<T>;
    }

    //UseCount() Function
    int useCount() { return controlBlockPtr.refCount(); }
private:
    T* storedPtr;
    T* controlBlockPtr;

    //Helper Function, frees resources of both pointers when called (if there is a valid control block)
    void release()
    {
        if (controlBlockPtr)
        {
            controlBlockPtr.decrement();
            if (controlBlockPtr.refCount() == 0)
            {
                delete storedPtr;
                delete controlBlockPtr;
            }
        }
    }
};

template <typename T, typename... Args>
SharedPtr<T> makeSharedBasic(Args&&... args)
{
    return new ControlBlock<T>(std::forward<Args>(args)...);
}

#endif
