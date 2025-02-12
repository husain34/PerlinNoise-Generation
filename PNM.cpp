#include <iostream>
#include <ctime>
#include <map>
#include <cmath>
#include <fstream>
using namespace std;

map<pair<int,int>,pair<float,float>> Grid;
long int seed;
int scale;
float frequency;
float RandomGradientVector(long int& seed) {
    const long int a = 1103515245;  //Constant multiplier
    const long int c = 12345;   //Constant increment
    const long int m = 32767;   //Constant modulus
    
    seed = abs(seed * a + c);   //Change seed per random number generated
    return ((seed%m)%1000)/1000.0;    //Returns a random number
}

pair<float,float> RandomUnitVector2D(float num){
    float theta = num * 2 * M_PI;
    return {cos(theta),sin(theta)};
}

void GradientGenerator(long &seed,int size){
    for(int i = 0;i < size; i++){
        for (int j = 0; j < size; j++)
        {
            Grid[{i,j}] = RandomUnitVector2D(RandomGradientVector(seed));
        }
        
    }
}
float Fade(float t){
    return t*t*t*(t*(t*6-15)+10);
}
float LERP(float a,float b,float t){
    t = Fade(t);
    return a+t*(b-a);
}
float DotProduct(pair<float,float> target,pair<int,int> grid){
    return ((grid.first-target.first)*Grid[grid].first)+((grid.second-target.second)*Grid[grid].second);
}


float GeneratePerlinNoise(int Size,float x,float y){
    pair<int,int> corner00,corner10,corner01,corner11;
    float dot00,dot10,dot01,dot11;

    corner00 = {(int)x,(int)y};
    corner10 = {corner00.first+1,corner00.second}; 
    corner01 = {corner00.first,corner00.second+1};
    corner11 = {corner00.first+1,corner00.second+1};

    dot00 = DotProduct({x,y},corner00);
    dot10 = DotProduct({x,y},corner10);
    float n1 = LERP(dot00,dot10,x - corner00.first);

    dot01 = DotProduct({x,y},corner01);
    dot11 = DotProduct({x,y},corner11);
    float n2 = LERP(dot01,dot11,x - corner00.first); 
    float noiseValue = LERP(n1,n2,y - corner00.second);
    return noiseValue;

}

void PerlinNoiseMap(int Size) {
    ofstream file("perlin_noise.ppm");
    file << "P3\n" << Size - scale << " " << Size - scale << "\n255\n";

    for (int y = 0; y < Size - scale; y++) {
        for (int x = 0; x < Size - scale; x++) {
            double value = GeneratePerlinNoise(Size, (float)x*frequency / scale, (float)y*frequency / scale);
            int pixel = (int)(pow(((value + 1) / 2),1) * 255);

            float t = pixel / 255.0f;
            int r,g,b;
            if (t < 0.6f) {  // Water
                r = 0;
                g = (int)(50 + 100 * t);
                b = (int)(150 + 100 * t);
            }
            else {  // Island
                r = (int)(50 * (t - 0.6f));  // Darker brown for higher values
                g = (int)(150 + 50 * (t - 0.6f));  // Greenish tint for higher values
                b = (int)(50 * (t - 0.6f));  // Brownish color
            }

            file << r << " " << g << " " << b << " ";
        }
        file << "\n";
    }

    file.close();
}


int main(){
    int size;
    cout << "Enter the size of the grid: ";
    cin >> size;
    cout << "Enter the Seed: ";
    cin >> seed;
    cout << "Enter the Scale: ";
    cin >> scale;
    cout << "Enter the Frequency: ";
    cin >> frequency;
    size = size/scale;
    clock_t start = clock();

    GradientGenerator(seed,size);

    PerlinNoiseMap(size*scale);

    clock_t end = clock();
    double duration = double(end - start) / CLOCKS_PER_SEC;
    cout << "Time taken: " << duration << " seconds" << endl;
    return 0;
}
