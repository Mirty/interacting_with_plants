//
//  Flower.hpp
//  plant_OF
//
//  Created by Marta Murtas on 04/03/2019.
//
//

#ifndef Flower_hpp
#define Flower_hpp

#include <stdio.h>
#include <vector>
#include "Petal.hpp"
#include "ofColor.h"
#include "ofMain.h"
#include <map>

#endif /* Flower_hpp */

#define BG_COLOR 255

class Flower {
    private:
        std::vector <Petal> petals; // ogni fiore i è composto da xi petali
        ofColor colore; // ogni fiore ha un suo colore
    
    public:
        // ------------------------------ proprietà ------------------------------
        ofPoint startingPoint; // è il punto in cui ha centro il pistillo del fiore
    
        // ------------------------------ metodi e funzioni ----------------------
        void initialize (ofPoint startingPoint, ofColor colore); // procedura per l'inizializzazione dell'istanza
        void addPetal (int frequencyValue, int intervallo); // procedura per l'aggiunta di un nuovo petalo
        void draw (bool isBeingTouched); // procedura per il disegno del fiore. se isBeingTouched è true allora il fiore viene disegnato con maggiore luminosità
        void draw (); // procedura per il disegno del fiore
        void update (int freq, int angle, int max_frequency); // procedura per l'aggiornamento di alcuni valori del fiore
        void update (ofPoint c, int value); // procedura per l'aggiornamento della posizione del fiore
        void setColor (ofColor color); // procedura per il settaggio del colore del fiore
};