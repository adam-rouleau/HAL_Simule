class InputChannel:
    """
    Classe designant un input que la logique du VCU peut lire.
    """
    def __init__(self, nom_pedale_test, clock):
        self.__current_value = int
        self.nom_pedale_test = nom_pedale_test
        self.clock = clock
        self.__liste_input_test_mode = {
            "accel_1" : [], 
            "accel_2" : [], 
            "brake" : [], 
        }

        # Creation des listes de test
        # Tout le monde augmente
        for i in range(101):
            self.__liste_input_test_mode["accel_1"].append(i / 100)
            self.__liste_input_test_mode["accel_2"].append(i / 100)
            self.__liste_input_test_mode["brake"].append(i / 100)
        
        # Tout le monde descend
        for i in range(101):
            self.__liste_input_test_mode["accel_1"].append((100 - i) / 100)
            self.__liste_input_test_mode["accel_2"].append((100 - i) / 100)
            self.__liste_input_test_mode["brake"].append((100 - i) / 100)

        # Accel 1 monte seul        
        for i in range(101):
            self.__liste_input_test_mode["accel_1"].append(i / 100)
            self.__liste_input_test_mode["accel_2"].append(0)
            self.__liste_input_test_mode["brake"].append(0)

        # Accel 2 monte seul
        for i in range(101):
            self.__liste_input_test_mode["accel_1"].append(0)
            self.__liste_input_test_mode["accel_2"].append(i / 100)
            self.__liste_input_test_mode["brake"].append(0)

        # Brake monte seul
        for i in range(101):
            self.__liste_input_test_mode["accel_1"].append(0)
            self.__liste_input_test_mode["accel_2"].append(0)
            self.__liste_input_test_mode["brake"].append(i / 100)

        # Accel 1 & 2 montent ensemble
        for i in range(101):
            self.__liste_input_test_mode["brake"].append(0)
            self.__liste_input_test_mode["accel_1"].append(i / 100)
            self.__liste_input_test_mode["accel_2"].append(i / 100)

        # Accel 1 monte 2x plus vite que Accel 2
        for i in range(101):
            self.__liste_input_test_mode["accel_1"].append(i / 100)
            self.__liste_input_test_mode["accel_2"].append((i // 2) / 100)
            self.__liste_input_test_mode["brake"].append(0)

        print(self.__liste_input_test_mode["accel_1"])
        print(self.__liste_input_test_mode["accel_2"])
        print(self.__liste_input_test_mode["brake"])


    def get_current_value(self, test_mode:bool):
        if test_mode:
            pedale = self.nom_pedale_test
            moment = self.clock.get_time()
            self.__current_value = self.__liste_input_test_mode[pedale][moment]
        return self.__current_value


class OutputChannel:
    def __init__(self):
        self.__current_output = int

    def set_current_output(self, new_output=int):
        self.__current_output = new_output

class InternalClock:
    def __init__(self):
        self.__counter = 0

    def count_up(self):
        self.__counter += 1 
        return self.__counter

    def get_time(self):
        return self.__counter