//so far only works on basic ones

#include <iostream>
#include <fstream>
#include <stdlib.h>
#define SUDOKU_SIZE 9
void makeSudokuArrayFromFile(int [SUDOKU_SIZE][SUDOKU_SIZE],std::ifstream&);
void addConstraints(int [SUDOKU_SIZE][SUDOKU_SIZE][SUDOKU_SIZE]);
void addSquareConstraint(int , int ,int [SUDOKU_SIZE][SUDOKU_SIZE][SUDOKU_SIZE], int);
bool checkVals(int [SUDOKU_SIZE][SUDOKU_SIZE][SUDOKU_SIZE]);
void printArrayEntire(int [SUDOKU_SIZE][SUDOKU_SIZE][SUDOKU_SIZE]);
void transformArray(int [SUDOKU_SIZE][SUDOKU_SIZE][SUDOKU_SIZE], int [SUDOKU_SIZE][SUDOKU_SIZE]);
void removeValue(int arr[SUDOKU_SIZE], int deleted);
void print2dArray(int [SUDOKU_SIZE][SUDOKU_SIZE]);
int whereValInRow(int []);
void findSquareWithLowestPossibleChoices(int [SUDOKU_SIZE][SUDOKU_SIZE][SUDOKU_SIZE], int &, int &, int &);
void removeValueSquare(int rowIndex, int colIndex,int valsPossible [SUDOKU_SIZE][SUDOKU_SIZE][SUDOKU_SIZE], int valToBeDeleted, int rowStart, int colStart);
void fillSodukuSolution(int sudokuSolution[SUDOKU_SIZE][SUDOKU_SIZE][SUDOKU_SIZE], int [SUDOKU_SIZE][SUDOKU_SIZE]);
void addRowConstraint(int row, int valsPossible [SUDOKU_SIZE][SUDOKU_SIZE][SUDOKU_SIZE], int valToBeDeleted,int col);
void addColumnConstraint(int col, int valsPossible [SUDOKU_SIZE][SUDOKU_SIZE][SUDOKU_SIZE],int valToBeDeleted, int row);

int main(int argc, const char * argv[]) {
    if(argc!=2){
        printf("Invalid argument amount, need absolute pathname for text file. Thanks (:");
    }
    
    std::ifstream inputSudoku;
    inputSudoku.open(argv[1]);
    if(inputSudoku.is_open()==false){
        printf("Could not open file... Check pathname");
    }
    int sudoku[SUDOKU_SIZE][SUDOKU_SIZE]={{0}};
    int sudokuSolution[SUDOKU_SIZE][SUDOKU_SIZE][SUDOKU_SIZE]={0};
    
    makeSudokuArrayFromFile(sudoku, inputSudoku);
    fillSodukuSolution(sudokuSolution,sudoku);//correct
    addConstraints(sudokuSolution);
    transformArray(sudokuSolution, sudoku);
    print2dArray(sudoku);
    
    
}

void fillSodukuSolution(int sudokuSolution[SUDOKU_SIZE][SUDOKU_SIZE][SUDOKU_SIZE],int sudoku[SUDOKU_SIZE][SUDOKU_SIZE]){//this will give us a clear sudoku, such that all cols and rows have all decision vars as possible or if it already exists in the puzzle, we put that value with all zeros to show its the only possible value
    for(int i=0;i<SUDOKU_SIZE;i++){
        for(int j=0;j<SUDOKU_SIZE;j++){
            if(sudoku[i][j]!=0){
                int z=0;
                sudokuSolution[i][j][z++]=sudoku[i][j];
                for(;z<SUDOKU_SIZE;z++){
                    sudokuSolution[i][j][z]=0;
                }
            }
            else{
                for(int z=1;z<=SUDOKU_SIZE;z++){
                    sudokuSolution[i][j][z-1]=z;
                }
            }
        }
    }
}
bool isDone(int arr[SUDOKU_SIZE]){//returns true if
    int count=0;
    
    for(int i=0;i<SUDOKU_SIZE;i++){
       
        if(arr[i]==0){
            
            
            count++;
        }
    }
   
    if(count==8){
        return true;
    }
    else if(count>8){
        printf("Something is terribly wrong in: isDone (A position no longer has any decision vars left)\n");
        return false;
    }
    else{
        return false;
    }
}

