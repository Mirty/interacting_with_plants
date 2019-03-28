//
//  Petal.cpp
//  plant_OF
//
//  Created by Marta Murtas on 04/03/2019.
//
//

#include "Petal.hpp"

void Petal::initialize(int frequencyValue, ofPoint startingPoint, float angle) {
    /* procedura per l'inizializzazione del petalo */
    
    // la sintassi parla da se
    this->startingPoint = startingPoint;
    this->frequencyValue = frequencyValue;
    this->angle = angle;
    lunghezza = STARTING_PETAL_LENGTH;
}

void Petal::updateLength (int update) {
    /* procedura per l'aggiornamento della lunghezza del petalo */
    
    lunghezza += update;
    
    // limito inferiormente e superiormente la lunghezza
    if (lunghezza < MIN_PETAL_LENGTH) lunghezza = MIN_PETAL_LENGTH;
    if (lunghezza > MAX_PETAL_LENGTH) lunghezza = MAX_PETAL_LENGTH;
}

void Petal::setAngle (float angle) {
    /* procedura per l'aggiornamento dell'angolo del petalo */
    this->angle = angle;
}