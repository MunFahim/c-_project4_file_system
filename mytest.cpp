/*
    Name: Muntasir Fahim
    Date: 11/30/24
    File: mytest.cpp

*/

#include "filesys.h"
#include <math.h>
#include <algorithm>
#include <random>
#include <vector>
using namespace std;
enum RANDOM {UNIFORMINT, UNIFORMREAL, NORMAL, SHUFFLE};
class Random {
public:
    Random(){}
    Random(int min, int max, RANDOM type=UNIFORMINT, int mean=50, int stdev=20) : m_min(min), m_max(max), m_type(type)
    {
        if (type == NORMAL){
            //the case of NORMAL to generate integer numbers with normal distribution
            m_generator = std::mt19937(m_device());
            //the data set will have the mean of 50 (default) and standard deviation of 20 (default)
            //the mean and standard deviation can change by passing new values to constructor 
            m_normdist = std::normal_distribution<>(mean,stdev);
        }
        else if (type == UNIFORMINT) {
            //the case of UNIFORMINT to generate integer numbers
            // Using a fixed seed value generates always the same sequence
            // of pseudorandom numbers, e.g. reproducing scientific experiments
            // here it helps us with testing since the same sequence repeats
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_unidist = std::uniform_int_distribution<>(min,max);
        }
        else if (type == UNIFORMREAL) { //the case of UNIFORMREAL to generate real numbers
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_uniReal = std::uniform_real_distribution<double>((double)min,(double)max);
        }
        else { //the case of SHUFFLE to generate every number only once
            m_generator = std::mt19937(m_device());
        }
    }
    void setSeed(int seedNum){
        // we have set a default value for seed in constructor
        // we can change the seed by calling this function after constructor call
        // this gives us more randomness
        m_generator = std::mt19937(seedNum);
    }
    void init(int min, int max){
        m_min = min;
        m_max = max;
        m_type = UNIFORMINT;
        m_generator = std::mt19937(10);// 10 is the fixed seed value
        m_unidist = std::uniform_int_distribution<>(min,max);
    }
    void getShuffle(vector<int> & array){
        // this function provides a list of all values between min and max
        // in a random order, this function guarantees the uniqueness
        // of every value in the list
        // the user program creates the vector param and passes here
        // here we populate the vector using m_min and m_max
        for (int i = m_min; i<=m_max; i++){
            array.push_back(i);
        }
        shuffle(array.begin(),array.end(),m_generator);
    }

    void getShuffle(int array[]){
        // this function provides a list of all values between min and max
        // in a random order, this function guarantees the uniqueness
        // of every value in the list
        // the param array must be of the size (m_max-m_min+1)
        // the user program creates the array and pass it here
        vector<int> temp;
        for (int i = m_min; i<=m_max; i++){
            temp.push_back(i);
        }
        std::shuffle(temp.begin(), temp.end(), m_generator);
        vector<int>::iterator it;
        int i = 0;
        for (it=temp.begin(); it != temp.end(); it++){
            array[i] = *it;
            i++;
        }
    }

    int getRandNum(){
        // this function returns integer numbers
        // the object must have been initialized to generate integers
        int result = 0;
        if(m_type == NORMAL){
            //returns a random number in a set with normal distribution
            //we limit random numbers by the min and max values
            result = m_min - 1;
            while(result < m_min || result > m_max)
                result = m_normdist(m_generator);
        }
        else if (m_type == UNIFORMINT){
            //this will generate a random number between min and max values
            result = m_unidist(m_generator);
        }
        return result;
    }

    double getRealRandNum(){
        // this function returns real numbers
        // the object must have been initialized to generate real numbers
        double result = m_uniReal(m_generator);
        // a trick to return numbers only with two deciaml points
        // for example if result is 15.0378, function returns 15.03
        // to round up we can use ceil function instead of floor
        result = std::floor(result*100.0)/100.0;
        return result;
    }

    string getRandString(int size){
        // the parameter size specifies the length of string we ask for
        // to use ASCII char the number range in constructor must be set to 97 - 122
        // and the Random type must be UNIFORMINT (it is default in constructor)
        string output = "";
        for (int i=0;i<size;i++){
            output = output + (char)getRandNum();
        }
        return output;
    }
    
