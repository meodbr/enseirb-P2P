def create_file_with_01(filename, n):
    # Créer la chaîne "01" répétée
    content = "01" * (n // 2)
    
    # Si n est impair, ajouter un "0" à la fin
    if n % 2 != 0:
        content += "0"
    
    # Écrire le contenu dans le fichier
    with open(filename, 'w') as file:
        file.write(content)

# Utilisation du script
filename = "output.txt"
n = 1024 * 32000  # Taille du fichier en octets
create_file_with_01(filename, n)
