//
//  Cell.h
//  conways
//
//  Created by Christian Chinchole on 10/14/18.
//  Copyright © 2018 Christian Chinchole. All rights reserved.
//

#ifndef Cell_h
#define Cell_h

enum State {
    alive,
    dead
};

class Cell {
    
public:
    int neighbors = 0;
    Cell()
    {
        state = State::dead;
    }
    State getState()
    {
        return state;
    }
    void setState(State s)
    {
        state = s;
    }
private:
    State state;
};


#endif /* Cell_h */