    int getMin(){return m_min;}
    int getMax(){return m_max;}
    private:
    int m_min;
    int m_max;
    RANDOM m_type;
    std::random_device m_device;
    std::mt19937 m_generator;
    std::normal_distribution<> m_normdist;//normal distribution
    std::uniform_int_distribution<> m_unidist;//integer uniform distribution
    std::uniform_real_distribution<double> m_uniReal;//real uniform distribution

};


// simple hash function also given in the driver file for this project
unsigned int hashCode(const string str) {
   unsigned int val = 0 ;
   const unsigned int thirtyThree = 33 ;  // magic number from textbook
   for (unsigned int i = 0 ; i < str.length(); i++)
      val = val * thirtyThree + str[i] ;
   return val ;
}


// database of all the file names
string namesDB[6] = {"driver.cpp", "test.cpp", "test.h", "info.txt", "mydocument.docx", "tempsheet.xlsx"};

class Tester{
    public:
        bool testInsert();
        bool testInsertCol();
        bool testFind();
        bool testRemove();
        bool testRehashIns();
        bool testRehashDel();
    private:
        int indexHelper(prob_t hashType, int i, string name, int size);
        bool checkRemove(FileSys &sys, File f);
};


int main(){

    Tester myTest;
    if (myTest.testInsert()){
        cout << "pass" << endl;
    }else{
        cout << "fail" << endl;
    }
    if (myTest.testInsertCol()){
        cout << "pass" << endl;
    }else{
        cout << "fail" << endl;
    }
    if (myTest.testFind()){
        cout << "pass" << endl;
    }else{
        cout << "fail" << endl;
    }
    
    if (myTest.testRemove()){
        cout << "pass" << endl;
    }else{
        cout << "fail" << endl;
    }   
    if (myTest.testRehashIns()){
        cout << "pass" << endl;
    }else{
        cout << "fail" << endl;
    }  
    if (myTest.testRehashDel()){
        cout << "pass" << endl;
    }else{
        cout << "fail" << endl;
    }  
    return 0;
}


bool Tester::checkRemove(FileSys &sys, File f){
    // checks if file was correctly removed
    // first gets the file from the system to make sure file exists
    File checkExist = sys.getFile(f.m_name, f.m_diskBlock);
    if (checkExist.m_name !=  f.m_name || checkExist.m_diskBlock != f.m_diskBlock ){
        return false;
    }
    // then removes the file, and makes sure remove function worked
    if (!sys.remove(f)){
        return false;
    }
    // check gets file again to check if file was corrently removed from the system
    File checkExistAgain = sys.getFile(f.m_name, f.m_diskBlock);
    if (checkExistAgain.m_name != "" || checkExistAgain.m_diskBlock != 0){
        return false;
    }

    return true;
}



int Tester::indexHelper(prob_t hashType, int i, string name, int size){
    // helper to find the next probing index
    // uses the hashtype -> linear, double, or quad to get the next i'th position
    if (hashType == LINEAR){
        return (hashCode(name)+i) % size;
    }else if (hashType == DOUBLEHASH){
        return ((hashCode(name) % size) + i * (11-(hashCode(name) % 11))) % size;
    }else{
        return ((hashCode(name)%size)+(i*i)) % size;
    }
}




bool Tester::testInsert(){
    // testing to see if files are corrently inserted
    cout << "Testing insert for filesys: ";
    Random RndID(DISKMIN,DISKMAX);
    FileSys filesys(MINPRIME, hashCode, LINEAR);
    // testing for non-coliding 
    // creates 6 files all unique
    for(int i = 0; i < 6; i++){
        int ranBlock = RndID.getRandNum();
        File aFile = File(namesDB[i], ranBlock, true);
        // checks if inserted correctly and are placed in the corrently index
        if (!filesys.insert(aFile)){
            return false;
        }else{
            int place = hashCode(namesDB[i]) % filesys.m_currentCap;
            File* checkF = filesys.m_currentTable[place];
            if (checkF->m_name != namesDB[i] || checkF->m_diskBlock != ranBlock){
                return false;
            }
        }
        int checkI = i;
        // checks if the size of modified files are correct
        if (filesys.m_currentSize != ++checkI){
            return false;
        }
    }

    return true;
}

