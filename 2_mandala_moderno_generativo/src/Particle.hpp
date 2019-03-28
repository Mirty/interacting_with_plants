//
//  Particle.hpp
//  vento
//
//  Created by Marta Murtas on 08/03/2019.
//
//

#ifndef Particle_hpp
#define Particle_hpp

#include <stdio.h>
#include "ofMain.h"
#include "ofPoint.h"

#endif /* Particle_hpp */

class Particle {
private:
public:
    // ------------------------------ proprietà ------------------------------
    ofPoint pos; // la posizione della particella (disegnino)
    ofPoint t; // valore in x e y per il rumore di Perlin
    
    // ------------------------------ metodi e funzioni ----------------------
    void initialize (ofPoint pos); // procedura per l'inizializzazione del disegnino
    void update (ofPoint c, int plantValue, int sparsity); // procedura per l'aggiornamento del disegno
};