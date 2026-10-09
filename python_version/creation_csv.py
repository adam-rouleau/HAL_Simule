import csv

liste_input_test_mode = {
    "accel_1" : [], 
    "accel_2" : [], 
    "brake" : [], 
}

# Creation des listes de test
# Tout le monde augmente
for i in range(101):
    liste_input_test_mode["accel_1"].append(i / 100)
    liste_input_test_mode["accel_2"].append(i / 100)
    liste_input_test_mode["brake"].append(i / 100)

# Tout le monde descend
for i in range(101):
    liste_input_test_mode["accel_1"].append((100 - i) / 100)
    liste_input_test_mode["accel_2"].append((100 - i) / 100)
    liste_input_test_mode["brake"].append((100 - i) / 100)

# Accel 1 monte seul        
for i in range(101):
    liste_input_test_mode["accel_1"].append(i / 100)
    liste_input_test_mode["accel_2"].append(0)
    liste_input_test_mode["brake"].append(0)

# Accel 2 monte seul
for i in range(101):
    liste_input_test_mode["accel_1"].append(0)
    liste_input_test_mode["accel_2"].append(i / 100)
    liste_input_test_mode["brake"].append(0)

# Brake monte seul
for i in range(101):
    liste_input_test_mode["accel_1"].append(0)
    liste_input_test_mode["accel_2"].append(0)
    liste_input_test_mode["brake"].append(i / 100)

# Accel 1 & 2 montent ensemble
for i in range(101):
    liste_input_test_mode["brake"].append(0)
    liste_input_test_mode["accel_1"].append(i / 100)
    liste_input_test_mode["accel_2"].append(i / 100)

# Accel 1 monte 2x plus vite que Accel 2
for i in range(101):
    liste_input_test_mode["accel_1"].append(i / 100)
    liste_input_test_mode["accel_2"].append((i // 2) / 100)
    liste_input_test_mode["brake"].append(0)

with open("inputs.csv", "w", newline="") as file:
    writer = csv.DictWriter(file, liste_input_test_mode.keys(), dialect="excel")
    liste_pedales = ["accel_1", "accel_2", "brake"]
    writer.writeheader()
    writer.writerows([{"accel_1" : accel_1, "accel_2" : accel_2, "brake" : brake} for accel_1, accel_2, brake in zip(liste_input_test_mode["accel_1"], liste_input_test_mode["accel_2"], liste_input_test_mode["brake"])])