/*
 *
 *  This file is part of the Virtual Leaf.
 *
 *  The Virtual Leaf is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The Virtual Leaf is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the Virtual Leaf.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2010 Roeland Merks.
 *
 */

#include <QObject>
#include <QtGui>

#include "parameter.h"

#include "wallbase.h"
#include "cellbase.h"
#include "cambium.h"
#include "node.h"

static const std::string _module_id("$Id$");

/*
Cell Types and Their Behavior:

CellType(0) : Bark Cells
- Can grow slightly (prevents potential bugs if restricted completely).
- Cannot divide.
- Stiffness = 10 × Cambium stiffness.

CellType(1) : Cambium Cells
- Can grow until a specific threshold is reached.
- Upon reaching the threshold, the cell divides:
    - If only one daughter cell is in contact with the bark, it becomes CellType(1) (Cambium), and the other becomes CellType(2) (Growing Xylem).
    - If both daughter cells are in contact with the bark, both become CellType(1).

CellType(2) : Growing Xylem Cells
- Can grow until they reach a threshold of 3 × BaseArea().
- Cannot divide.
- When growth limit is reached, they transform into CellType(3) (Mature Xylem).

CellType(3) : Mature Xylem Cells
- Cannot grow or divide.
- Stiffness = 100 × Cambium stiffness.
*/



QString cambium::ModelID(void) {
  // specify the name of your model here
  return QString( "Cambium" );
}

// return the number of chemicals your model uses
int cambium::NChem(void) { return 0; }
// Constructor - add this code to initialize the bark_cells vector
cambium::cambium() {
    // Initialize bark cells with the original values
    bark_cells = {0, 15, 27, 28, 21, 20, 29, 16, 17, 18, 19, 24, 25, 23, 22, 26, 14, 1};
}

// To be executed after cell division

void cambium::AfficherNoeuds(CellBase *c) {
    qDebug() << "=== INFORMATIONS DES NOEUDS DE LA CELLULE " << c->Index() << " ===";
    qDebug() << "Type de cellule: " << c->CellType();
    qDebug() << "Nombre total de noeuds: " << c->getNodes().size();

    int i = 0;
    for (list<Node *>::const_iterator it = c->getNodes().begin(); it != c->getNodes().end(); ++it, ++i) {
        Node* noeud = *it;
        qDebug() << "Nœud " << noeud->Index() << ":";

        qDebug() << "  - À la frontière: " << (noeud->BoundaryP() ? "Oui" : "Non");

    }
    qDebug() << "=========================================";
}

void cambium::SetCellTypeProperties(CellBase *c) { // Set cell properties
/* SetLambdaLength => Cette propriété fout la merde partout :
    -les cellules "coulent" avec une valeur élevée(>2)
    -une valeur très faible pour l'écorce (<0.01) fait que les cellules ne s'immmicent pas entre les cellules d'écorce
Normal c'est le coefficient d'élasticité de la paroi donc elle peut s'allnger de lambda* la valeut initiiale (?)
*/
  if (c->CellType()==0){
    c-> SetWallStiffness(3);
    c-> SetLambdaLength(0);
  }
  else if (c->CellType()==1){
    c-> SetWallStiffness(1);
    c-> SetLambdaLength(0);
  }
  else if (c->CellType()==2){
    c-> SetWallStiffness(1);
    c-> SetLambdaLength(0);
  }
  else {
    c-> SetWallStiffness(1);
    c-> SetLambdaLength(0);
  }
}




void cambium::SetCellColor(CellBase *c, QColor *color) {
  //cell Coloring depending on type
  if (c->CellType()==0){
      if (c->Area() < 0.8 * c->BaseArea()) {
           color->setNamedColor("yellow");
      }
      else {
           color->setNamedColor("brown");
      }
  }
  else if (c->CellType()==1){
        color->setNamedColor("red");
  }
  else if (c->CellType()==2){
        color->setNamedColor("lightblue");
  }
  else {
        color->setNamedColor("blue");
  }
}


