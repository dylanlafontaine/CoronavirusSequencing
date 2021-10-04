// PA2-CMake.h : Include file for standard system include files,
// or project specific include files.

#pragma once

//#define _CRTDBG_MAP_ALLOC
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//#include <crtdbg.h>
#include <time.h>
#include <Windows.h>
#include <limits.h>

typedef struct Node
{
    int leafID;              //unique leafID for a node
    int id;
    int stringID;
    int staticStringID;
    int string_depth;        //Depth of the string
    struct Node* parent;     //Parent pointer
    struct Node* suffixLink; //Suffix link pointer
    int start;               //Start index of edgelabel
    int end;                 //End index of edgelabel
    struct Node** child;     //Child array of length sigma + 1
} Node;

typedef struct dp_cell
{
    int Sscore; //field for Substitution (S) score
    int Dscore; //field for Deletion (D) score
    int Iscore; //field for Insertion (I) score
    int score;
    int retraceCase;
    int numMatchesS;
    int numMatchesD;
    int numMatchesI;
} DP_cell;

typedef struct tuple
{
    int i;
    int j;
} Tuple;

Node* CreateNewNode(int leafID, int string_depth, Node* parent, int start, int end, int leaf, char* sigma, int sigmaLength, int stringIndex);
int inAlphabet(char* sigma, char c, int sigmaLength);
int charToIndex(char c, char* sigma, int sigmaLength);
int DetermineString(int i, int inputLength);
Node* FindPath(Node* u, char* string, int i, char* sigma, int sequenceLength, int sigmaLength);
int myStringcmp(int betaStart, int betaEnd, int curStart, int curEnd, char* string);
Node* NodeHop(Node* u, char* string, int i, int betaStart, int betaEnd, int betaNull, char* sigma, int sigmaLength);
int PrintBWT(Node* currNode, char* string, char* sigma, int sequenceLength, int sigmaLength, int fileOutput, FILE* file1, FILE* file2);
int DFSNodePrint(Node* currNode, char* string, char* sigma, int sequenceLength, int sigmaLength, int fileOutput, FILE* file1);
int BuildSTNaive(char* string, char* sigma, int sequenceLength, int sigmaLength, FILE* file1, FILE* file2);
Node* BuildST(char* string, char* sigma, int sequenceLength, int sigmaLength, FILE* file1, Node* root);
int readFASTA(char* fileName, char** name, char** sequence, int stringIndex);
int readAlphabet(char* fileName, char** sigma);
int FreeTree(Node* node, char* sigma, int sigmaLength);
int DetermineStats(Node* currNode, char* sigma, int sequenceLength, int sigmaLength, int* numInternalNodes, int* averageStringDepth, int* deepestString, Node** deepestNode);
int Maximum(int x, int y);
int PrintChildren(Node* currNode, char* sigma, int sigmaLength);
int DFSRandomChild(Node* currNode, char* string, char* sigma, int sequenceLength, int sigmaLength, int fileOutput, FILE* file1, FILE* file2);
void ValidateFileName(char* name);
int LongestRepeat(Node* currNode);
int LengthAllInputStrings(char** argv, char** name, char** sequence);
int CatInputStrings(char** argv, char** name, char** sequence, int* sequenceLength, char** stringsCat);
Node* BuildGST(char** argv, char* sigma, int sigmaLength, FILE* file1, int* sequenceLength, char** name, char** sequence, char** stringsCat);
Node* BuildGST2Inputs(char* s1, char* s2, char* sigma, int sigmaLength, FILE* file1, int* sequenceLength, char** name, char** sequence, char** stringsCat);
int DFSColorNodesSUS(Node* node, int sigmaLength, Node** shortestMatches, char* sigma, char* stringsCat);
int DFSColorNodesLCS(Node* node, int sigmaLength, Node** lcsChildren, char* sigma);
void ComputeLCSCoords(Node** lcsChildren, int** lcsStartCoords, int** lcsEndCoords, char* stringsCat, int i, int j);
void retrace(DP_cell** table, int i, int j, char* s1, char* s2, int maxVal, int* matches, int offseti, int offsetj);
void FreeDPTable(DP_cell** table, int m);
Node* ComputeSimilarityMatrix(char** argv, char* sigma, int sigmaLength, FILE* file1, int* sequenceLength, char** name, char** sequence, int numInputs, DP_cell* currentRow, DP_cell* previousRow, int** similarityMatrix);
int ComputeNumInputs(char** argv);
DP_cell** InitDPTable(DP_cell** currentRow, DP_cell** previousRow, int m, int n, int h, int g);
int DetermineRetrace(int a, int b, int c);
int Maximum3Vars(DP_cell** currentRow, DP_cell** previousRow, int x, int y, int z, int i, int j);
int maximum(int x, int y, int* totalMatches, DP_cell currentCell);
int s(char a, char b, const int match, const int mismatch);
void ReverseStrings(Node** lcsChildren, int m, int n, int siStart, int siEnd, int sjStart, int sjEnd, char** reversedInputStrings, char* stringsCat);
void MainRecurrenceGlobal(DP_cell** currentRow, DP_cell** previousRow, int m, int n, Node** lcsChildren, const int match, const int mismatch, const int h, const int g, char** string, int* matches, int offseti, int offsetj, int s1Index, int s2Index);
int ComputeGlobalAlignment(DP_cell** currentRow, DP_cell** previousRow, int m, int n, Node** lcsChildren, char** string, int offseti, int offsetj, int s1Index, int s2Index);
void InitSimilarityMatrix(int*** similarityMatrix, int numInputs);
void CopySimilarityMatrixDiagonals(int** similarityMatrix, int numInputs);
void OutputSimilarityMatrix(int** similarityMatrix, int numInputs);
void InitShortestMatches(Node*** shortestMatches, int numInputs);
void OutputFingerprints(Node** shortestMatches, char* stringsCat, int numInputs);
void FingerprintStrings(Node* root, char** argv, char* sigma, int sigmaLength, FILE* file1, int* sequenceLength, char** name, char** sequence, char** stringsCat, int numInputs, Node** shortestMatches);