void addConstraints(int sudokuSolution[SUDOKU_SIZE][SUDOKU_SIZE][SUDOKU_SIZE]){
    for(int i=0;i<SUDOKU_SIZE;i++){
        for(int j=0;j<SUDOKU_SIZE;j++){
            if(isDone(sudokuSolution[i][j])==true){//constrain search space
                std::cout<<"adding constraints and pruning..."<<std::endl;
                addRowConstraint(i, sudokuSolution, sudokuSolution[i][j][0], j);
                addColumnConstraint(j, sudokuSolution, sudokuSolution[i][j][0], i);
                addSquareConstraint(i, j, sudokuSolution, sudokuSolution[i][j][0]);
            }
        }
    }//these are the initial ones
    /*
     After we constrain the search space, we must search through the sudoku and find those for isDone==true if we find, then we propogate those constraints in the search space
        if nothing is found, then we make a decision with one of the indices with the lowest number of decision variables left possible
     */
    //See what vals have to be added -> Add value -> apply constraints ---->repeat
    while(checkVals(sudokuSolution)==false){
        std::cout<<"checking values..."<<std::endl;
    }
    
    
    
    
}

bool checkVals(int sudokuSolution[SUDOKU_SIZE][SUDOKU_SIZE][SUDOKU_SIZE]){//THIS IS SUPER INEFFICIENT AND NEEDS TO BE BETTER (but its a proof of concept)
    bool flag=true;
    for(int i=0;i<SUDOKU_SIZE;i++){
        for(int j=0;j<SUDOKU_SIZE;j++){
            if(isDone(sudokuSolution[i][j])==true){
                std::cout<<"adding constraints and pruning..."<<std::endl;
                int index = whereValInRow(sudokuSolution[i][j]);
                addRowConstraint(i, sudokuSolution,sudokuSolution[i][j][index] , j);
                addColumnConstraint(j, sudokuSolution,sudokuSolution[i][j][index] , i);
                addSquareConstraint(i, j, sudokuSolution, sudokuSolution[i][j][index]);
            }
            else{
                flag=false;//this means that we are not done propogating
            }
        }
    }
    return flag;
}

void findSquareWithLowestPossibleChoices(int sudokuSolution[SUDOKU_SIZE][SUDOKU_SIZE][SUDOKU_SIZE], int & row, int & col, int & valIndex){
    row=0;
    int currentMinValues=10;
    col=0;
    valIndex=0;
    for(int i=0;i<SUDOKU_SIZE;i++){
        for(int j=0;j<SUDOKU_SIZE;j++){
            int count=0;
            for(int z=0;z<SUDOKU_SIZE;z++){
                if(sudokuSolution[i][j][z]==0){
                    count++;
                }
            }
            if(currentMinValues>9-count){
                currentMinValues=9-count;
                row=i;
                col=j;
                valIndex = whereValInRow(sudokuSolution[i][j]);
            }
        }
    }
}

int whereValInRow(int arr[]){
    for(int i=0;i<SUDOKU_SIZE;i++){
        if(arr[i]>0){
            return i;
        }
    }
    return -1;//something went wrong
}

