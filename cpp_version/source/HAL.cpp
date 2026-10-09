#include "HAL.hpp"

InternalClock::InternalClock() : counter(0) {}

int InternalClock::count_up() {
    return ++counter;
}

int InternalClock::get_time() const {
    return counter;
}

OutputChannel::OutputChannel() : current_output(0.0f) {}

float OutputChannel::set_current_output(float new_output) {
    current_output = new_output;
    return new_output;
};

// Constructeur
// clock_ref est la reference a la valeur du clock, utile parce que sinon tout le monde travaille avec une copie qui n'a pas le bon timing
// nom_pedale_test(nom_pedale) est utile parce que c'est ce qui assigne nom_pedale (la variable) a nom_pedale_test (l'attribut)
InputChannel::InputChannel(std::string nom_pedale, InternalClock& clock_ref) 
    : nom_pedale_test(nom_pedale), clock(clock_ref) {
    
    // Initialisation des clés
    dictionnaire_input_test_mode["accel_1"] = {};
    dictionnaire_input_test_mode["accel_2"] = {};
    dictionnaire_input_test_mode["brake"] = {};

    // OPTIMISATION : Il y a 7 boucles de 101 éléments, soit 707 éléments au total par vecteur.
    // Réserver la mémoire à l'avance évite les réallocations dynamiques ( reallocation / realloc ).
    int total_elements = 7 * 101;
    dictionnaire_input_test_mode["accel_1"].reserve(total_elements);
    dictionnaire_input_test_mode["accel_2"].reserve(total_elements);
    dictionnaire_input_test_mode["brake"].reserve(total_elements);

    // 1. Tout le monde augmente
    for (int i = 0; i <= 100; ++i) {
        float val = static_cast<float>(i) / 100.0f;
        dictionnaire_input_test_mode["accel_1"].push_back(val);
        dictionnaire_input_test_mode["accel_2"].push_back(val);
        dictionnaire_input_test_mode["brake"].push_back(val);
    }

    // 2. Tout le monde descend
    for (int i = 0; i <= 100; ++i) {
        float val = static_cast<float>(100 - i) / 100.0f;
        dictionnaire_input_test_mode["accel_1"].push_back(val);
        dictionnaire_input_test_mode["accel_2"].push_back(val);
        dictionnaire_input_test_mode["brake"].push_back(val);
    }

    // 3. Accel 1 monte seul        
    for (int i = 0; i <= 100; ++i) {
        dictionnaire_input_test_mode["accel_1"].push_back(static_cast<float>(i) / 100.0f);
        dictionnaire_input_test_mode["accel_2"].push_back(0.0f);
        dictionnaire_input_test_mode["brake"].push_back(0.0f);
    }

    // 4. Accel 2 monte seul
    for (int i = 0; i <= 100; ++i) {
        dictionnaire_input_test_mode["accel_1"].push_back(0.0f);
        dictionnaire_input_test_mode["accel_2"].push_back(static_cast<float>(i) / 100.0f);
        dictionnaire_input_test_mode["brake"].push_back(0.0f);
    }

    // 5. Brake monte seul
    for (int i = 0; i <= 100; ++i) {
        dictionnaire_input_test_mode["accel_1"].push_back(0.0f);
        dictionnaire_input_test_mode["accel_2"].push_back(0.0f);
        dictionnaire_input_test_mode["brake"].push_back(static_cast<float>(i) / 100.0f);
    }

    // 6. Accel 1 & 2 montent ensemble
    for (int i = 0; i <= 100; ++i) {
        float val = static_cast<float>(i) / 100.0f;
        dictionnaire_input_test_mode["brake"].push_back(0.0f);
        dictionnaire_input_test_mode["accel_1"].push_back(val);
        dictionnaire_input_test_mode["accel_2"].push_back(val);
    }

    // 7. Accel 1 monte 2x plus vite que Accel 2
    for (int i = 0; i <= 100; ++i) {
        dictionnaire_input_test_mode["accel_1"].push_back(static_cast<float>(i) / 100.0f);
        // (i / 2) en division entière conserve le comportement Python (i // 2)
        dictionnaire_input_test_mode["accel_2"].push_back(static_cast<float>(i / 2) / 100.0f);
        dictionnaire_input_test_mode["brake"].push_back(0.0f);
    }
}

// Pas besoin de lui passer nom_pedale parce que nom_pedale_test est deja un attribut de InputChannel
// Pas besoin de mettre le namespace de nom_pedale_test parce qu'on est deja dans le namespace InputChannel
float InputChannel::get_current_value(bool test_mode) {
    if (test_mode) {
        float moment = clock.get_time();
        if (moment > 0 && static_cast<size_t>(moment - 1) < dictionnaire_input_test_mode[nom_pedale_test].size()) {
                current_value = dictionnaire_input_test_mode[nom_pedale_test][moment - 1];
        }
    }
    return current_value;
}