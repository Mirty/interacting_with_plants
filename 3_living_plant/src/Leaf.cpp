//
//  Leaf.cpp
//  bozza_project
//
//  Created by Marta Murtas on 20/03/2019.
//
//

#include "Leaf.hpp"


Leaf::Leaf () {
    // costruttore vuoto
    // per inizializzare la foglia chiamo il metodo setup della classe padre ofxBox2dCircle
};

void Leaf::setTexture (ofTexture * texture) {
    /* procedura per settare il puntatore a texture */
    
    // punto a texture passata come parametro
    this->texture = texture;
    
    // trovo la grandezza minima e massima tra width e height della texture
    int min = fmin (texture->getWidth (), texture->getHeight());
    int max = fmax (texture->getWidth (), texture->getHeight());
    
    // faccio corrispondere la grandezza minima al raggio, e mi calcolo la grandezza massima.
    // questo perché altrimenti ho che quando 2 oggetti collidono si respingono se la distanza tra i loro raggi è <= della somma dei 2 raggi. ma siccome le mie foglie hanno larghezza e altezza diverse tra loro (non sono mai quadrati, ma quasi sempre rettangoli) viene brutto l'effetto di repulsione perché si nota la distanza tra le foglie. allora mappo la dimensione minima sul raggio, e mi calcolo la dimensione massima => min :radius = max : max_dim 
    max_dim = (max * getRadius ()) / min;
};

void Leaf::draw () {
    /* procedura per disegnare la foglia */
    
    // se non è stata creata non la disegno
    if(!isBody()) return;
    
    // alzo di un livello
    ofPushMatrix();
    // mi sposto nel centro della foglia e in z 0
    ofTranslate(getPosition().x, getPosition().y, 0);
    // ruoto di getRotation() gradi in z
    ofRotateDeg(getRotation(), 0, 0, 1);
    // setto il colore a 255 in modo che non colori la foglia di altri colori settati precedentemente
    ofSetColor (255);
    // disegno la texture in 0,0 con grandezza getRadius (), max_dim
    texture->draw (0, 0, getRadius (), max_dim);
    //ofPopStyle(); // DA TOGLIERE FORSE -> https:\//openframeworks.cc/documentation/graphics/ofGraphics/#show_ofPopStyle
    // torno al livello precedente
    ofPopMatrix();
};




