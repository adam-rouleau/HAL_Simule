#include <exception>
#include <string>

#include "HAL.hpp"

// Classe decrivant l'erreur retournee par un probleme de brakes
class BrakeImplausibilityException : public std::exception {
private:
    std::string message;
public:
    explicit BrakeImplausibilityException(std::string msg = "Brake Implausibility, Cause Unknown") : message(msg) {}
    // Sans conversion, definit le message avant d'entrer dans le constructeur
    const char* what() const noexcept override { return message.c_str(); }
    // Equivalent de redefinition de __str__(self) en Python
};

// Classe decrivant l'erreur retournee par un probleme de pedale d'acceleration
class AccelImplausibilityException : public std::exception {
private:
    std::string message;
public:
    explicit AccelImplausibilityException(std::string msg = "Acceleration Pedal Implausibility, Cause Unknown") : message(msg) {}
    const char* what() const noexcept override { return message.c_str(); }
};

// Classe decrivant le VCU
class VCU {
private:
    InternalClock internal_clock;
    InputChannel brake_channel;
    InputChannel accel_channel_1;
    InputChannel accel_channel_2;
    OutputChannel motor_channel;
    bool motor_ready{false};

    // Compteurs
    int brake_reading_failure_count{0};
    int brake_range_failure_count{0};
    int accel_1_reading_failure_count{0};
    int accel_1_range_failure_count{0};
    int accel_2_reading_failure_count{0};
    int accel_2_range_failure_count{0};
    int accel_plausibility_failure_count{0};
    bool bspd_shutdown{false}; 

public:
    VCU();

    std::string verification_fail_brake(
        float brake_status, 
        int& brake_reading_failure_count, 
        int& brake_range_failure_count, 
        bool test_mode
    );

    std::string verification_fail_accel(
        float accel_1_status, 
        float accel_2_status, 
        int& accel_1_reading_failure_count, 
        int& accel_1_range_failure_count, 
        int& accel_2_reading_failure_count, 
        int& accel_2_range_failure_count, 
        int& accel_plausibility_failure_count, 
        bool test_mode
    );

    bool verification_bspd(
        float brake_status, 
        float accel_1_status, 
        float accel_2_status, 
        bool bspd_shutdown
    );

    float calcul_demande_couple(
        float brake_status, 
        float accel_1_status, 
        float accel_2_status, 
        bool bspd_shutdown
    );

    float update(bool test_mode);
};