bool Tester::testInsertCol(){
    // checking insertions with collisions in the table
    cout << "Testing insert w/ Collisions for filesys: ";
    Random RndID(DISKMIN,DISKMAX);

    // linear system
    FileSys filesys(MINPRIME, hashCode, LINEAR);

    filesys.insert(File(namesDB[0], RndID.getRandNum(), true));
    // added 49 files
    for (int i = 1; i < 49; i++){
        int ranBlock = RndID.getRandNum();
        File insFile = File(namesDB[0], ranBlock, true);
        // checks if they are being inserted
        if (!filesys.insert(insFile)){
            return false;
        }
        // checks if the index value is correct based on the probing method
        int indexCheck = indexHelper(LINEAR, i, namesDB[0], filesys.m_currentCap);
        File* checkF = filesys.m_currentTable[indexCheck];
        // if invalid index then insertion did not work
        if (checkF == nullptr || checkF->m_name != namesDB[0] || checkF->m_diskBlock != ranBlock){
            return false;
        }
        
        int checkS = i;
        // checks size
        if (filesys.m_currentSize != ++checkS){
            return false;
        }
    }

    // quadratic system

    FileSys filesysQ(MINPRIME, hashCode, QUADRATIC);
    filesysQ.insert(File(namesDB[0], RndID.getRandNum(), true));
    // same testing method, but based on quadratic probing
    for (int i = 1; i < 49; i++){
        int ranBlock = RndID.getRandNum();
        File insFile = File(namesDB[0], ranBlock, true);
        if (!filesysQ.insert(insFile)){
            return false;
        }
        int indexCheck = indexHelper(QUADRATIC, i, namesDB[0], filesysQ.m_currentCap);
        File* checkF = filesysQ.m_currentTable[indexCheck];
        
        if (checkF == nullptr || checkF->m_name != namesDB[0] || checkF->m_diskBlock != ranBlock){
            return false;
        }
        
        int checkS = i;
        if (filesysQ.m_currentSize != ++checkS){
            return false;
        }
    }

    return true;
}



bool Tester::testFind(){
    // function to test if finding a file works
    cout << "Testing searching file (error/normal cases): ";
    Random RndID(DISKMIN,DISKMAX);
    FileSys filesys(MINPRIME, hashCode, LINEAR);

    File arr[6];
    // adds in 6 unique files
    for (int i = 0; i < 6; i++){
        int ranBlock = RndID.getRandNum();
        File insFile = File(namesDB[i], ranBlock, true);
        if(!filesys.insert(insFile)){
            return false;
        }else{
            arr[i]=insFile;
        }
    }

    // normal case, with no colliding keys
    // gets each file to make sure they are the same ones 
    for (int i = 0; i < 6; i++){
        File got = filesys.getFile(arr[i].m_name, arr[i].m_diskBlock);
        if (got.m_name != arr[i].m_name || got.m_diskBlock != arr[i].m_diskBlock){
            return false;
        }
    }

    // error case, no file
    // tries to get a invalid file
    File testFind = filesys.getFile("doesNotExist.txt", RndID.getRandNum());
    if (testFind.m_diskBlock != 0 || testFind.m_name != ""){
        return false;
    }
    
    // test with colliding
    // adds 20 new files 
    File arrColl[20];
    Random RndName(0,5);
    for (int i = 0; i < 20; i++){
        int ranBlock = RndID.getRandNum();
        File insFile = File(namesDB[RndName.getRandNum()], ranBlock, true);
        if(!filesys.insert(insFile)){
            return false;
        }else{
            arrColl[i]=insFile;
        }
    }
    // checks if all 20 can be found, and are correct
    for (int i = 0; i < 20; i++){
        File got = filesys.getFile(arrColl[i].m_name, arrColl[i].m_diskBlock);
        if (got.m_name != arrColl[i].m_name || got.m_diskBlock != arrColl[i].m_diskBlock){
            return false;
        }
    }
    
    return true;
}

