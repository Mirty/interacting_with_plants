# interacting_with_plants
interacting with plants through Arduino and generating art with openFrameworks 


## the 3 applications
### 1 - fiori stilizzati su dipinto
![alt text](https://github.com/Mirty/interacting_with_plants/blob/master/imgs/Screen%20Shot%202019-03-28%20at%2019.35.30.png)


### 2 - mandala moderno generativo
![alt text](https://github.com/Mirty/interacting_with_plants/blob/master/imgs/Screen%20Shot%202019-03-28%20at%2019.36.11.png)


### 3 - living plant
![alt text](https://github.com/Mirty/interacting_with_plants/blob/master/imgs/Screen%20Shot%202019-03-28%20at%2019.38.44.png)


## setup
I've followed [this guide](https://github.com/robbiebarrat/plant-art), but since the aux cable I bought had 4 wires inside (and not only two as the cable used in that tutorial) I also bought [this adapter](https://www.amazon.it/gp/product/B07GXMLGJG/ref=ppx_yo_dt_b_asin_title_o01_s00?ie=UTF8&psc=1) (I connect my electodes to one end) and [this](https://www.amazon.it/gp/product/B071XCPRNH/ref=ppx_yo_dt_b_asin_title_o00_s00?ie=UTF8&psc=1) (and I connect this to the other end of the first adapter). You can see the result in the picture below.
![alt text](https://github.com/Mirty/interacting_with_plants/blob/master/imgs/setup.jpg)
I added a green led to Arduino, which lights up when I touch the plant.
![alt text](https://github.com/Mirty/interacting_with_plants/blob/master/imgs/setup_arduino.jpg)
![alt text](https://github.com/Mirty/interacting_with_plants/blob/master/imgs/arduino.png)


## addons I've used for this project
* [ofxGui](https://openframeworks.cc/documentation/ofxGui/) - enables the user to edit some program parameters at run time
* [ofxInkSim](https://github.com/Akira-Hayasaka/ofxInkSim) - generates a watercolor kind of drawing
* [ofxTraingle](https://github.com/obviousjim/ofxTriangle) - used for the triangulization of the figures on the screen
* [ofxBox2d](https://github.com/vanderlin/ofxBox2d) - the elements on the screen have a particular physic


To install addons, navigate to your openframeworks addon directory and write
```sh
$ git clone <addon name>
```
you will then be able to include use it or them. 


## what inspired me
* [plant-art project](https://github.com/robbiebarrat/plant-art) - generating art from plants using Arduino and Processing
* [touché experiment](https://www.nickarner.com/blog/2017/7/8/talking-to-plants-touch%C3%A9-experiments) 
* [plants play](https://www.plantsplay.com/) - a device able to generate music from plants


