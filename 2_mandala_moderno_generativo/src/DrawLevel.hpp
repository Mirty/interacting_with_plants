//
//  DrawLevel.hpp
//  vento
//
//  Created by Marta Murtas on 08/03/2019.
//
//

#ifndef DrawLevel_hpp
#define DrawLevel_hpp

#include <stdio.h>
#include "ofMain.h"
#include "ofColor.h"
#include "Particle.hpp"

#endif /* DrawLevel_hpp */

class DrawLevel {
    private:
    
    public:
        // ------------------------------ proprietà ------------------------------
        string type; // [line, particle, circle, segment, triangle, rectangle]
        int dimension; // la dimensione dei disegnini
        int dist_from_center; // la sua distanza rispetto al centro dello schermo
        vector <Particle> points; // l'insieme di punti da cui il livello è composto
        ofColor color; // il colore di questo livello
    
        // ------------------------------ metodi e funzioni ----------------------
        void initialize (string type, int dist_from_center, ofColor color); // procedura per l'inizializzazione del livello di disegno
        void updatePoints (ofPoint c, int plantValue, int sparsity); // procedura per l'aggiornamento dei disegnini del livello
        void updateRepresentation (ofPoint c, int angle, int dimension); // procedura per l'aggiornamento della rappresentazione di questo livello
        void draw (ofPoint c, int angle); // procedura per il disegno del livello
};