//
//  Petalo.hpp
//  plant_OF
//
//  Created by Marta Murtas on 04/03/2019.
//
//

#ifndef Petal_hpp
#define Petal_hpp

#include <stdio.h>
#include "ofPoint.h"
#include "ofMain.h"

#endif /* Petal_hpp */

#define STARTING_PETAL_LENGTH 30 // la lunghezza iniziale di un petalo
#define MAX_PETAL_LENGTH 200 // la massima lunghezza di un petalo
#define MIN_PETAL_LENGTH 25 // la minima lunghezza di un petalo

class Petal {
    public:
        // ------------------------------ proprietà ------------------------------
        int frequencyValue; // il valore del petalo
        ofPoint startingPoint; // il punto da cui parte il petalo
        int lunghezza; // la lunghezza del petalo
        float angle; // l'angolo attorno al quale è fatto ruotare il petalo
    
        // ------------------------------ metodi e funzioni ----------------------
        void initialize (int frequencyValue, ofPoint startingPoint, float angle); /* procedura per l'inizializzazione del petalo */
        void updateLength (int update); /* procedura per l'aggiornamento della lunghezza del petalo */
        void setAngle (float angle); /* procedura per l'aggiornamento dell'angolo del petalo */
};