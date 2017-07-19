#ifndef TIMER_H
#define TIMER_H

#include <thread>
#include <chrono>

class Timer{
    std::thread Thread;
    bool Alive = false;
    long CallNumber = -1L;          // Long integer num
    long repeat_count = -1L;
    std::chrono::milliseconds interval = std::chrono::milliseconds(0);
    std::function<void (void)> funct = nullptr;

    void SleepAndRun(){
        std::this_thread::sleep_for(interval);
        if (Alive)
            Function()();
    }

    // Pass to the thread
    void ThreadFunc(){
    if (CallNumber == Infinite)
        while (Alive)
            SleepAndRun();
    else
        while (repeat_count--)
            SleepAndRun();
    }

    public:
        static const long Infinite = -1L;

        // Empty Timer function
        Timer(){}

        // Accepts a pointer to a function
        Timer (const std::function<void(void)> &f) : funct (f) {}

        Timer (const std::function<void(void)> &f,
               const unsigned long &i,
               const long repeat = Timer::Infinite) : funct (f),
                                                                      interval (std::chrono::milliseconds (i)),
                                                                      CallNumber (repeat) {}
        void Start (bool Async = true){
            if (IsAlive())
                return;
            Alive = true;           // Makes the keylogger to start running
            repeat_count = CallNumber;
            if (Async)
                Thread = std::thread (ThreadFunc, this);
            else
                this -> ThreadFunc();
        }

        // Manually stops timer
        void Stop(){
            Alive = false;
            Thread.join();
        }

        // Getter functions
        bool IsAlive () const {return Alive;}
        long GetLeftCount () const {return repeat_count;}
        long RepeatCount () const {return CallNumber;}
        unsigned long Interval () const {return interval.count();}
        const std::function<void(void)> &Function() const {return funct;}

        // Setter functions
        void RepeatCount (const long r){
            if (Alive)
                return;
            CallNumber = r;
        }

        void SetFunction (const std::function<void(void)> &f) {funct = f;}

        void SetInterval (const unsigned long &i){
            if (Alive)
                return;
            interval = std::chrono::milliseconds (i);
        }
};
#endif // TIMER_H
