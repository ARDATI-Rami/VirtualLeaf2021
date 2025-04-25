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
void cambium::OnDivide(ParentInfo *parent_info, CellBase *daughter1, CellBase *daughter2) {
    // Rules to be executed after cell division go here
    // (e.g., cell differentiation rules)
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
            qDebug() << "Daughter1 (ID:" << daughter1->Index() << ") is neighbor with bark cell ID:" << idx;
        }
    }

    // Check daughter2's neighbors
    for (auto idx : d2_neighbor_indices) {
        if (std::find(bark_cells.begin(), bark_cells.end(), idx) != bark_cells.end()) {
            d2_neighbors_bark = true; // daughter1 has a neighbor bark cell
            qDebug() << "Daughter2 (ID:" << daughter2->Index() << ") is neighbor with bark cell ID:" << idx;
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

    // Debug print to confirm new cell types
    qDebug() << "New cell types - Daughter1 (ID:" << daughter1->Index() << ") is now type:" << daughter1->CellType()
             << ", Daughter2 (ID:" << daughter2->Index() << ") is now type:" << daughter2->CellType();

    // Print all bark cell IDs after division
    qDebug() << "Bark cells after division:" << bark_cells.size() << "cells:";
    QStringList bark_ids;
    for (auto id : bark_cells) {
        bark_ids << QString::number(id);
    }
    qDebug() << "Barks cells :" << bark_cells.size()  << "cell IDs: [" << bark_ids.join(", ") << "]";
}

/*void cambium::SetCellTypeProperties(CellBase *c) { // Set cell properties
Je sais pas où appeler la méthode pour qu'elle s'éxecute dans VirtualLeaf

  if (c->CellType()==0){
    c-> SetWallStiffness(1.0);
    c-> SetLambdaLength(10);
  }
  else if (c->CellType()==1){
    c-> SetWallStiffness(0.1);
    c-> SetLambdaLength(1);
  }
  else if (c->CellType()==2){
    c-> SetWallStiffness(0.1);
    c-> SetLambdaLength(1);
  }
  else {
    c-> SetWallStiffness(10.0);
    c-> SetLambdaLength(100);
  }
}
*/
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



void cambium::CellHouseKeeping(CellBase *c) { // How cells behave after division
 //See value of lambda_length prperty (i.e Spring constant of wall elements, idk the difference between this and wall stiffness)
 qDebug() << "Processing cell in CellHouseKeeping. Cell ID:" << c->Index()
           << ", Type:" << c->CellType()
           << ", Lambda_Length:" << c->GetLambdaLength();
  /*// See value of the wall stiffness property
  qDebug() << "Processing cell in CellHouseKeeping. Cell ID:" << c->Index()
           << ", Type:" << c->CellType()
           << ", Wall Stiffness:" << c->GetWallStiffness();*/

  // Check if a cambium cell is no longer adjacent to the bark, if not it has to be transformed into a Growing Xylem cell
  if (c->CellType() == 1) {
    //c-> SetLambdaLength(100);
    //c->SetWallStiffness(1.0);

    // Get the indices of neighboring cells
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
      /* It does't work because for the new isolated bark cell, the program still consider a bark cell as a neighbor,
      to resolve this issue actualize the neighbor cell list. */
    }

    // Normal behavior for cambium cells
    c->EnlargeTargetArea(par->cell_expansion_rate);
    if (c->Area() > par->rel_cell_div_threshold * c->BaseArea()) {
      c->Divide();
    }
  }
  else if(c->CellType() == 2) { // If cell is a type 2, grow until it reach 3*BaseArea then transform into a Type 3
    //c->SetWallStiffness(0.1);
    if (c->Area() < 3 * c->BaseArea()) {
      c->EnlargeTargetArea(par->cell_expansion_rate);
    }
    else {
      c->SetCellType(3); // Set grown Type 2 cell to a Type 3
      //c->SetWallStiffness(10.0); // Changes the stiffness of the Type 2 cell to the stiffness of the Type 3 cell
      //c-> SetLambdaLength(100);
    }
  }
  else if (c->CellType() == 0) {
/* If the cell is a bark cell (i.e., type 0), we need to slightly enlarge it to prevent excessive stretching,
   which could cause issues in the simulation. This adjustment ensures stability during runtime. */
    c-> SetLambdaLength(3);
    if (c->Area() < 1 * c->BaseArea()) {
      /*qDebug() << "Type 0 cell being enlarged: Area =" << c->Area()
               << ", BaseArea =" << c->BaseArea()
               << ", Threshold =" << 0.8 * c->BaseArea()
               << ", Expansion rate =" << par->cell_expansion_rate;*/
      c->EnlargeTargetArea(par->cell_expansion_rate);
      c-> BaseArea() = c-> Area();
    }
  }
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
