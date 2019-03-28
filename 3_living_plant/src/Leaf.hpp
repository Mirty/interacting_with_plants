//
//  Leaf.hpp
//  bozza_project
//
//  Created by Marta Murtas on 20/03/2019.
//
//

#ifndef Leaf_hpp
#define Leaf_hpp

#include <stdio.h>

#endif /* Leaf_hpp */

#include "ofxBox2d.h"


class Leaf : public ofxBox2dCircle {
    
    private:
        ofTexture * texture; // ogni foglia è rappresentata a schermo tramite una texture
        int max_dim; // la dimensione massima in larghezza o altezza
    
    public:
        Leaf (); // costrutture
        void setTexture (ofTexture * texture); // procedura per settare il puntatore a texture
        void draw (); // procedura per disegnare la foglia
};
