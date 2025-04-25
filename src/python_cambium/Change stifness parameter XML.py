import xml.etree.ElementTree as ET


def update_stiffness_and_lambda(xml_path, output_path):
    # Charger l'arbre XML depuis le chemin passé en argument
    tree = ET.parse(xml_path)
    root = tree.getroot()

    # Compteurs pour suivre les modifications
    stiffness_modified = 0
    lambda_modified = 0

    # Dictionnaires pour suivre les modifications par type
    stiffness_by_type = {0: 0, 1: 0, 2: 0, 3: 0}
    lambda_by_type = {0: 0, 1: 0, 2: 0, 3: 0}

    # Première boucle : mise à jour du stiffness
    print("=== MISE À JOUR DU STIFFNESS ===")
    for cell in root.findall('.//cell'):
        try:
            cell_type = int(cell.get('cell_type'))
            cell_index = int(cell.get('index'))
            old_stiffness = float(cell.get('stiffness'))
            new_stiffness = old_stiffness  # Valeur par défaut

            # Appliquer les règles de mise à jour selon le type cellulaire
            if cell_type == 0:
                new_stiffness = 10000000000000.0
                cell.set('stiffness', str(new_stiffness))
                stiffness_modified += 1
                stiffness_by_type[0] += 1
            elif cell_type == 1:
                new_stiffness = 1.0
                cell.set('stiffness', str(new_stiffness))
                stiffness_modified += 1
                stiffness_by_type[1] += 1
            elif cell_type == 2:
                new_stiffness = 1.0
                cell.set('stiffness', str(new_stiffness))
                stiffness_modified += 1
                stiffness_by_type[2] += 1
            elif cell_type == 3:
                new_stiffness = 100000000000.0
                cell.set('stiffness', str(new_stiffness))
                stiffness_modified += 1
                stiffness_by_type[3] += 1

            # Afficher les détails de la modification si la valeur a changé
            if old_stiffness != new_stiffness:
                print(
                    f"Le stiffness de la cellule {cell_index} du type {cell_type} est passé de {old_stiffness} à {new_stiffness}")

        except (ValueError, TypeError) as e:
            print(f"Erreur avec une cellule: {e}")

    # Deuxième boucle : mise à jour du lambda_celllength
    print("\n=== MISE À JOUR DU LAMBDA_CELLLENGTH ===")
    for cell in root.findall('.//cell'):
        try:
            cell_type = int(cell.get('cell_type'))
            cell_index = int(cell.get('index'))
            old_lambda = float(cell.get('lambda_celllength', '0'))  # Valeur par défaut si l'attribut n'existe pas
            new_lambda = old_lambda  # Valeur par défaut

            # Appliquer les règles de mise à jour selon le type cellulaire
            if cell_type == 0:
                new_lambda = 0.0
                cell.set('lambda_celllength', str(new_lambda))
                lambda_modified += 1
                lambda_by_type[0] += 1
            elif cell_type == 1:
                new_lambda = 0.0
                cell.set('lambda_celllength', str(new_lambda))
                lambda_modified += 1
                lambda_by_type[1] += 1
            elif cell_type == 2:
                new_lambda = 0.0
                cell.set('lambda_celllength', str(new_lambda))
                lambda_modified += 1
                lambda_by_type[2] += 1
            elif cell_type == 3:
                new_lambda = 0.0
                cell.set('lambda_celllength', str(new_lambda))
                lambda_modified += 1
                lambda_by_type[3] += 1

            # Afficher les détails de la modification si la valeur a changé
            if old_lambda != new_lambda:
                print(
                    f"Le lambda_celllength de la cellule {cell_index} du type {cell_type} est passé de {old_lambda} à {new_lambda}")

        except (ValueError, TypeError) as e:
            print(f"Erreur avec une cellule: {e}")

    # Sauvegarder dans le fichier de sortie
    tree.write(output_path, encoding='utf-8', xml_declaration=True)
    print(f"\nFichier mis à jour enregistré dans : {output_path}")
    print(f"Total de {stiffness_modified} cellules modifiées pour stiffness")
    print(f"Total de {lambda_modified} cellules modifiées pour lambda_celllength")
    print(f"Répartition des modifications de stiffness par type: Type 0: {stiffness_by_type[0]}, Type 1: {stiffness_by_type[1]}, "
          f"Type 2: {stiffness_by_type[2]}, Type 3: {stiffness_by_type[3]}")
    print(f"Répartition des modifications de lambda_celllength par type: Type 0: {lambda_by_type[0]}, Type 1: {lambda_by_type[1]}, "
          f"Type 2: {lambda_by_type[2]}, Type 3: {lambda_by_type[3]}")


# Chemin de votre fichier XML
update_stiffness_and_lambda(
    "/mnt/c/Users/Rojas/PycharmProjects/VirtualLeaf2021/data/leaves/cambium_updated.xml",
    "/mnt/c/Users/Rojas/PycharmProjects/VirtualLeaf2021/data/leaves/cambium_updated.xml"
)