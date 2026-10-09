#include <string>
#include <map>
#include <vector>

class InternalClock {
private:
    int counter{0};
    
public:
    InternalClock(); //Constructeur

    int count_up();
    int get_time() const;
};

class OutputChannel {
private:
    float current_output{0.0f}; // Rajoute le f pour que ce soit un float

public:
    OutputChannel(); // Constructeur
    float set_current_output(float new_output);
    float get_current_output() const { return current_output; }; // Fonction inline qui retourne le current_output
};

// Classe designant un input que la logique du VCU peut lire.
class InputChannel {
private:
    float current_value{0.0f};
    std::string nom_pedale_test;
    InternalClock& clock; // Le & fait une reference donc tout le monde va chercher la meme et pas une copie (pas besoin en python mais necessaire en c++)

    // Creation du dictionnaire des valeurs input - Fait avec Gemini a partir du code Python
    std::map<std::string, std::vector<float>> dictionnaire_input_test_mode;

public:
    // Constructeur
    InputChannel(std::string nom_pedale, InternalClock& clock_ref);

    float get_current_value(bool test_mode = true);
};

// La difference entre nom_pedale_test et nom_pedale c'est que 
// nom_pedale_test c'est un attribut de InputChannel et pedale_test c'est 
// l'argument qu'on passe au constructeur d'InputChannel