//
//  Particle.cpp
//  vento
//
//  Created by Marta Murtas on 08/03/2019.
//
//

#include "Particle.hpp"


void Particle::initialize (ofPoint pos) {
    /* procedura per l'inizializzazione del disegnino */
    
    this->pos = pos;
    // assegno a x e y di t un valore randomico iniziale, che poi andrò a cambiare a ogni update
    t.x = ofRandom (1000);
    t.y = ofRandom (1000);
};

void Particle::update (ofPoint c, int plantValue, int sparsity) {
    /* procedura per l'aggiornamento  */
    
    // se l'utente setta sparsity a 10, io faccio 10-10=0, quindi t non varia e quindi pos cresce sempre dello stesso tanto.
    // se l'utente setta sparsity a 0, io faccio 10-0=10, quindi t si aggiorna sempre e ottengo valori sempre diversi di ofSignedNoise, quindi pos varia tanto.
    
    // attenzione che a ofSignedNoise non sto passando sparsity ma t, che cambia solo se sparsity != 0, quindi più sparsity è grande + c'è cambiamento, al contrario il movimento sarà costante.
    
    t.x += sparsity * 0.02; // aggiorno il valore di poco per ottenere da ofSignedNoise un nuovo valore
    t.y += sparsity * 0.02;
    // aggiorno t.x e t.y perché ofSignedNoise per una stessa x restituisce sempre la stessa y.
    // se quindi io voglio un movimento altalenante devo darle in input valori sempre diversi.
    // minore è il cambiamento e più risulterà continuo il movimento.
    // maggiore è il cambiamento e più il movimento sembrerà andare a zig zag.
    
    pos.x += ofSignedNoise (t.x) * plantValue/10;
    pos.y += ofSignedNoise (t.y) * plantValue/10;
};