void cambium::OnDivide(ParentInfo *parent_info, CellBase *daughter1, CellBase *daughter2) {
    // Rules to be executed after cell division go here
    // (e.g., cell differentiation rules)

// Partie division de la solution n°1 à propos des cellules de cambium
//qui touchent l'extérieur

    // Vérifier si c'est une division spéciale
    int parent_id1 = daughter1->Index(); // Récupérer l'ID de la cellule mère via une fille
    int parent_id2 = daughter2->Index();
    if (special_division_cells.find(parent_id1) != special_division_cells.end() or special_division_cells.find(parent_id2) != special_division_cells.end() ) {
        // Déterminer quelle cellule fille est plus proche de l'extérieur
        bool d1_more_exposed = false;
        bool d2_more_exposed = false;

        // Compter les nœuds exposés pour chaque cellule fille
        int d1_exposed_nodes = 0;
        int d2_exposed_nodes = 0;

        for (auto node_it = daughter1->getNodes().begin(); node_it != daughter1->getNodes().end(); ++node_it) {
            if ((*node_it)->BoundaryP()) d1_exposed_nodes++;
        }

        for (auto node_it = daughter2->getNodes().begin(); node_it != daughter2->getNodes().end(); ++node_it) {
            if ((*node_it)->BoundaryP()) d2_exposed_nodes++;
        }

        d1_more_exposed = (d1_exposed_nodes > d2_exposed_nodes);
        d2_more_exposed = (d2_exposed_nodes > d1_exposed_nodes);

        // Transformer la cellule plus exposée en écorce, l'autre reste cambium
        if (d1_more_exposed) {
            daughter1->SetCellType(0);  // Écorce
            daughter2->SetCellType(1);  // Cambium

            // Ajouter la nouvelle cellule d'écorce à notre liste
            bark_cells.push_back(daughter1->Index());
        } else {
            daughter1->SetCellType(1);  // Cambium
            daughter2->SetCellType(0);  // Écorce

            // Ajouter la nouvelle cellule d'écorce à notre liste
            bark_cells.push_back(daughter2->Index());
        }

        // Supprimer la cellule de notre ensemble après traitement
        special_division_cells.erase(parent_id1);
        special_division_cells.erase(parent_id2);
        return;
    }


    // Construct neighbor lists for both daughter cells
    daughter1->GetNeighborIndices();
    daughter2->GetNeighborIndices();
    // Get neighbor indices for both daughter cells
    std::vector<int> d1_neighbor_indices = daughter1->GetNeighborIndices();
    std::vector<int> d2_neighbor_indices = daughter2->GetNeighborIndices();

    // Check if daughter cells are neighbors with any bark cells
    bool d1_neighbors_bark = false;
    bool d2_neighbors_bark = false;

    // Check daughter1's neighbors
    for (auto idx : d1_neighbor_indices) {
        if (std::find(bark_cells.begin(), bark_cells.end(), idx) != bark_cells.end()) {
            d1_neighbors_bark = true; // daughter1 has a neighbor bark cell
            //qDebug() << "Daughter1 (ID:" << daughter1->Index() << ") is neighbor with bark cell ID:" << idx;
        }
    }

    // Check daughter2's neighbors
    for (auto idx : d2_neighbor_indices) {
        if (std::find(bark_cells.begin(), bark_cells.end(), idx) != bark_cells.end()) {
            d2_neighbors_bark = true; // daughter1 has a neighbor bark cell
            //qDebug() << "Daughter2 (ID:" << daughter2->Index() << ") is neighbor with bark cell ID:" << idx;
        }
    }

    // Handle the case where both daughter cells are neighbors to bark cells
    if (d1_neighbors_bark && d2_neighbors_bark) {
        // Both cells become type 1 (Cambium cells)
        daughter1->SetCellType(1);
        daughter2->SetCellType(1);
    }
    // Handle cases where only one daughter cell is neighbor to bark cells
    else if (d1_neighbors_bark) { // Daughter 1 is neighbor to a bark cell
        daughter1->SetCellType(1); // Daughter 1 becomes a Cambium cell on division
        daughter2->SetCellType(2); // Daughter 2 becomes a Growing Xylem on division
    }
    else if (d2_neighbors_bark) {// Daughter 2 is neighbor to a bark cell
        daughter1->SetCellType(2);// Daughter 1 becomes a Growing Xylem on division
        daughter2->SetCellType(1);// Daughter 2 becomes a Cambium cell on division
    }
    else { // Neither are neighbor to a bark cell.
        daughter1->SetCellType(2);// Daughter 1 becomes a Growing Xylem cell on division
        daughter2->SetCellType(2);// Daughter 2 becomes a Growing Xylem cell on division
    }
    /*
    // Debug print to confirm new cell types
    qDebug() << "New cell types - Daughter1 (ID:" << daughter1->Index() << ") is now type:" << daughter1->CellType()
             << ", Daughter2 (ID:" << daughter2->Index() << ") is now type:" << daughter2->CellType();
    */
    /*
    // Print all bark cell IDs after division
    qDebug() << "Bark cells after division:" << bark_cells.size() << "cells:";
    QStringList bark_ids;
    for (auto id : bark_cells) {
        bark_ids << QString::number(id);
    }
    qDebug() << "Barks cells :" << bark_cells.size()  << "cell IDs: [" << bark_ids.join(", ") << "]";
    */
}




