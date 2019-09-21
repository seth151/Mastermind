#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>  

void set_random_seed();
int randn(int n);
bool isNumInVector(int integer, const std::vector<int> &vector);

struct mm_code_maker{
    
    void init(int i_length, int i_num){
        length = i_length;
        num = i_num;
    }
    
    void generate_sequence(){
        for(int i = 0; i < length; i++){
            sequence.push_back(randn(num));
        }
    }
    
    void give_feedback(const std::vector<int>& attempt, int& black_hits, int& white_hits){
        /// write here your implementation for this function
        /// which takes in input an attempt
        /// and provides feedback in terms of black hits
        /// and white hits (see linked paper)
        std::vector<int> attemptIndexesUsed;
        std::vector<int> sequenceIndexesUsed;
        black_hits = 0;
        white_hits = 0;

        //we first try to find if there are black hits
        for (int i = 0; i < attempt.size(); i++)
        {
            if (attempt[i] == sequence[i])
            {
                
                black_hits++;
                attemptIndexesUsed.push_back(i);
                sequenceIndexesUsed.push_back(i);
            }
        }

        //now find the white ones - only if they haven't been marked as black already
        for (int i = 0; i < attempt.size(); i++)
        {
            for (int j = 0; j < sequence.size(); j++)
            {
                if (!isNumInVector(i, attemptIndexesUsed) && !isNumInVector(j, sequenceIndexesUsed))
                {
                    //make sure we are not re-using values
                    if (attempt[i] == sequence[j])
                    {
                        white_hits++;
                        

                        attemptIndexesUsed.push_back(i);
                        sequenceIndexesUsed.push_back(j);
                    }
                }
            }
        }
    }

    std::vector<int> sequence;
    
    int length;
    int num;
};

struct mm_solver{
    

    
    int length;
    int num;
    int black_hits; // we want to keep a track of max number of black hits 
    int prev_black_hits; // to keep a track of max number of black hits we managed so far and try to improve on it
    int num_index; // index of the symbol we will try changing
    std::vector<int> prev_attempt;
    
    void init(int i_length, int i_num){
        length = i_length;
        num = i_num;
        black_hits=0;
        prev_black_hits=0;
        num_index=0;
    }
    
    /// this member function creates an attempt to find the right code
    /// this version generates an attempt 0000...attempt.size() and gets feedback
    /// by checking o. of black hits it gradually increases numbers at each index until the number of black hits increases
    void create_attempt(std::vector<int>& attempt){
        
        std::cout<<"black hits="<<black_hits<<std::endl;
        std::cout<<"prev_black_hits="<<prev_black_hits<<std::endl;
                if (prev_attempt.size()==0) // first guess is 00000......attempt.size()
                {
                    for(int i = 0; i < length; i++){
                        attempt.push_back(0);
                    }
                }
                else
                {
                    if (black_hits<prev_black_hits) // if our number of black hits has gone down (this can't happen in the first attempt) we revert our symbol back (we previously must have chosen the next symbol at test_index so we go back to the previous symbol
                    {
                        prev_attempt[num_index]--;
                        num_index++; // the symbol at position num_index is now known to be correct, so we move on to the next position
                    }
                    
                    else
                    if (black_hits>prev_black_hits) // we have made a hit (or hits)
                    {
                        prev_black_hits=black_hits; // let's record the new maximum number of black hits we have
                        if (num_index>0 || prev_attempt[num_index]>0) // the below always applies except for when in our initial attempt (e.g. 0000..) one symbol was a black hit (not necessarily the first one)
                            num_index++; // we have found the right symbol at position num_index so move on to the next position
                    }
                    
                    for (int i = 0; i < length; i++){ // this is where we try the next symbol at the current position (it is making a copy of the previous attempt with the symbol at position test_index increased by one)
                        if (i!=num_index)
                            attempt.push_back(prev_attempt[i]);
                        else
                            attempt.push_back(prev_attempt[i]+1); // stops once you get a black hit
                    }
                }
        
                prev_attempt.clear();
                for(int i = 0; i < length; i++){
                    prev_attempt.push_back(attempt[i]);
                }
    }
    
    //we feed info about new number of blackhits into the solver
    void learn(std::vector<int>& attempt, int newblack_hits, int newwhite_hits){
               
               black_hits=newblack_hits;
               white_hits=newwhite_hits;
        


       }
};

/// the main is the same as in the previous example(mm-2)

int main(){
    
    set_random_seed();
    
    int length, num;
    std::cout << "enter length of sequence and number of possible values:" << std::endl;
    std::cin >> length >> num;
    
    mm_solver solver;
    solver.init(length, num);
    
    mm_code_maker maker;
    maker.init(length, num);
    maker.generate_sequence();
    
    clock_t begin = clock();
    
    int black_hits=0, white_hits=0;
    int attempts_limit = 500000;
    int attempts = 0;
    while((black_hits < length) && (attempts < attempts_limit)){
        std::vector<int> attempt;
        solver.create_attempt(attempt);
        maker.give_feedback(attempt, black_hits, white_hits);
        std::cout << "attempt: " << std::endl;
        for(int i = 0; i < attempt.size(); i++){
            std::cout << attempt[i] << " ";
        }
        std::cout << std::endl;
        std::cout << "black pegs: " << black_hits << " " << " white pegs: " << white_hits << std::endl;
        solver.learn(attempt, black_hits, white_hits);
        attempts++;
    }
    
    if(black_hits == length){
        clock_t end = clock();
        double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
        std::cout << "the solver has found the sequence in " << attempts << " attempts" << std::endl;
        std::cout << "time taken was "<<elapsed_secs<<" sec(s)"<<std::endl;
    }
    else{
        std::cout << "after " << attempts << " attempts still no solution" << std::endl;
    }
    std::cout << "the sequence generated by the code maker was:" << std::endl;
    for(int i = 0; i < maker.sequence.size(); i++){
        std::cout << maker.sequence[i] << " ";
    }
    std::cout << std::endl;
    return 0;
}

void set_random_seed(){
    std::srand(std::time(0));
}

int randn(int n){
    return std::rand() % n;
}
bool isNumInVector(int number, const std::vector<int> &vector)
{
    for (int i = 0; i < vector.size(); i++)
    {
        if (number == vector[i])
        {
            return true;
        }
    }
    return false;
}
