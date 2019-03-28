//
//  DrawLevel.cpp
//  vento
//
//  Created by Marta Murtas on 08/03/2019.
//
//

#include "DrawLevel.hpp"

void DrawLevel::initialize (string type, int dist_from_center, ofColor color) {
    /* procedura per l'inizializzazione del livello di disegno */
    
    this->type = type; // [line, particle, circle, segment, triangle, rectangle]
    this->dist_from_center = dist_from_center;
    this->color = color;
}

void DrawLevel::updateRepresentation (ofPoint c, int angle, int dimension) {
    /* procedura per l'aggiornamento della rappresentazione di questo livello */
    
    // aggiorno la dimensione dei disegnini
    this->dimension = dimension;
    // calcolo il numero di volte che dovrò disegnare (quindi il numero di disegnini per livello)
    int num_drawings = 360 / angle;
    // verifico se è cambiato qualcosa dalla chiamata precedente, nel caso true allora aggiorno i punti
    if (num_drawings != points.size()) {
        points.clear(); // svuoto il vettore
        float radians = float (angle * PI) / float(180); // calcolo l'angolo in radianti
        Particle p;
        ofPoint pos;
        // a seconda del tipo di disegno faccio delle scelte diverse (per quanto riguarda il centro dei disegnini e la loro dimensione).. quello che è uguale per tutti è che, una volta generato il centro di ogni disegno, lo aggiungo al vettore di punti points.
        if (type == "line" || type == "particle" || type == "circle" || type == "triangle" || type == "rev_triangle") {
            for (int i = 0; i < num_drawings; i++) {
                pos = ofPoint (c.x + cos (radians * i) * dist_from_center, c.y + sin (radians * i) * dist_from_center);
                p.initialize(pos);
                points.push_back (p);
            }
        }
        else if (type == "segment") {
            for (int i = 0; i < num_drawings; i++) {
                pos = ofPoint (c.x + cos (radians * i) * dist_from_center, c.y + sin (radians * i) * dist_from_center);
                p.initialize(pos);
                points.push_back (p);
            }
        }
        else if (type == "rectangle") {
            for (int i = 0; i < num_drawings; i++) {
                pos = ofPoint (c.x + cos (radians * i) * dist_from_center, c.y + sin (radians * i) * dist_from_center);
                p.initialize(pos);
                points.push_back (p);
            }
        }
    }
}

void DrawLevel::updatePoints (ofPoint c, int plantValue, int sparsity) {
    /* procedura per l'aggiornamento dei disegnini del livello */
    
    for (int i = 0; i < points.size(); i++) {
        points[i].update(c, plantValue, sparsity);
    }
    
}

void DrawLevel::draw (ofPoint c, int angle) {
    /* procedura per il disegno del livello */
    
    // setto il colore per questo livello
    ofSetColor (color);
    ofFill ();
    // decido la dimensione del tratto
    int line_width = 3;
    // a seconda del tipo di disegni di questo livello prendo scelte diverse...
    if (type == "line") {
        for (int i = 0; i < points.size(); i++) {
            ofPushMatrix();
            ofTranslate (points[i].pos);
            ofRotateDeg(angle * i);
            ofTranslate (-points[i].pos);
            ofDrawRectangle(points[i].pos.x, points[i].pos.y - dimension, line_width, dimension * 2);
            ofPopMatrix();
            
            // disegno la linea che collega questo punto al successivo
            if (i < points.size() - 1)
                ofDrawLine(points[i].pos.x, points[i].pos.y, points[i+1].pos.x, points[i+1].pos.y);
        }
    }
    else if (type == "particle" || type == "circle") {
        if (type == "circle") ofNoFill();
        for (int i = 0; i < points.size(); i++) {
            // disegno la linea che collega questo punto al successivo
            if (i < points.size() - 1)
                ofDrawLine(points[i].pos.x, points[i].pos.y, points[i+1].pos.x, points[i+1].pos.y);
            ofDrawCircle(points[i].pos.x, points[i].pos.y, dimension, dimension);
           
        }
        ofFill ();
    }
    else if (type == "segment") {
        for (int i = 0; i < points.size(); i++) {
            ofPushMatrix();
            ofTranslate (points[i].pos);
            ofRotateDeg(angle * i);
            ofTranslate (-points[i].pos);
            ofDrawRectangle (points[i].pos.x - dimension, points[i].pos.y, dimension * 2, line_width);
            ofPopMatrix();
            
            // disegno la linea che collega questo punto al successivo
            if (i < points.size() - 1)
                ofDrawLine(points[i].pos.x, points[i].pos.y, points[i+1].pos.x, points[i+1].pos.y);
        }
    }
    else if (type == "rectangle") {
        for (int i = 0; i < points.size(); i++) {
            ofPushMatrix();
            ofTranslate (points[i].pos);
            ofRotateDeg(angle * i);
            ofTranslate (-points[i].pos);
            ofDrawRectangle (points[i].pos.x - dimension, points[i].pos.y - dimension, dimension * 2, dimension * 2);
            ofPopMatrix();
            
            // disegno la linea che collega questo punto al successivo
            if (i < points.size() - 1)
                ofDrawLine(points[i].pos.x, points[i].pos.y, points[i+1].pos.x, points[i+1].pos.y);
        }
        
    }
    else if (type == "triangle") {
        for (int i = 0; i < points.size(); i++) {
            ofPushMatrix();
            ofTranslate (points[i].pos);
            ofRotateDeg(angle * i);
            ofTranslate (-points[i].pos);
            ofDrawTriangle(points[i].pos.x - dimension, points[i].pos.y,
                           points[i].pos.x + dimension, points[i].pos.y,
                           points[i].pos.x, points[i].pos.y - dimension * 2);
            ofPopMatrix();
            
            // disegno la linea che collega questo punto al successivo
            if (i < points.size() - 1)
                ofDrawLine(points[i].pos.x, points[i].pos.y, points[i+1].pos.x, points[i+1].pos.y);
        }
    }
    else if (type == "rev_triangle") {
        for (int i = 0; i < points.size(); i++) {
            ofPushMatrix();
            ofTranslate (points[i].pos);
            ofRotateDeg(angle * i);
            ofTranslate (-points[i].pos);
            ofDrawTriangle(points[i].pos.x, points[i].pos.y,
                           points[i].pos.x + dimension, points[i].pos.y - dimension,
                           points[i].pos.x - dimension, points[i].pos.y - dimension);
            ofPopMatrix();
            
            // disegno la linea che collega questo punto al successivo
            if (i < points.size() - 1)
                ofDrawLine(points[i].pos.x, points[i].pos.y, points[i+1].pos.x, points[i+1].pos.y);
        }
    }
    
    if (points.size() > 0) // disegno la linea che collega l'ultimo punto al primo
        ofDrawLine(points[points.size() - 1].pos.x, points[points.size() - 1].pos.y, points[0].pos.x, points[0].pos.y);
}

