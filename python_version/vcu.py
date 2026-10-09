from hal import InputChannel, OutputChannel, InternalClock

class BrakeImplausibilityException(Exception):
    def __init__(self, message="Brake Implausibility, Cause Unknown"):
        super.__init__(message)

    def __str__(self):
        return self.args[0] # Permet de retourner le message

class AccelImplausibilityException(Exception):
    def __init__(self, message="Acceleration Pedal Implausibility, Cause Unknown"):
        super.__init__(message)

    def __str__(self):
        return self.args[0] # Permet de retourner le message

class VCU:
    def __init__(self, test_mode=bool):
        self.internal_clock = InternalClock()
        self.brake_channel = InputChannel("brake", self.internal_clock)
        self.accel_channel_1 = InputChannel("accel_1", self.internal_clock)
        self.accel_channel_2 = InputChannel("accel_2", self.internal_clock)
        self.motor_channel_1 = OutputChannel()

    def verification_fail_brake(self, 
                                brake_status:int,
                                brake_reading_failure_count:int,
                                brake_range_failure_count:int,
                                ) -> bool:
        
        if brake_status == None:
            if brake_reading_failure_count == 99:
                return "Brake Reading Failed"
            else:
                brake_reading_failure_count += 1
        elif brake_range_failure_count > 0: 
            brake_reading_failure_count -= 1 # Si on remet a 0 il faudrait que les erreurs soient consecutives et meme si on manque la majorite des readings le vehicule continuerait

        if brake_status > 1 or brake_status < 0:
            if brake_range_failure_count == 99:
                return "Reading Outside of Range"
            else:
                brake_range_failure_count += 1
        elif brake_range_failure_count > 0:
            brake_range_failure_count -= 1
            return False

    def verification_fail_accel(self, 
                                accel_1_status:int,
                                accel_2_status:int, 
                                accel_1_reading_failure_count:int, 
                                accel_1_range_failure_count:int, 
                                accel_2_reading_failure_count:int, 
                                accel_2_range_failure_count:int, 
                                accel_plausibility_failure_count:int,
                                ) -> bool:
        # Verification de l'existence accel 1
        if accel_1_status == None:
            if accel_1_reading_failure_count == 99:
                return "Can't Read Accel 1"
            else:
                accel_1_reading_failure_count += 1
        elif accel_1_reading_failure_count > 0:
            accel_1_reading_failure_count -= 1

        # Verification du range accel 1
        if accel_1_status > 1 or accel_1_status < 0:
            if accel_1_range_failure_count == 99:
                return "Accel 1 Out of Range"
            else:
                accel_1_range_failure_count += 1
        elif accel_1_range_failure_count > 0:
            accel_1_range_failure_count -= 1

        # Verification de l'existence accel 2
        if accel_2_status == None:
            if accel_2_reading_failure_count == 99:
                return "Can't Read Accel 2"
            else:
                accel_2_reading_failure_count += 1
        elif accel_2_reading_failure_count > 0:
            accel_2_reading_failure_count -= 1

        # Verification du range accel 2
        if accel_2_status > 1 or accel_2_status < 0:
            if accel_2_range_failure_count == 99:
                return "Accel 2 Out of Range"
            else:
                accel_2_range_failure_count  += 1
        elif accel_2_range_failure_count > 0:
            accel_2_range_failure_count -= 1

        # Verification de deviation des capteurs
        if (accel_1_status < (accel_2_status*0.9) or accel_2_status > (accel_2_status*1.1)):
            if accel_plausibility_failure_count == 99:
                return "Readings Deviated"
            else: 
                accel_plausibility_failure_count += 1
        elif accel_plausibility_failure_count > 0:
            accel_plausibility_failure_count -= 1

    def verification_bspd(self, 
                          brake_status:int, 
                          accel_1_status:int, 
                          accel_2_status:int,
                          bspd_shutdown:bool,
                          ):
        # Durant la verification, on prend l'acceleration la plus grande 
        highest_accel = max(accel_1_status, accel_2_status)
        if brake_status != 0 and highest_accel > 0.25:
            return True
        elif bspd_shutdown and highest_accel < 0.05:
            return False
        else:
            return True

    def calcul_demande_couple(brake_status:int, 
                              accel_1_status:int, 
                              accel_2_status:int,
                              bspd_shutdown:bool,
                              ) -> int:
        # Techniquement pas besoin de mettre de if ici parce qu'on a le motor_ready mais better safe than sorry.
        if not bspd_shutdown:
            return min(accel_1_status, accel_2_status)
        else:
            return 0

    def update(self, test_mode:bool):
        current_time = self.internal_clock.count_up()
        self.motor_ready = False # S'active seulement apres tous les checks

        # Vérification  de la presence d'un reading, du fitment dans le range et de la plausibility
        brake_reading_failure_count = 0
        brake_range_failure_count = 0
        accel_1_reading_failure_count = 0
        accel_1_range_failure_count = 0
        accel_2_reading_failure_count = 0
        accel_2_range_failure_count = 0
        accel_plausibility_failure_count = 0
        bspd_shutdown = False

        # Readings
        brake_status = self.brake_channel.get_current_value(test_mode)
        accel_1_status = self.accel_channel_1.get_current_value(test_mode)
        accel_2_status = self.accel_channel_2.get_current_value(test_mode)

        # Check de l'acceleration
        acceleration_ok = self.verification_fail_accel(accel_1_status, 
                                                       accel_2_status, 
                                                       accel_1_reading_failure_count, 
                                                       accel_1_range_failure_count,
                                                       accel_2_reading_failure_count, 
                                                       accel_2_range_failure_count,
                                                       accel_plausibility_failure_count,
                                                       )
        if not acceleration_ok:
            self.motor_channel_1.set_current_output(0)
        if not test_mode:
            if acceleration_ok == "Can't read Accel 1":
                raise AccelImplausibilityException("Can't read Accel 1")
            elif acceleration_ok == "Accel 1 Out of Range":
                raise AccelImplausibilityException("Accel 1 Out of Range")
            elif acceleration_ok == "Can't read Accel 2":
                raise AccelImplausibilityException("Can't read Accel 2")
            elif acceleration_ok == "Accel 2 Out of Range":
                raise AccelImplausibilityException("Accel 2 Out of Range")
            elif acceleration_ok == "Reading are deviated":
                raise AccelImplausibilityException("Reading are deviated")

        # Check des Brakes
        brake_ok = self.verification_fail_brake(brake_status, 
                                                brake_reading_failure_count,
                                                brake_range_failure_count,
                                                )
        if not brake_ok:
            self.motor_channel_1.set_current_output(0)
        if not test_mode:
            if brake_ok == "Brake Reading Failed":
                raise BrakeImplausibilityException("Can't get reading")
            if brake_ok == "Brake Reading Outside of Range":
                raise BrakeImplausibilityException("Reading Outside of Range")

        # Check du bspd
        bspd_shutdown = self.verification_bspd(brake_status, 
                                               accel_1_status, 
                                               accel_2_status, 
                                               bspd_shutdown,
                                               )

        demande_couple = self.calcul_demande_couple(brake_status, 
                                                    accel_1_status, 
                                                    accel_2_status
                                                    )

        if acceleration_ok and brake_ok and not(bspd_shutdown) and (not test_mode):
            self.motor_channel_1.set_current_output(demande_couple)

        if test_mode:
            print(f"\
                ------------------------------+             \n \
                Internal Clock: {current_time}              \n \
                Niveau des freins: {brake_status}           \n \
                Etat des freins: {brake_ok}                 \n \
                Niveau d'acceleration 1: {accel_1_status}   \n \
                Niveau d'acceleration 2: {accel_2_status}   \n \
                Etat d'acceleration: {acceleration_ok}      \n \
                Demande de couple: {demande_couple}         \n \
                ------------------------------+             \n \
                "
            )

        return demande_couple