#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "sound_system.hpp"
#include <iostream>
#include <cstring>
#include <cmath>
#include <vector>


#define SMOOTH_SPEC
#define SMOOTH_BARS

#define RESOLUTION_X 1366
#define RESOLUTION_Y 768
#define PITCH 4
int main() {



    sf::RenderWindow window(sf::VideoMode(RESOLUTION_X, RESOLUTION_Y), "simple spectrum",sf::Style::Fullscreen);
    sf::Clock clock;

    float spectrumL[SPECTRUMSIZE];
    float spectrumR[SPECTRUMSIZE];





     const char *song_name = "You & Me.mp3";

     sound_system_c sound_system(song_name);
     sound_system.play_music();

     //float *spectrumL, float *spectrumR;

    // Place your initialization logic here
   /* SimpleAudioManager audio;
    audio.Load("Glassworks.mp3");*/


    window.setFramerateLimit(30);
    // Start the game loop
    while (window.isOpen()) {
        // Only run approx 60 times per second

        sf::Event event;




        while (window.pollEvent(event)) {

            // Handle window events
            if (event.type == sf::Event::Closed) window.close();



            // Handle user input
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
            {
                window.close();

            }


        }
        // Place your update and draw logic here


          sound_system.get_spectrum(spectrumL,spectrumR);


          //Smooth the actual spectrum
         #ifdef SMOOTH_SPEC
                  float temp_spectrumL[SPECTRUMSIZE];
                  float temp_spectrumR[SPECTRUMSIZE];
                  memcpy(temp_spectrumL, spectrumL, sizeof(float) * SPECTRUMSIZE);
                  memcpy(temp_spectrumR, spectrumR, sizeof(float) * SPECTRUMSIZE);
                  for(int i = SPECTRUM_START; i < SPECTRUM_END; i++) {
                      spectrumL[i]
                          = 0.1 * (temp_spectrumL[i - 2] + temp_spectrumL[i + 2])
                          + 0.2 * (temp_spectrumL[i - 1] + temp_spectrumL[i + 1])
                          + 0.4 * temp_spectrumL[i];
                      spectrumR[i]
                          = 0.1 * (temp_spectrumR[i - 2] + temp_spectrumR[i + 2])
                          + 0.2 * (temp_spectrumR[i - 1] + temp_spectrumR[i + 1])
                          + 0.4 * temp_spectrumR[i];
                  }
            #endif



        #define BAR_MULT 1.022 //Affects the amount of bars
        int bar_amount = 0; //The amount of bars
        float i = BAR_MULT - 1;
        float start = 0;
        while(start + i <= SPECTRUMSIZE - 1) {
            if(start >= SPECTRUM_START && start + i <= SPECTRUM_END) bar_amount++;
            start+= i;
            i*= BAR_MULT;
        }

        int *bar_start = new int[bar_amount]; //Start of full frequencies
        int *bar_end = new int[bar_amount]; //End of full frequencies
        int *bar_first = new int[bar_amount]; //First non-full frequency
        float *bar_first_mult = new float[bar_amount]; //Mult for first non-full frequency
        int *bar_last = new int[bar_amount]; //Last non-full frequency
        float *bar_last_mult = new float[bar_amount]; //Mult for last non-full frequency

        i = BAR_MULT - 1;
        start = 0;
        while(start < SPECTRUM_START) { //Skip some frequencies
            start+= i;
            i*= BAR_MULT;
        }
        for(int j = 0; j < bar_amount; j++) {
            const float end = start + i;
            bar_start[j] = ceil(start);
            bar_end[j] = floor(end);
            bar_first[j] = floor(start);
            bar_first_mult[j] = bar_start[j] == bar_first[j] ? 0.0 : 1.0 - start + floor(start);
            bar_last[j] = floor(end);
            bar_last_mult[j] = end - floor(end);
            if(bar_first[j] == bar_last[j]) {
                bar_first_mult[j] = end - start;
                bar_last_mult[j] = 0.0;
            }
            start+= i;
            i*= BAR_MULT;
        }

        int frist_bar_x = (RESOLUTION_X-bar_amount*PITCH)/2;


        float *bar1_heights = new float[bar_amount];
                    //Calculate the heights for the bars
                    for(int i = 0; i < bar_amount; i++) {
                        float sumL = spectrumL[bar_first[i]] * bar_first_mult[i] + spectrumL[bar_last[i]] * bar_last_mult[i];
                        float sumR = spectrumR[bar_first[i]] * bar_first_mult[i] + spectrumR[bar_last[i]] * bar_last_mult[i];

                        for(int j = bar_start[i]; j < bar_last[i]; j++) {
                            sumL+= spectrumL[j - 1];
                            sumR+= spectrumR[j - 1];
                        }

                        bar1_heights[i] = std::max((sumL + sumR) * 5.0 - 0.04, 0.0) + 0.015;
                    }
                    //Draw the bars

                    std::vector<sf::RectangleShape> Lines;

                    for(int i = 0; i < bar_amount; i++) {


                        sf::RectangleShape lineR(sf::Vector2f(3,-bar1_heights[i]*35) );
                        sf::RectangleShape lineL(sf::Vector2f(3, bar1_heights[i]*35) );

                        lineR.setPosition(sf::Vector2f(frist_bar_x+i*PITCH,RESOLUTION_Y/2));
                        lineL.setPosition(sf::Vector2f(frist_bar_x+i*PITCH,RESOLUTION_Y/2));

                        float var_collor =  round(bar1_heights[i]*80);
                        if(var_collor > 30) var_collor = 40;
                        float alpha = 5+6*var_collor;
                        if(alpha > 255) alpha = 255;

                        lineR.setFillColor(sf::Color(102+var_collor,0,237.0-var_collor,alpha));
                        lineL.setFillColor(sf::Color(102+var_collor,0,237.0-var_collor,alpha));

                        Lines.push_back(lineR);
                        Lines.push_back(lineL);


                    }




                    while (!Lines.empty())
                      {
                         window.draw(Lines.back());
                         Lines.pop_back();
                      }
                    delete [] bar1_heights;



           window.display();
           window.clear(sf::Color(0,0,0));
           sound_system.update();




    }
    // Place your shutdown logic here
    return 0;
}
