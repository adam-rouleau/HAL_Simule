from vcu import VCU 

def main():
    test_mode_input = input("Enable Test Mode? [y/n]")
    if test_mode_input == "y":
        test_mode = True
        nb_iterations = int(input("How many iterations? [1, 706]"))
    elif test_mode_input == "n":
        test_mode = False
    else:
        print("Invalid")
        raise NameError("Enter [y/n] next time.")
    
    main_vcu = VCU()
    for i in range():
        main_vcu.update(test_mode)

if __name__ == "__main__":
    main()