bool Tester::testRemove(){
    
    cout << "Testing remove file (colliding & non-colliding): ";
    Random RndID(DISKMIN,DISKMAX);
    FileSys filesys(MINPRIME, hashCode, LINEAR);
    File fArr[6];
    
    // testing remove for non-colliding
    // adds 6 unique files
    for (int i = 0; i < 6; i++){
        int ranBlock = RndID.getRandNum();
        File insFile = File(namesDB[i], ranBlock, true);
        if(!filesys.insert(insFile)){
            return false;
        }else{
            fArr[i]=insFile;
        }
        int tempSize = i;
        if (filesys.m_currentSize != ++tempSize){
            return false;
        }

    }


    
    // removes the file from the 2th and 3rd positions in the array
    if (!checkRemove(filesys, fArr[2])){
        return false;
    }
    if (!checkRemove(filesys, fArr[3])){
        return false;
    }


    // testing remove with colliding
    File fArrColl[20];
    Random RndName(0,5);
    for (int i = 0; i < 20; i++){
        int ranBlock = RndID.getRandNum();
        File insFile = File(namesDB[RndName.getRandNum()], ranBlock, true);
        if(!filesys.insert(insFile)){
            return false;
        }else{
            fArrColl[i]=insFile;
        }
    }

    // removes 3 files, from the 5th, 10th, and 14th position in the array
    if (!checkRemove(filesys, fArrColl[5])){
        return false;
    }
    if (!checkRemove(filesys, fArrColl[10])){
        return false;
    }
    if (!checkRemove(filesys, fArrColl[14])){
        return false;
    }

     if (filesys.m_currNumDeleted != 5){
        return false;
    }


    return true;
}


bool Tester::testRehashIns(){
    cout << "Testing rehash for filesys insertion: ";
    
    Random RndID(DISKMIN,DISKMAX);
    FileSys filesys(MINPRIME, hashCode, LINEAR);
    Random RndName(0,5);
    // gets the number of elements need for there to be a rehash -> rehashI
    int rehashI = 0.5 * MINPRIME;
    
    // gets 25% of the size of the table
    int rehashPart = (MINPRIME*25)/100;
    // calculates how many times 25% will be needed to complete the rehashing process
    int insertionsToComplete = MINPRIME/rehashPart;
   
    // adds 100 files
    for(int i = 0; i < 100; i++){
        int ranBlock = RndID.getRandNum();
        File aFile = File(namesDB[RndName.getRandNum()], ranBlock, true);
        if (!filesys.insert(aFile)){
            return false;
        }
        // checks to see if rehashI position creates a old table and updates a new current table
        if (i == rehashI && filesys.m_oldTable == nullptr){
            return false;
            // also checking if the size of the new table is the smallest prime number 4 times the currentSize-deleted
        }else if (i == rehashI+1 && filesys.m_currentCap <= (4*filesys.m_currentSize)){
            return false;
        }
        // checks to see if rehash was completed 
        if (i == rehashI+insertionsToComplete+1 && filesys.m_oldTable != nullptr){
            return false;
        }
    }
    
    return true;
}
bool Tester::testRehashDel(){

    cout << "Testing rehash for filesys deletion: ";
    
    Random RndID(DISKMIN,DISKMAX);
    FileSys filesys(MINPRIME, hashCode, LINEAR);
    Random RndName(0,5);
   
    File fArr[30];
    
    // gets the I position for reashing to start, and how many iterations it will take to finish
    int rehashAfterI = (0.8*30)+1;
    int rehashPart = (MINPRIME*25)/100;
    int insertionsToComplete = MINPRIME/rehashPart;

    // adds 30 files
    for(int i = 0; i < 30; i++){
        int ranBlock = RndID.getRandNum();
        File aFile = File(namesDB[RndName.getRandNum()], ranBlock, true);
        if (!filesys.insert(aFile)){
            return false;
        }else{
            fArr[i] = aFile;
        }
    }
    
    // removes 28 and checks to see if rehashing took place
    for(int i = 0; i < 28; i++){
        if (!checkRemove(filesys, fArr[i])){
            return false;
        }
        // checks if rehash is correct when > 0.8
        if (i == rehashAfterI && filesys.m_oldTable == nullptr){
            return false;
        }
        // checks to see if rehash has been completed after the required iterations
        if (i == rehashAfterI+insertionsToComplete+1 && filesys.m_oldTable != nullptr){
            return false;
        }
    }
    
    return true;
}