#include <iostream>
#include <algorithm>

#include "VCU.hpp"

VCU::VCU() : 
    internal_clock(), 
    brake_channel("brake", internal_clock), 
    accel_channel_1("accel_1", internal_clock), 
    accel_channel_2("accel_2", internal_clock), 
    motor_channel() {}

std::string VCU::verification_fail_brake(float brake_status, 
                                        int& brake_reading_failure_count, 
                                        int& brake_range_failure_count, 
                                        bool test_mode) {
    if (brake_status > 1.0f || brake_status < 0.0f) {
        if (test_mode || brake_range_failure_count == 99) {
            return "Reading Outside of Range";
        }
        else {
            brake_range_failure_count++;
        }
    }
    else if (brake_range_failure_count > 0) {
        brake_range_failure_count--;
    }
    return "";
}

std::string VCU::verification_fail_accel(float accel_1_status, float accel_2_status,
                                        int& accel_1_reading_failure_count, int& accel_1_range_failure_count,
                                        int& accel_2_reading_failure_count, int& accel_2_range_failure_count,
                                        int& accel_plausibility_failure_count, 
                                        bool test_mode) {
    if (accel_1_status > 1.0f || accel_1_status < 0.0f) {
        if (test_mode || accel_1_range_failure_count == 99) { // si on est dans le test mode on retourne tout de suite sinon on voit pas avec les tests si le systeme fonctionne
            return "Accel 1 Out of Range";
        }
        else {
            accel_1_range_failure_count++;
        }
    } 
    else if (accel_1_range_failure_count > 0) {
        accel_1_range_failure_count--;
    }

    if (accel_2_status > 1.0f || accel_2_status < 0.0f) {
        if (test_mode || accel_2_range_failure_count == 99) {
            return "Accel 2 Out of Range";
        }
        else {
            accel_2_range_failure_count++;
        }
    } 
    else if (accel_2_range_failure_count > 0) {
        accel_2_range_failure_count--;
    }

    if (accel_1_status < (accel_2_status * 0.9f) || accel_1_status > (accel_2_status * 1.1f)) {
        if (test_mode || accel_plausibility_failure_count == 99) {
            return "Readings Deviated";
        }
        else {
            accel_plausibility_failure_count++;
        }
    } 
    else if (accel_plausibility_failure_count > 0) {
        accel_plausibility_failure_count--;
    }

    return "";
}

bool VCU::verification_bspd(float brake_status, 
                            float accel_1_status, 
                            float accel_2_status, 
                            bool bspd_shutdown) {
    float highest_accel = std::max(accel_1_status, accel_2_status);
    if (brake_status != 0.0f && highest_accel > 0.25f) {
        return true;
    } 
    else if (bspd_shutdown && highest_accel < 0.05f) {
        return false;
    } 
    else {
        return true;
    }
}

float VCU::calcul_demande_couple(float brake_status, 
                                float accel_1_status, 
                                float accel_2_status, 
                                bool bspd_shutdown) {
    if (!bspd_shutdown) {
        return std::min(accel_1_status, accel_2_status);
    } else {
        return 0.0f;
    }
}

float VCU::update(bool test_mode) {
    int current_time = internal_clock.count_up();
    motor_ready = false;

    float brake_status = brake_channel.get_current_value(test_mode);
    float accel_1_status = accel_channel_1.get_current_value(test_mode);
    float accel_2_status = accel_channel_2.get_current_value(test_mode);

    std::string accel_err = verification_fail_accel(accel_1_status, accel_2_status,
                                                    accel_1_reading_failure_count, accel_1_range_failure_count,
                                                    accel_2_reading_failure_count, accel_2_range_failure_count,
                                                    accel_plausibility_failure_count, 
                                                    test_mode);

    bool acceleration_ok = accel_err.empty(); // Verifie s'il y a une erreur dans accel_err qui a fait toutes les verifications
    if (!acceleration_ok) motor_channel.set_current_output(0.0f);

    if (!test_mode && !acceleration_ok) {
        throw AccelImplausibilityException(accel_err); // C'est ce qui va donner une erreur quand on est pas dans le mode de test
    }

    std::string brake_err = verification_fail_brake(brake_status, brake_reading_failure_count, brake_range_failure_count, test_mode);
    bool brake_ok = brake_err.empty();
    if (!brake_ok) motor_channel.set_current_output(0.0f);

    if (!test_mode && !brake_ok) {
        throw BrakeImplausibilityException(brake_err);
    }

    bspd_shutdown = verification_bspd(brake_status, accel_1_status, accel_2_status, bspd_shutdown);
    float demande_couple = calcul_demande_couple(brake_status, accel_1_status, accel_2_status, bspd_shutdown);

    if (acceleration_ok && brake_ok && !bspd_shutdown && !test_mode) {
        motor_channel.set_current_output(demande_couple);
    }

    if (test_mode) {
        std::cout << "------------------------------+\n"
                  << "Internal Clock: " << current_time << "\n"
                  << "Niveau des freins: " << brake_status << "\n"
                  << "Etat des freins: " << (brake_ok ? "OK" : brake_err) << "\n"
                  << "Niveau d'acceleration 1: " << accel_1_status << "\n"
                  << "Niveau d'acceleration 2: " << accel_2_status << "\n"
                  << "Etat d'acceleration: " << (acceleration_ok ? "OK" : accel_err) << "\n"
                  << "Demande de couple: " << demande_couple << "\n"
                  << "------------------------------+" << std::endl;
    }

    return demande_couple;
}