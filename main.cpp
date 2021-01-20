#include "MurmurHash3.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string.h>
#include <math.h>
#include <set>
#include <vector>
//#include <random>
#include <iterator> //for std::ostream_iterator
#include <algorithm> //for std::copy
//#include <string>

using namespace std;

template <typename T>
void printNums(T num)
{
    int len = log10(pow(2, sizeof(T) * 8)) + 1;
    cout << setfill('0') << setw(len) << num;
    cout << endl;
}

template <typename T>
void printBits(T num, int count = -1)
{
    //int a;
    //cin >> a;

    int len = (count != -1) ? count : (sizeof(T) * 8);

    // cout << num << " " << count << " " << len << endl;

    for(int bit = 0; bit < len; bit++)
    {
        if (bit != 0 && !(bit % 8)) cout << " ";
        printf("%c", (num & 0x01) ? '1' : '0');
        //printf("%c", (num & 0x01) ? 'X' : '-');
        num = num >> 1;
    }

    if(count) cout << endl;
}

template <typename T>
void print(T num, int count = -1)
{
    //printBits<T>(num, count);
    printNums<T>(num);
}

void test()
{
    uint64_t hash_otpt[2];  // allocate 128 bits

    const int K = 13;
    const int m = 32e2; // 32e6;
    const int N = 1e6;

    const int MurLen = 128;

    int murRepits = m / MurLen;
    int murReminder = m % MurLen;

    const char *key = "Test";

    cout << "Murmur3 hash of \"" << key << "\":" << endl;
    for (int i = 0;  i < K; ++i)
    {
        cout << "i = " << i << ", murRepeats = " << murRepits << ", murReminder = " << murReminder << endl;
        for(int j = 0; j < murRepits; ++j)
        {
            int seed = i + K * j;
            //cout << "seed = " << seed << endl;
            MurmurHash3_x64_128(key, (uint64_t)strlen(key), seed, hash_otpt);
            print<uint64_t>(hash_otpt[0]);
            print<uint64_t>(hash_otpt[1]);
        }
        int seed = i + K * murRepits;
        MurmurHash3_x64_128(key, (uint64_t)strlen(key), seed, hash_otpt);
        //cout << "seed = " << seed << endl;
        if (murReminder < MurLen/2)
        {
            print<uint64_t>(hash_otpt[0], murReminder);
        }
        else
        {
            print<uint64_t>(hash_otpt[0]);
            print<uint64_t>(hash_otpt[1], murReminder - MurLen/2);
        }

        cout << endl;

        //MurmurHash3_x64_128(key, (uint64_t)strlen(key), i/*seed*/, hash_otpt);
        //cout << "seed = " << setw(2) << i << " -> ";
        //printBits<uint64_t>(hash_otpt[1]/*, ones, total*/);
        //cout << endl;

        //cout << "seed = " << i << " -> " << setfill('0') << setw(20) << hash_otpt[0] << /*setfill('0') << setw(20) << hash_otpt[1] <<*/ endl;
    }
}

int hashFunc(const char * key, int funcId, int m)
{
    uint64_t hash_otpt[2];
    MurmurHash3_x64_128(key, (uint64_t)strlen(key), funcId, hash_otpt);
    return hash_otpt[0] % m;
}

void split(const string& s, char c,
           vector<string>& v)
{
    int i = 0;
    int j = s.find(c);

    while (j >= 0)
    {
        v.push_back(s.substr(i, j-i));
        i = ++j;
        j = s.find(c, j);

        if (j < 0)
        {
            v.push_back(s.substr(i, s.length()));
        }
    }
}

void loadCSV(istream& in, vector<vector<string>*>& data)
{

    vector<string>* p = NULL;
    string tmp;

    while (!in.eof())
    {
        getline(in, tmp, '\n');                     // Grab the next line

        p = new vector<string>();
        split(tmp, ',', *p);                        // Use split from
        // Recipe 4.7
        data.push_back(p);

        cout << tmp << '\n';
        tmp.clear();
    }
}

void bitSet(int * arr, int index, bool value)
{
    int * b = &(arr[index/sizeof(int)]);
    if (value)
        (*b) |= (0x01 << (index%sizeof(int)));
    else
        (*b) &= ~(0x01 << (index%sizeof(int)));
}

bool bitGet(int * arr, int index)
{
    int * b = &(arr[index/sizeof(int)]);
    return !!((*b) & (0x01 << (index%sizeof(int))));
}


int main()
{
    srand(time(0));

    int K; // = 13;
    int m; // = 32e6;
    int N; // = 1e1; //1e6;

    //const int keyLen = 40;

    cout << "Enter K: " << endl;
    cin >> K;
    cout << "Enter m:" << endl;
    cin >> m;
    cout << "Enter structure file name:" << endl;
    string structFile;
    cin >> structFile;
    cout << "Enter test file name:" << endl;
    string testFile;
    cin >> testFile;

    ifstream inS(structFile);
    ifstream inT(testFile);


    if (!inS || !inT)
        return(EXIT_FAILURE);

    int * arr = new int[m / sizeof(int) + 1];
    for(int i = 0; i < m / sizeof(int) + 1; ++i)
        arr[i] = 0;

    vector<vector<string>*> dataS;
    cout << endl << "File " << structFile << ":" << endl;
    loadCSV(inS, dataS);

    set <int> s1;
    for (int lineId = 0; lineId < dataS.size(); ++lineId)
    {
        vector<string>& line = (*(dataS[lineId]));
        for (int wordId = 0; wordId < line.size(); ++wordId)
            for(int j = 0; j < K; ++j)
                //s1.insert(hashFunc(line[wordId].c_str(), j, m));
                bitSet(arr, hashFunc(line[wordId].c_str(), j, m), true);
    }

    for (vector<vector<string>*>::iterator p = dataS.begin();
            p != dataS.end(); ++p)
    {
        delete *p;                                  // Be sure to de-
    }

//    cout << endl << "Used cells:" << endl;
//    std::copy(
//        s1.begin(),
//        s1.end(),
//        std::ostream_iterator<int>(std::cout, " ")
//    );
//    cout << endl;

    vector<vector<string>*> dataT;
    cout << endl << "File " << testFile << ":" << endl;
    loadCSV(inT, dataT);
    cout << endl;

    //    char * testKey = "Test key";

    for (int lineId = 0; lineId < dataT.size(); ++lineId)
    {
        vector<string>& line = (*(dataT[lineId]));
        for (int wordId = 0; wordId < line.size(); ++wordId)
        {
            bool failed = false;
            for(int j = 0; j < K; ++j)
            {
                int h = hashFunc(line[wordId].c_str(), j, m);
                bool found = bitGet(arr, hashFunc(line[wordId].c_str(), j, m));//(s1.find(h)!= s1.end());
                if (!found)
                {
                    failed = true;
                    break;
                }
            }
            cout << setw(20) << line[wordId] << (failed ? " not " : " ") << "exists" << endl;
        }
    }

    for (vector<vector<string>*>::iterator p = dataT.begin();
            p != dataT.end(); ++p)
    {
        delete *p;                                  // Be sure to de-
    }                                              // reference p!

    delete[] arr;

//    char a;
//    cin >> a;

    return 0;
}
