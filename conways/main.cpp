#include <SFML/Graphics.hpp>
#include <unistd.h>
#include <iostream>
#include "ResourcePath.hpp"
#include "Global.h"
#include "Cell.h"


sf::Uint8* pixels = new sf::Uint8[ ((_WIDTH*_HEIGHT))*_ARRSIZE ];
Cell cells[(_WIDTH/_SIZE)][(_HEIGHT/_SIZE)];

void putPixel(int x, int y, int color[])
{
    /*
     
        The unsigned int array holds the pixels as pointers.
     0: Red
     1: Green
     2: Blue
     3: Alpha
     
     */
    
    /* Setup variables to set their intial location */
     x = (x * (_ARRSIZE*_SIZE));
     y = (y * (_SCALAR*_SIZE));
    
    for(int i = 0; i < _SIZE; i++)
        for(int j = 0; j < _SIZE; j++)
        {
            for(int g = 0; g <= (_ARRSIZE-1); g++)
            {
                pixels[ (x+y) + ( (i*_ARRSIZE)+(j*_SCALAR))+g] = color[g];
            }
        }
}


bool checkBounds(int i, int j)
{
    if(i < 0)
        return false;
    else if(i >= _WIDTH/_SIZE)
        return false;
    else if(j < 0)
        return false;
    else if(j >= _HEIGHT/_SIZE)
        return false;
    else
        return true;
}


void updateCells()
{
    /* Get cell neighbors */
    for(int x = 0; x < _WIDTH/_SIZE; x++)
        for(int y = 0; y < _HEIGHT/_SIZE; y++)
        {
            for(int i = -1; i <= 1; i++)
                for(int j = -1; j <= 1; j++)
                {
                    if(!checkBounds(x+i, y+j) || (x+i == x && y+j == y))
                    {
                        continue;
                    }
                    if(cells[x+i][y+j].getState() == State::alive)
                        cells[x][y].neighbors++;
                }
        }
    
    /* Logic for cells */
    for(int x = 0; x < _WIDTH/_SIZE; x++)
        for(int y = 0; y < _HEIGHT/_SIZE; y++)
        {
            
            if(cells[x][y].getState() == State::alive)
            {
                switch(cells[x][y].neighbors)
                {
                    case 2:
                        cells[x][y].setState(State::alive);
                        break;
                    case 3:
                        cells[x][y].setState(State::alive);
                        break;
                    default:
                        cells[x][y].setState(State::dead);
                        break;
                }
            }
            else
            {
                if(cells[x][y].neighbors == 3)
                    cells[x][y].setState(State::alive);
            }
            cells[x][y].neighbors = 0;
        }
    cycles++;
}

void drawCells()
{
    /* Put all the pixels into the pixel array and define their colors */
    for(int x = 0; x < _WIDTH/_SIZE; x++)
        for(int y = 0; y < _HEIGHT/_SIZE; y++)
        {
            if(cells[x][y].getState() == State::alive)
                putPixel(x, y, _ACTIVECOLOR);
            else
                putPixel(x,y, _DEADCOLOR);
        }
}

void resetCells()
{
    /* Clears the cell array */
    for(int x = 0; x < _WIDTH/_SIZE; x++)
        for(int y = 0; y < _HEIGHT/_SIZE; y++)
            cells[x][y].setState(State::dead);
    cycles = 0;
    
}

int main(int, char const**)
{
    sf::RenderWindow window(sf::VideoMode(_WIDTH, _HEIGHT), "SFML window");
    
    /* Creates a texture to lay the pixels onto */
    sf::Texture texture;
    texture.create(_WIDTH, _HEIGHT);
    sf::Sprite sprite(texture);
    
    /* Setup a view for us to pan and zoom */
    sf::View viewPoint( sf::Vector2f(_WIDTH/2, _HEIGHT/2), sf::Vector2f(_WIDTH, _HEIGHT) );
    
    /* Setup Font */
    sf::Font font;
    if (!font.loadFromFile(resourcePath() + "sansation.ttf")) {
        return EXIT_FAILURE;
    }
    
    sf::Text text;
    char str[24];
    sprintf(str, "Cycles: %d", cycles);
    text.setFont(font);
    text.setString(str);
    text.setCharacterSize(60);
    text.setFillColor(sf::Color::Black);
    
    
    
    
    bool ready = false;
    
    
    /* Used for debounce*/
    int prevX=0, prevY=0;
    
    
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            
            if(event.type == sf::Event::KeyPressed)
            {
                if(event.key.code == sf::Keyboard::Q)
                    window.close();
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
                    ready = !ready;
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::C))
                    resetCells();
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::N))
                    updateCells();
                
            }
            
            if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                /* Gets the coords of the mouse and converts them to the map pixels */
                sf::Vector2i tempLoc = sf::Mouse::getPosition(window);
                sf::Vector2f pos = window.mapPixelToCoords(tempLoc);
                int x = pos.x/_SIZE;
                int y = pos.y/_SIZE;
                
                /* Check the mouse click was within the window and set the state of the clicked cell. */
                if(checkBounds(x, y) && !(x == prevX && y == prevY) )
                {  if(cells[x][y].getState() == State::alive)
                        cells[x][y].setState(State::dead);
                    else
                        cells[x][y].setState(State::alive);
                    
                    prevX = x;
                    prevY = y;
                }
                
            }
        }
            /* Movement events */
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) )
                window.close();
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
                viewPoint.zoom(0.94f);
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::X))
                viewPoint.zoom(1.06f);
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                viewPoint.setCenter(viewPoint.getCenter().x-2.68f, viewPoint.getCenter().y);
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                viewPoint.setCenter(viewPoint.getCenter().x+2.68f, viewPoint.getCenter().y);
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
                viewPoint.setCenter(viewPoint.getCenter().x, viewPoint.getCenter().y-2.68f);
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                viewPoint.setCenter(viewPoint.getCenter().x, viewPoint.getCenter().y+2.68f);
        
        
        window.clear(sf::Color::White);
        
        /* Set viewport and draw text */
        window.setView(viewPoint);
        sprintf(str, "Cycles: %d", cycles);
        text.setString(str);
        
        /* Draw Cells */
        drawCells();
        
        /* Set the texture to the pixels */
        texture.update(pixels);
        window.draw(sprite);
        window.draw(text);
        window.display();
        
        if(ready){
            /* Update the cells on timer */
            updateCells();
            usleep(10000);
        }
    }
    
    

    return EXIT_SUCCESS;
}
