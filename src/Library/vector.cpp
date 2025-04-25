/*
 *
 *  This file is part of the Virtual Leaf.
 *
 *  VirtualLeaf is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  VirtualLeaf is distributed in the hope that it will be useful,
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

// Class qui définit le fonctionnement des vecteurs et des opérations associées

#include <string>      // Pour utiliser les chaînes de caractères (std::string)
#include <stddef.h>    // Définitions standards comme NULL, size_t
#include <ostream>     // Pour l’affichage ou l’écriture de texte
#include <limits.h>    // Pour les limites des types numériques
#include <stdio.h>     // Fonctions de base comme printf
//#include <math.h>    // (Commenté) Fonctions mathématiques comme sqrt, pow, etc.
#include <stdarg.h>    // Pour les fonctions avec un nombre variable d’arguments

#include "sqr.h"       // Fichier externe probablement lié à une fonction carré
#include "pi.h"        // Fichier externe contenant la valeur de Pi ?
#include "vector.h"    // Contient la définition de la classe Vector
#include "tiny.h"      // Autre fichier utile pour ce projet

// Variable de texte (string) pour identifier ce fichier/module (souvent utilisé avec un système de version)
static const std::string _module_id("$Id$");

// Définition de l’opérateur d’affectation (=) pour la classe Vector
void Vector::operator=(const Vector &source) {

  // Vérifie si on essaie d’affecter un vecteur à lui-même
  if (this==&source) return;  // Si c’est le même objet, on ne fait rien

  // Sinon, on copie les coordonnées x, y, z du vecteur source dans l’objet courant
  x = source.x;
  y = source.y;
  z = source.z;
}


// Méthode pour afficher les coordonnées du vecteur
ostream &Vector::print(ostream &os) const {
  // On écrit dans le flux (ex: cout) les valeurs entre parenthèses
  os << "(" << x << ", " << y << ", " << z << ")";
  return os;  // On retourne le flux pour permettre les chaînes d'affichage (ex: cout << v1 << v2)
}

// Permet d’utiliser l’opérateur << directement sur un vecteur
ostream &operator<<(ostream &os, const Vector &v) {
  v.print(os);  // Appelle la méthode print() pour afficher le vecteur
  return os;    // Retourne le flux pour pouvoir enchaîner les affichages
}

// Surcharge de l’opérateur + pour additionner deux vecteurs
Vector Vector::operator+(const Vector &v) const {
  Vector result;       // On crée un nouveau vecteur résultat
  result.x = x + v.x;  // Addition des coordonnées x
  result.y = y + v.y;  // Addition des coordonnées y
  result.z = z + v.z;  // Addition des coordonnées z

  return result;       // On retourne le vecteur résultant
}
// Soustraction et modification directe du vecteur courant
Vector& Vector::operator-=(const Vector &v) {
  x -= v.x;  // Soustraction de x
  y -= v.y;  // Soustraction de y
  z -= v.z;  // Soustraction de z
  return *this;  // Retourne l’objet modifié
}

// Division d’un vecteur par un scalaire — retourne un nouveau vecteur
Vector Vector::operator/(const double divisor) const {
  Vector result;
  result.x = x / divisor;
  result.y = y / divisor;
  result.z = z / divisor;
  return result;
}

// Multiplication d’un vecteur par un scalaire — retourne un nouveau vecteur
Vector Vector::operator*(const double multiplier) const {
  Vector result;
  result.x = x * multiplier;
  result.y = y * multiplier;
  result.z = z * multiplier;
  return result;
}

// Multiplication scalaire à gauche : permet "3 * v" en plus de "v * 3"
Vector operator*(const double multiplier, const Vector &v) {
  Vector result;
  result.x = v.x * multiplier;
  result.y = v.y * multiplier;
  result.z = v.z * multiplier;
  return result;
}

// Division par un scalaire et modification directe
Vector &Vector::operator/=(const double divisor) {
  x /= divisor;
  y /= divisor;
  z /= divisor;
  return *this;
}

// Multiplication par un scalaire et modification directe
Vector &Vector::operator*=(const double multiplier) {
  x *= multiplier;
  y *= multiplier;
  z *= multiplier;
  return *this;
}

Vector Vector::operator*(const Vector &v) const {

  // cross product ("uitproduct")
  Vector result;

  result.x=y*v.z-z*v.y;
  result.y=z*v.x-x*v.z;
  result.z=x*v.y-y*v.x;

  return result;
}


double InnerProduct(const Vector &v1, const Vector &v2) {

  // Inner product ("inproduct")
  double result;
  result=v1.x*v2.x+v1.y*v2.y+v1.z*v2.z;
  return result;
}

double Vector::Angle(const Vector &v) const {

  // angle between this vector and another vector

  // angle is within range of [0,pi] radians

  // angle is arccosine of the inner product over the product of the norms of the vectors

  double cos_angle=InnerProduct(*this,v)/(Norm()*v.Norm());

  // check for computational inaccuracies
  if (cos_angle<=-1)
    return Pi;

  if (cos_angle>=1)
    return 0.;

  double angle=acos(cos_angle);

  return angle;
}

double Vector::SignedAngle(const Vector &v) const {

  // angle between this vector and another vector

  // angle is within range of [-pi,pi] radians

  // angle is arccosine of the inner product over the product of the norms of the vectors

  double cos_angle=InnerProduct(*this,v)/(Norm()*v.Norm());

  // check for computational inaccuracies
  if (cos_angle<=-1)
    return Pi;

  if (cos_angle>=1)
    return 0.;

  double angle=acos(cos_angle);

  double sign = (InnerProduct ( Perp2D(), v ) )>0.?1.:-1.;
  return angle * sign;
}


bool Vector::operator==(const Vector &v) const {

  // "sloppy equal"
  if ((fabs(x-v.x)<TINY) && (fabs(y-v.y)<TINY) && (fabs(z-v.z)<TINY))
    return true;
  else
    return false;
}

bool Vector::operator< (const Vector &v) const {

  // Compare x coordinate
  if (x<v.x) 
    return true;
  else 
    return false;
}


double Vector::SqrNorm(void) const {

  // return the square of the norm
  // added this function to avoid taking the square root and 
  // the square again if this value is needed
  return DSQR(x)+DSQR(y)+DSQR(z);
}

void Vector::Normalise(void) {

  double norm;
  norm=Norm(); // Absolute value;

  if (norm>0.) { // if the norm is 0, don't normalise 
    // (otherwise division by zero occurs)

    (*this)/=norm;
  }
}

Vector Vector::Normalised(void) const {

  double norm;
  norm=Norm(); // Absolute value;

  if (norm>0.) { // if the norm is 0, don't normalise 
    // (otherwise division by zero occurs)
    return (*this)/norm;
  } else {
    return *this;
  }
}

bool Vector::SameDirP(const Vector &v) {

  // return true if the two (parallel) vectors point in the same direction
  //  if (x*v.x>=0 && y*v.y>=0 && z*v.z>=0)
  double angle=Angle(v);

  if (angle<(Pi/2))
    return true;
  else
    return false;
}

double Vector::Max(void) {

  // Find maximum value of vector
  double max;

  max=x > y ? x : y;
  max=max > z ? max : z;

  return max;
}

double Vector::Min(void) {

  // Find minimum value of vector
  double min;

  min=x < y ? x : y;
  min=min < z ? min : z;

  return min;
}

// test

#ifdef TEST

void main() {

  Vector v(1.,2.,3.);

  Vector d;

  d=5*v;

  //  cerr << d << "\n";

  d=v*5;

  //  cerr << d << "\n";

  d=v/5;

  //  cerr << d << "\n";

  //  cerr << d.Norm() << "\n";

  d.Normalise();

  //  cerr << d << "  " << d.Norm() << "\n";
}

#endif 

/* finis */
