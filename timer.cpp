#include "easylogging++.h"
#include "timer.h"
#include <iostream>
#include <chrono>

Timer::Timer(const std::string& name ="") 
        : name_(name), start_(std::chrono::high_resolution_clock::now()) {
        // CLOG(DEBUG)<<name_<<"Timer started for: " << name_ ;
        // std::cout << "Timer started for: " << name_ << std::endl;
        }
        
Timer::~Timer() {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start_; // 以毫秒为单位
        
        // std::cout << "Timer ended for: " << name_ 
        //           << ". Duration: " << elapsed.count() << " milliseconds." << std::endl;
        CLOG(INFO,TTE_LOGGER)<<name_<< "Timer ended for: " << name_ 
                  << ". Duration: " << elapsed.count() << " milliseconds." ;        
    }


// void Timer::reset() {
//     start_ = std::chrono::high_resolution_clock::now();
// }

// double Timer::elapsed() const {
//     return std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start_).count();
// }