void makeSudokuArrayFromFile(int sudokuArray[SUDOKU_SIZE][SUDOKU_SIZE],std::ifstream &input){
    std::string buf;
    int i=0;
    
    while(std::getline(input,buf)){
        size_t pos=0;
        std::string token;
        std::string s = buf;
        int j=0;
        std::string delimiter=",";
        while ((pos = s.find(delimiter)) != std::string::npos||strlen(s.c_str())==1) {
           
           if(strlen(s.c_str())==1){
               token=s;
               s="";
           }
           else{
               token = s.substr(0, pos);
               s.erase(0, pos + delimiter.length());
           }
            
           
           sudokuArray[i][j] = stoi(token);
           j++;
        }
            i++;
    }
}
void addRowConstraint(int row, int valsPossible [SUDOKU_SIZE][SUDOKU_SIZE][SUDOKU_SIZE], int valToBeDeleted,int col){
   
    for(int i=0;i<SUDOKU_SIZE;i++){
        if(i==col){
            
        }
        else{
            
            removeValue(valsPossible[row][i], valToBeDeleted);
        }
    }
    
}
void addColumnConstraint(int col, int valsPossible [SUDOKU_SIZE][SUDOKU_SIZE][SUDOKU_SIZE],int valToBeDeleted, int row){
    for(int i=0;i<SUDOKU_SIZE;i++){
        if(i==row){}
        else{
            removeValue(valsPossible[i][col], valToBeDeleted);
        }
    }
    
}
void addSquareConstraint(int row, int col,int valsPossible [SUDOKU_SIZE][SUDOKU_SIZE][SUDOKU_SIZE], int valToBeDeleted){//the most challenging
    if(row<=2&&col<=2){//first square
        
        removeValueSquare(row, col, valsPossible, valToBeDeleted, 0, 0);
    }
    else if(row<=2&&col<=5){//second square (to the right of first)
        
        removeValueSquare(row, col, valsPossible, valToBeDeleted, 0, 3);
    }
    else if(row<=2&&col<=8){//third square (to the right of second)
        removeValueSquare(row, col, valsPossible, valToBeDeleted, 0, 6);
    }
    else if(row<=5&&col<=2){//second row of squares
        removeValueSquare(row, col, valsPossible, valToBeDeleted, 3, 0);
    }
    else if(row<=5&&col<=5){//etc...
        removeValueSquare(row, col, valsPossible, valToBeDeleted, 3, 3);
    }
    else if(row<=5&&col<=8){
        removeValueSquare(row, col, valsPossible, valToBeDeleted, 3, 6);
    }
    else if(row<=8&&col<=2){
        removeValueSquare(row, col, valsPossible, valToBeDeleted, 6, 0);
    }
    else if(row<=8&&col<=5){
        removeValueSquare(row, col, valsPossible, valToBeDeleted, 6, 3);
    }
    else if(row<=8&&col<=8){
        removeValueSquare(row, col, valsPossible, valToBeDeleted, 6, 6);
    }
}

void removeValueSquare(int rowIndex, int colIndex,int valsPossible [SUDOKU_SIZE][SUDOKU_SIZE][SUDOKU_SIZE], int valToBeDeleted, int rowStart, int colStart){
    for(int i=rowStart;i<rowStart+3;i++){
        for(int j=colStart;j<colStart+3;j++){
            if(i!=rowIndex&&j!=colIndex){
                for(int z=0;z<SUDOKU_SIZE;z++){
                    if(valsPossible[i][j][z]==valToBeDeleted){
                        valsPossible[i][j][z]=0;
                    }
                }
            }
        }
    }
    
    
}

void removeValue(int arr[SUDOKU_SIZE], int deleted){
    for(int i=0;i<SUDOKU_SIZE;i++){
        if(arr[i]==deleted){
            arr[i]=0;
        }
    }
}


void printArrayEntire(int sudokuSolution[SUDOKU_SIZE][SUDOKU_SIZE][SUDOKU_SIZE]){
    
    for(int i=0;i<SUDOKU_SIZE;i++){
        std::cout<<"["<<std::endl;
        for(int j=0;j<SUDOKU_SIZE;j++){
            std::cout<<"["<<std::endl;
            for(int z=0;z<SUDOKU_SIZE;z++){
                std::cout<<sudokuSolution[i][j][z]<<" ";
            }
            std::cout<<"]"<<std::endl;
        }
        std::cout<<"]"<<std::endl;
    }
}

void transformArray(int sudokuSolution[SUDOKU_SIZE][SUDOKU_SIZE][SUDOKU_SIZE], int sudoku[SUDOKU_SIZE][SUDOKU_SIZE]){
    for(int i=0;i<SUDOKU_SIZE;i++){
        for(int j=0;j<SUDOKU_SIZE;j++){
            sudoku[i][j]=sudokuSolution[i][j][whereValInRow(sudokuSolution[i][j])];
        }
    }
    
}
void print2dArray(int arr[SUDOKU_SIZE][SUDOKU_SIZE]){
    for(int i=0;i<SUDOKU_SIZE;i++){
        
        for(int j=0;j<SUDOKU_SIZE;j++){
            std::cout<<arr[i][j]<<" ";
        }
        std::cout<<std::endl;
    }
}