void cambium::CellHouseKeeping(CellBase *c) { // How cells behave after division
  SetCellTypeProperties(c);
 /*//See value of lambda_length prperty (i.e Spring constant of wall elements, idk the difference between this and wall stiffness)
 qDebug() << "Processing cell in CellHouseKeeping. Cell ID:" << c->Index()
           << ", Type:" << c->CellType()
           << ", Lambda_Length:" << c->GetLambdaLength();*/
  /*// See value of the wall stiffness property
  qDebug() << "Processing cell in CellHouseKeeping. Cell ID:" << c->Index()
           << ", Type:" << c->CellType()
           << ", Wall Stiffness:" << c->GetWallStiffness();*/


  // Check if a cambium cell is no longer adjacent to the bark, if not it has to be transformed into a Growing Xylem cell

if (c->CellType() == 1) {

/* Solution n°1 au problème du cambium qui touche l'extérieur :
la cellule de cambium se divise en deux et la cellule la plus a l'extérieur
se transforme en cellule d'écorce.
C'est pas forcément la bonne approche mais ça fonctionne.
Voir solution n°2 dans la méthode CellHouseKeeping partie CellType(0)
*/

    // Vérifier si des nœuds de la cellule sont exposés à l'extérieur
    bool nodeExposed = false;
    Node* exposedNode = nullptr;
    Vector boundaryDirection;
    AfficherNoeuds(c);

    // Parcourir tous les nœuds de la cellule
    for (list<Node *>::const_iterator it = c->getNodes().begin(); it != c->getNodes().end(); ++it) {
        Node* noeud = *it;
        if (noeud->BoundaryP()) {  // Correction: noeud au lieu de node
            nodeExposed = true;
            exposedNode = noeud;

            qDebug() << "=== DIVISION CELLULE EXPOSÉE ===";
            qDebug() << "Cellule cambium ID:" << c->Index() << "a un nœud exposé";

            // Forcer la division selon cet axe
            c->Divide();

            // OnDivide s'occupera du reste
            special_division_cells.insert(c->Index());
            return;  // Sortir après avoir initié la division
        }
    }

    // Si aucun nœud exposé n'a été trouvé
    if (!nodeExposed) {
        qDebug() << "Aucun nœud exposé trouvé pour la cellule cambium ID:" << c->Index();
    }

/* Solution pour les cellules de cambium qui se retrouvent au milieu
du tissu de Xylème :
- It does't work because for the new isolated bark cell, the program still consider a bark cell as a neighbor,
to resolve this issue actualize the neighbor cell list.


    std::vector<int> neighbor_indices = c->GetNeighborIndices();
    //c->SetWallStiffness(1.0);
    // Check if at least one neighbor is a bark cell
    bool has_bark_neighbor = false;
    for (auto idx : neighbor_indices) {
      if (std::find(bark_cells.begin(), bark_cells.end(), idx) != bark_cells.end()) {
        has_bark_neighbor = true;
        break;
      }
    }

    // Logs if cambium cell has no neighbor bark cell before transforming it into bark cell
    if (!has_bark_neighbor) {
      qDebug() << "=== CAMBIUM ISOLÉ DÉTECTÉ ===";
      qDebug() << "Cellule ID:" << c->Index() << ", Area:" << c->Area() << ", BaseArea:" << c->BaseArea();
      qDebug() << "Position: (" << c->Centroid().x << "," << c->Centroid().y << ")";

      // Liste des indices des voisins (sans accéder aux objets voisins)
      qDebug() << "Indices des" << neighbor_indices.size() << "voisins:";
      for (auto idx : neighbor_indices) {
        qDebug() << "  - Voisin ID:" << idx;
      }

      // Vérifie si les voisins sont des cellules d'écorce
      qDebug() << "Vérification bark_cells:";
      for (auto bark_idx : bark_cells) {
        qDebug() << "  - Bark cell ID:" << bark_idx;
      }

      qDebug() << "Analyse: La cellule cambium est isolée, transformation en Growing Xylem (type 2)";
      qDebug() << "==============================";

      c->SetCellType(2);

      return;

    }
*/

    // Normal behavior for cambium cells
    c->EnlargeTargetArea(par->cell_expansion_rate);
    if (c->Area() > par->rel_cell_div_threshold * c->BaseArea()) {
      c->Divide();
    }
  }
  else if(c->CellType() == 2) { // If cell is a type 2, grow until it reach 3*BaseArea then transform into a Type 3

    if (c->Area() < 3 * c->BaseArea()) {
      c->EnlargeTargetArea(par->cell_expansion_rate);
    }
    else {
      c->SetCellType(3); // Set grown Type 2 cell to a Type 3

    }
  }
  else if (c->CellType() == 0) {
/* If the cell is a bark cell (i.e., type 0), we need to slightly enlarge it to prevent excessive stretching,
   which could cause issues in the simulation. This adjustment ensures stability during runtime. */

    double perimeter = c->ExactCircumference(); // perimètre de ma cellule
    double area = c->Area();
    double ratio = perimeter / sqrt(area);

    if (ratio > 6.0) {
        // La cellule est trop déformée, augmenter sa rigidité
        c->SetWallStiffness(c->GetWallStiffness() * 1.5);
        // Forcer une légère expansion
        c->EnlargeTargetArea(2.0 * par->cell_expansion_rate);

        }
    }
    //if ()

}

void cambium::CelltoCellTransport(Wall *w, double *dchem_c1, double *dchem_c2) {
  // add biochemical transport rules here
}
void cambium::WallDynamics(Wall *w, double *dw1, double *dw2) {
  // add biochemical networks for reactions occuring at walls here
}
void cambium::CellDynamics(CellBase *c, double *dchem) {
  // add biochemical networks for intracellular reactions here
}
#include <set>  // Ajoutez cet include en haut du fichier




//Q_EXPORT_PLUGIN2(cambium, cambium)
