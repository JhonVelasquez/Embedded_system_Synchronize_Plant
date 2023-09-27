#include <iostream>

float last_valid_art = 0;
int count_valid = 0;
float pos_art;

int main() {
    
    int n_artif;
    std::cout<<"Enter N° atifacts: ";
    std::cin>>n_artif;
    
    const float step = 81.168948;
    const float start_pos = 0;
    const float dead_distance = 400;
    
    for(float i = 1; i <=  n_artif; i++ ) 
    {
        pos_art = start_pos +  (i-1) * step; // go for each artifact relative position
        
        if ( (pos_art > (last_valid_art + dead_distance)) || (i == 1) ) // is it valid?
        {
            count_valid ++;
            last_valid_art = pos_art;
        }
    }
    
    std::cout<<"\nExpected N° valid: "<<count_valid;
    std::cout<<"\nExpected N° invalid: "<< n_artif- count_valid;

    return 0;
}
