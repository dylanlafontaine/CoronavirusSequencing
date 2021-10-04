
#include "PA2-CMake.h"

#pragma warning (disable : 4996)

int globalLeafID = 0, iterator = 0, globalID = 0, memoryAllocated = 0, globalStringID = 0;
char** inputStrings;
Tuple** inputStringRanges;

struct Node* curLeaf;

Node* CreateNewNode(int leafID, int string_depth, Node* parent, int start, int end, int leaf, char* sigma, int sigmaLength, int stringIndex)
{
	Node* newNode = (Node*)malloc(sizeof(Node));
	if (newNode == NULL) {
		printf("Error: Could not create new node\n");
		return NULL;
	}
	if (leaf)
	{
		newNode->leafID = globalLeafID;
		globalLeafID++;
	}
	else
	{
		newNode->leafID = -1;
	}

	newNode->id = globalID;
	newNode->stringID = stringIndex;
	newNode->staticStringID = stringIndex;
	globalID++;
	newNode->string_depth = string_depth;
	newNode->parent = parent;
	newNode->start = start;
	newNode->end = end;
	newNode->suffixLink = NULL;

	newNode->child = (Node*)malloc((sigmaLength) * sizeof(Node*));
	for (int i = 0; i < sigmaLength; i++)
	{
		newNode->child[i] = NULL;
	}

	//Amount of memory allocated per CreateNewNode function call
	memoryAllocated += (int)sizeof(Node) + (int)((sigmaLength) * sizeof(Node*));

	return newNode;
}

int inAlphabet(char* sigma, char c, int sigmaLength)
{
	if (sigmaLength == 5 && sigma[1] == 'A' && sigma[2] == 'C' && sigma[3] == 'G' && sigma[4] == 'T') {
		if (c == 'A' || c == 'C' || c == 'G' || c == 'T' || c == '$') {
			return 1;
		}
		else {
			return 0;
		}
	}
	else {
		if (c != '\0' && (c == '$' || sigma[c - 64] == c))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
}

int charToIndex(char c, char* sigma, int sigmaLength)
{
	if (sigmaLength == 5 && sigma[1] == 'A' && sigma[2] == 'C' && sigma[3] == 'G' && sigma[4] == 'T') {
		//Then we are using the DNA Alphabet
		switch (c)
		{
		case 'A':
			return 1;
			break;
		case 'C':
			return 2;
			break;
		case 'G':
			return 3;
			break;
		case 'T':
			return 4;
			break;
		}
	}
	if (c == '$')
	{
		return 0;
	}
	else
	{
		return (c - 64);
	}
}

int DetermineString(int i, int inputLength) {
	for (int j = 0; j < inputLength; j++) {
		if (i >= inputStringRanges[j]->i && i <= inputStringRanges[j]->j) {
			return j;
		}
	}
}

Node* FindPath(Node* u, char* string, int i, char* sigma, int sequenceLength, int sigmaLength)
{
	//A leaf already exists
	//if (i > sequenceLength - 1) {
	//    i = sequenceLength - 1;
	//}
	Node* v = u;
	//char* string = malloc(sequenceLength + 1);
	int start, end;
	//strcpy(x, string); //copies string into x
	if (inAlphabet(sigma, string[i], sigmaLength))
	{ //x[i] is in the alphabet sigma
		if (v->child[charToIndex(string[i], sigma, sigmaLength)] == NULL)
		{ //No branch that starts with string[i]
			//Creates new leaf for s under u
			Node* newLeaf = CreateNewNode(i, sequenceLength, v, i, sequenceLength - 1, 1, sigma, sigmaLength, DetermineString(i, globalStringID));
			newLeaf->end = inputStringRanges[newLeaf->stringID]->j;
			newLeaf->string_depth = inputStringRanges[newLeaf->stringID]->j - i + 1 + newLeaf->parent->string_depth;
			v->child[charToIndex(string[i], sigma, sigmaLength)] = newLeaf;
			curLeaf = newLeaf;
			//free(x);
			return newLeaf;
		}
		else //There exists a branch that starts with string[i]
		{
			start = v->child[charToIndex(string[i], sigma, sigmaLength)]->start;
			end = v->child[charToIndex(string[i], sigma, sigmaLength)]->end;
			int tempi = i;
			for (start; start <= end; start++)
			{                                      //compare characters of the edge label against string[i..]
				if (string[start] != string[tempi]) //mismatch
				{
					Node* oldLeaf = v->child[charToIndex(string[i], sigma, sigmaLength)];
					Node* newInternalNode = CreateNewNode(tempi, 0, v, v->child[charToIndex(string[i], sigma, sigmaLength)]->start, start - 1, 0, sigma, sigmaLength, oldLeaf->staticStringID);
					newInternalNode->string_depth = v->string_depth + newInternalNode->end - newInternalNode->start + 1;
					Node* newLeaf = CreateNewNode(tempi, 0, newInternalNode, tempi, sequenceLength - 1, 1, sigma, sigmaLength, DetermineString(i, globalStringID));
					newLeaf->parent = newInternalNode;
					newLeaf->end = inputStringRanges[newLeaf->stringID]->j;
					newLeaf->string_depth = newLeaf->parent->string_depth + ((inputStringRanges[newLeaf->stringID]->j - inputStringRanges[newLeaf->stringID]->i) - tempi + 1);
					oldLeaf->parent = newInternalNode;
					oldLeaf->start = start;
					u->child[charToIndex(string[i], sigma, sigmaLength)] = newInternalNode;
					curLeaf = newLeaf;

					if (inAlphabet(sigma, string[start], sigmaLength))
					{ //string[start] is in the alphabet sigma at index k (creates new branch for oldLeaf)
						newInternalNode->child[charToIndex(string[start], sigma, sigmaLength)] = oldLeaf;
					}

					if (inAlphabet(sigma, string[tempi], sigmaLength))
					{
						newInternalNode->child[charToIndex(string[tempi], sigma, sigmaLength)] = newLeaf;
					}
					//free(x);
					return newInternalNode;
				}
				tempi++;
			}
			//No mismatch occurred
			if (u->child[charToIndex(string[i], sigma, sigmaLength)]->leafID != -1 && u->child[charToIndex(string[i], sigma, sigmaLength)]->leafID != 0) { //Leaf already exists
				curLeaf = u->child[charToIndex(string[i], sigma, sigmaLength)];
				if (curLeaf->stringID != globalID && curLeaf->stringID != -1) {
				    curLeaf->stringID = -1; //-1 is the "color" for mixed nodes
				}
				//free(x);
				return curLeaf;
			}
			FindPath(v->child[charToIndex(string[i], sigma, sigmaLength)], string, tempi, sigma, sequenceLength, sigmaLength);
		}
	}
	//free(x);
	return u;
}

int myStringcmp(int betaStart, int betaEnd, int curStart, int curEnd, char* string) {
	if (betaEnd - betaStart != curEnd - curStart) {
		return 0;
	}
	char* betaString = (char*)malloc((betaEnd - betaStart) + 2);
	char* nodeString = (char*)malloc((curEnd - curStart) + 2);
	memcpy(betaString, &string[betaStart], (betaEnd - betaStart) + 1);
	memcpy(nodeString, &string[betaStart], (curEnd - curStart) + 1);
	betaString[(betaEnd - betaStart) + 1] = '\0';
	nodeString[(curEnd - curStart) + 1] = '\0';

	if (!memcmp(betaString, nodeString, betaEnd - betaStart + 1)) {
		free(betaString);
		free(nodeString);
		return 1; //strings match
	}
	else {
		free(betaString);
		free(nodeString);
		return 0;
	}
}

Node* NodeHop(Node* u, char* string, int i, int betaStart, int betaEnd, int betaNull, char* sigma, int sigmaLength)
{
	if (betaNull)
	{
		return u;
	}

	if (inAlphabet(sigma, string[betaStart], sigmaLength))
	{ //Finds the proper child of u
		Node* curNode = u->child[charToIndex(string[betaStart], sigma, sigmaLength)];
		if (curNode == NULL) {
			return u;
		}

		if (curNode->end - curNode->start > betaEnd - betaStart)
		{
			//There does not exist a node with string beta
			Node* newInternalNode;
			newInternalNode = CreateNewNode(-1, 0, u, curNode->start, (curNode->start + (betaEnd - betaStart)), 0, sigma, sigmaLength, curNode->staticStringID); //Creates new internal node
			newInternalNode->string_depth = u->string_depth + (newInternalNode->end - newInternalNode->start) + 1;
			u->child[charToIndex(string[betaStart], sigma, sigmaLength)] = newInternalNode;
			curNode->start = newInternalNode->end + 1;
			curNode->parent = newInternalNode;

			if (inAlphabet(sigma, string[curNode->start], sigmaLength))
			{
				newInternalNode->child[charToIndex(string[curNode->start], sigma, sigmaLength)] = curNode;
			}

			return newInternalNode;
		}
		else if (myStringcmp(betaStart, betaEnd, curNode->start, curNode->end, string))
		{
			u = curNode; //There already exists a node with string beta
			return u;
		}
		else
		{
			u = NodeHop(curNode, string, i, betaStart + (curNode->end - curNode->start + 1), betaEnd, betaNull, sigma, sigmaLength);
			return u;
		}
	}
	return u;
}

int PrintBWT(Node* currNode, char* string, char* sigma, int sequenceLength, int sigmaLength, int fileOutput, FILE* file1, FILE* file2)
{
	//Prints the BWT of the tree
	if (currNode == NULL) {
		return 0;
	}

	int start, end;
	if (currNode != NULL && currNode->leafID != 0)
	{
		start = currNode->start;
		end = currNode->end;
		if (currNode->leafID != -1 && currNode->leafID != 1)
		{
			if (!fileOutput) {
				//printf("%c\n", string[currNode->leafID - 2]);
			}
			else {
				fprintf(file2, "%c\n", string[currNode->leafID - 2]);
			}
		}
		if (currNode->leafID == 1)
		{
			if (!fileOutput) {
				//printf("%c\n", string[sequenceLength - 1]);
			}
			else {
				fprintf(file2, "%c\n", string[sequenceLength - 1]);
			}
		}
	}

	for (int i = 0; i < sigmaLength; i++)
	{
		if (currNode != NULL)
		{
			PrintBWT(currNode->child[i], string, sigma, sequenceLength, sigmaLength, fileOutput, file1, file2);
		}
	}
}

int DFSNodePrint(Node* currNode, char* string, char* sigma, int sequenceLength, int sigmaLength, int fileOutput, FILE* file1)
{
	//Prints the Node information of the tree
	if (currNode == NULL) {
		return 0;
	}

	if (currNode->leafID == -1) {
		if (!fileOutput) {
			//printf("%d ", currNode->string_depth);
			iterator++;
			if (iterator == 10) {
				//printf("\n");
				iterator = 0;
			}
		}
		else {
			fprintf(file1, "%d ", currNode->string_depth);
			iterator++;
			if (iterator == 10) {
				fprintf(file1, "%c", '\n');
				iterator = 0;
			}
		}
	}

	for (int i = 0; i < sigmaLength; i++)
	{
		if (currNode != NULL)
		{
			DFSNodePrint(currNode->child[i], string, sigma, sequenceLength, sigmaLength, fileOutput, file1);
		}
	}
}

int BuildSTNaive(char* string, char* sigma, int sequenceLength, int sigmaLength, FILE* file1, FILE* file2)
{
	Node* root = CreateNewNode(0, 0, NULL, 0, 0, 1, sigma, sigmaLength, globalStringID);
	root->leafID = 0;
	root->id = 0;
	for (int i = 0; i < sequenceLength + 1; i++)
	{
		FindPath(root, &string[0], i, sigma, sequenceLength, sigmaLength);
	}
	PrintBWT(root, string, sigma, sequenceLength, sigmaLength, 0, file1, file2);
	DFSNodePrint(root, string, sigma, sequenceLength, sigmaLength, 0, file1);
	FreeTree(root, sigma, sigmaLength);
}

//McCreight's Algorithm
Node* BuildST(char* string, char* sigma, int sequenceLength, int sigmaLength, FILE* file1, Node* root)
{
	LARGE_INTEGER start, end, elapsedMicroseconds;
	LARGE_INTEGER frequency;

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&start);

	Node* u = NULL;
	Node* v = NULL;
	Node* uprime = NULL;
	Node* vprime = NULL;
	int betaStart;
	int betaEnd;
	curLeaf = root;

	for (int i = 0; i < sequenceLength; i++)
	{
		u = curLeaf->parent;
		if (u->suffixLink != NULL && u->leafID != 0)
		{ //Case IA
			v = u->suffixLink;
			FindPath(v, string, i + v->string_depth, sigma, sequenceLength, sigmaLength);
		}
		else if (u->suffixLink != NULL && u->leafID == 0)
		{ //Case IB
			v = u->suffixLink;
			FindPath(v, string, i, sigma, sequenceLength, sigmaLength);
		}
		else if (u->suffixLink == NULL && u->parent->leafID != 0)
		{
			betaStart = u->start;
			betaEnd = u->end;
			uprime = u->parent;
			vprime = uprime->suffixLink;
			if (i == 59675) {
				printf("");
			}
			v = NodeHop(vprime, string, i, betaStart, betaEnd, 0, sigma, sigmaLength);
			u->suffixLink = v;
			if (i + v->string_depth > sequenceLength) {
				printf("");
			}
			FindPath(v, string, i + v->string_depth, sigma, sequenceLength, sigmaLength);
		}
		else if (u->suffixLink == NULL && u->parent->leafID == 0)
		{
			int betaNull = 0;
			if (u->start - u->end != 0)
			{
				betaStart = u->start + 1;
			}
			else
			{
				betaStart = u->start;
				betaNull = 1;
			}
			betaEnd = u->end;
			uprime = u->parent;
			v = NodeHop(uprime, string, i, betaStart, betaEnd, betaNull, sigma, sigmaLength);
			u->suffixLink = v;

			if (!betaNull)
			{
				FindPath(v, string, i + (betaEnd - betaStart + 1), sigma, sequenceLength, sigmaLength);
			}
			else
			{
				FindPath(v, string, i, sigma, sequenceLength, sigmaLength);
			}
		}
	}
	//FindPath(root, string, sequenceLength - 1, sigma, sequenceLength, sigmaLength);

	QueryPerformanceCounter(&end);
	elapsedMicroseconds.QuadPart = end.QuadPart - start.QuadPart;
	elapsedMicroseconds.QuadPart *= 1000000;
	elapsedMicroseconds.QuadPart /= frequency.QuadPart;

	printf("Time for BuildST: %llu microseconds\n", elapsedMicroseconds.QuadPart);
	//PrintBWT(root, string, sigma, sequenceLength, sigmaLength);
	//DFSNodePrint(root, string, sigma, sequenceLength, sigmaLength, 0, file1);
	return root;
}

int readFASTA(char* fileName, char** name, char** sequence, int stringIndex)
{
	FILE* file;
	int read;
	int i = 0;
	int j = 0;
	long fileLength, nameLength;

	file = fopen(fileName, "r");

	if (file == NULL)
	{
		printf("Could not Open file\n");
		return -1;
	}

	//Gets the size of file
	fseek(file, 0, SEEK_END);
	fileLength = ftell(file);
	rewind(file);

	read = fgetc(file);
	if (read != '>')
	{
		printf("Not FASTA Format\n");
		return -1;
	}

	while ((read = fgetc(file)) != EOF)
	{
		if (read == '\n')
		{
			nameLength = ftell(file);
			fseek(file, 1, SEEK_SET);
			break;
		}
	}

	*name = (char*)malloc(nameLength + 25);

	if (name == NULL) {
		return -1;
	}
	fseek(file, 1, SEEK_SET);

	while ((read = fgetc(file)) != '\n')
	{
		(*name)[i] = (char)read;
		i++;
	}

	(*name)[i] = '\0';
	i = 0;
	fseek(file, nameLength, SEEK_SET);
	*sequence = (char*)malloc(fileLength - nameLength + 2);

	while ((read = fgetc(file)) != EOF)
	{
		if (read != '\n')
		{
			if (read < 65 || read > 90) {
				printf("Input sequence must be upper case.");
				return -1;
			}
			(*sequence)[i] = (char)read;
			i++;
		}
	}
	(*sequence)[i] = '$';
	(*sequence)[++i] = '\0';

	fclose(file);

	inputStrings[stringIndex] = *sequence;

	return 1;
}

int readAlphabet(char* fileName, char** sigma) {
	FILE* file;
	int read, fileLength, i = 1;

	if ((file = fopen(fileName, "r")) == NULL) {
		return -1;
	}

	fseek(file, 0, SEEK_END);
	fileLength = ftell(file);
	rewind(file);
	*sigma = (char*)malloc(fileLength + 3);
	(*sigma)[0] = '$';

	while ((read = fgetc(file)) != EOF) {
		if (read != ' ') {
			(*sigma)[i] = (char)read;
			i++;
		}
	}

	(*sigma)[i] = '\0';

	fclose(file);
	return 1;
}

int FreeTree(Node* node, char* sigma, int sigmaLength) {
	if (node == NULL) {
		return 0;
	}
	for (int i = 0; i < sigmaLength; i++)
	{
		if (node != NULL)
		{
			FreeTree(node->child[i], sigma, sigmaLength);
		}
	}
	node->parent = NULL;
	node->suffixLink = NULL;
	free(node->child);
	free(node);
}

int DetermineStats(Node* currNode, char* sigma, int sequenceLength, int sigmaLength, int* numInternalNodes, int* averageStringDepth, int* deepestString, Node** deepestNode)
{
	//Determines the stats for the created suffix tree
	if (currNode == NULL) {
		return 0;
	}

	if (currNode->leafID == -1) {
		(*numInternalNodes) += 1;
		(*averageStringDepth) += currNode->string_depth;
		int tempDeepestString = (*deepestString);
		(*deepestString) = Maximum(*deepestString, currNode->string_depth); //This line and the line below it determine what the 
		if (tempDeepestString != *deepestString) {                          //longest exact matching repeat is
			(*deepestNode) = currNode;
		}
	}

	for (int i = 0; i < sigmaLength; i++)
	{
		if (currNode != NULL)
		{
			DetermineStats(currNode->child[i], sigma, sequenceLength, sigmaLength, numInternalNodes, averageStringDepth, deepestString, deepestNode);
		}
	}
}

int Maximum(int x, int y) {
	if (x >= y) {
		return x;
	}
	else {
		return y;
	}
}

int PrintChildren(Node* currNode, char* sigma, int sigmaLength) {
	for (int i = 0; i < sigmaLength; i++) {
		if (currNode != NULL && currNode->child[i] != NULL) {
			//printf("%d ", currNode->child[i]->id);
		}
	}
	//printf("\n\n");
}

int DFSRandomChild(Node* currNode, char* string, char* sigma, int sequenceLength, int sigmaLength, int fileOutput, FILE* file1, FILE* file2)
{
	//Prints the Node information of the tree
	if (currNode == NULL) {
		return 0;
	}

	for (int i = 0; i < sigmaLength; i++)
	{
		DFSRandomChild(currNode->child[i], string, sigma, sequenceLength, sigmaLength, fileOutput, file1, file2);
	}

	if (currNode->id == 3) {
		PrintChildren(currNode, sigma, sigmaLength);
		DFSNodePrint(currNode, string, sigma, sequenceLength, sigmaLength, fileOutput, file1);
		fprintf(file1, "%c", '\n');
		PrintBWT(currNode, string, sigma, sequenceLength, sigmaLength, fileOutput, file1, file2);
		fprintf(file2, "%s", "\n\n");
	}
	if (currNode->id == 4) {
		PrintChildren(currNode, sigma, sigmaLength);
		DFSNodePrint(currNode, string, sigma, sequenceLength, sigmaLength, fileOutput, file1);
		fprintf(file1, "%c", '\n');
		PrintBWT(currNode, string, sigma, sequenceLength, sigmaLength, fileOutput, file1, file2);
		fprintf(file2, "%s", "\n\n");
	}
	if (currNode->id == 7) {
		PrintChildren(currNode, sigma, sigmaLength);
		DFSNodePrint(currNode, string, sigma, sequenceLength, sigmaLength, fileOutput, file1);
		fprintf(file1, "%c", '\n');
		PrintBWT(currNode, string, sigma, sequenceLength, sigmaLength, fileOutput, file1, file2);
		fprintf(file2, "%s", "\n\n");
	}
}

void ValidateFileName(char* name) {
	for (int i = 0; i < strlen(name); i++) {
		if (name[i] == '<' || name[i] == '>' || name[i] == ':' || name[i] == '\"' || name[i] == '/' || name[i] == '\\' || name[i] == '|' || name[i] == '?' || name[i] == '*' || name[i] == ',' || name[i] == '.' || name[i] == ' ') {
			name[i] = '_';
		}
	}
	if (strlen(name) > 128) {
		name[127] = '\0';
	}
	memcpy(&name[strlen(name)], ".txt\0", 5);
}

int LongestRepeat(Node* currNode) {
	if (currNode->id == 0) {
		return 0;
	}
	//printf("\n%d", currNode->end);
	LongestRepeat(currNode->parent);
}

int LengthAllInputStrings(char** argv, char** name, char** sequence) {
	int i = 2, lengthAllInputs = 0;
	while (argv[i] != NULL && strcmp(argv[i], "NULL")) {
		if (!readFASTA(argv[i], name, sequence, i - 2)) {
			printf("Error: Couldn't Read FASTA File\n");
		}
		lengthAllInputs += strlen(*sequence);
		i++;
	}
	return lengthAllInputs;
}

int CatInputStrings(char** argv, char** name, char** sequence, int* sequenceLength, char** stringsCat) {
	int i = 2;
	int j = 0;
	while (argv[i] != NULL && strcmp(argv[i], "NULL")) {
		if (!readFASTA(argv[i], name, sequence, i - 2)) {
			printf("Error: Couldn't Read FASTA File\n");
			return 1;
		}
		*sequenceLength = strlen(*sequence); //Length of the sequence s[i]
		inputStringRanges[globalStringID] = (Tuple*)malloc(sizeof(Tuple*));
		inputStringRanges[globalStringID]->i = j;
		inputStringRanges[globalStringID]->j = j + *sequenceLength - 1;
		strcpy(&(*stringsCat)[j], *sequence);
		i++;
		j += *sequenceLength;
		globalStringID++;
	}
}

//Builds a generalized suffix tree of all input strings provided by the user
Node* BuildGST(char** argv, char* sigma, int sigmaLength, FILE* file1, int* sequenceLength, char** name, char** sequence, char **stringsCat) {
	int lengthAllInputs;
	globalStringID = 0;
	//Initializes the root
	Node* root = CreateNewNode(0, 0, NULL, 0, 0, 1, sigma, sigmaLength, globalStringID);
	root->leafID = 0;
	root->id = 0;
	root->suffixLink = root;
	root->parent = root;
	//Reads in the input file names in the order they were given from the command line
	lengthAllInputs = LengthAllInputStrings(argv, name, sequence);
	*stringsCat = (char*)malloc(lengthAllInputs + 1);
	CatInputStrings(argv, name, sequence, sequenceLength, stringsCat);

	root = BuildST(*stringsCat, sigma, lengthAllInputs, sigmaLength, file1, root);
	return root;
}

//Builds a generalized suffix tree for only 2 input strings
Node* BuildGST2Inputs(char* s1, char* s2, char* sigma, int sigmaLength, FILE* file1, int* sequenceLength, char** name, char** sequence, char** stringsCat) {
	int lengthAllInputs;
	globalStringID = 0;
	//Initializes the root
	Node* root = CreateNewNode(0, 0, NULL, 0, 0, 1, sigma, sigmaLength, globalStringID);
	root->leafID = 0;
	root->id = 0;
	root->suffixLink = root;
	root->parent = root;

	char** localStrings = malloc(5 * sizeof(char*));
	localStrings[2] = s1;
	localStrings[3] = s2;
	localStrings[4] = NULL;

	lengthAllInputs = LengthAllInputStrings(localStrings, name, sequence);
	*stringsCat = (char*)malloc(lengthAllInputs + 1);
	CatInputStrings(localStrings, name, sequence, sequenceLength, stringsCat);

	root = BuildST(*stringsCat, sigma, lengthAllInputs, sigmaLength, file1, root);

	return root;
}

//Colors the nodes in the suffix tree and determines the shortest unique substring
int DFSColorNodesSUS(Node* node, int sigmaLength, Node** shortestMatches, char *sigma, char *stringsCat) {
	if (node == NULL || (node->leafID != -1 && node->leafID != 0)) {
		return 0;
	}

	for (int i = 0; i < sigmaLength; i++)
	{
		DFSColorNodesSUS(node->child[charToIndex(sigma[i], sigma, sigmaLength)], sigmaLength, shortestMatches, sigma, stringsCat);
	}
	//Assign color to node by looking at all children
	Node* previousChild = NULL;
	int mixed = 0; //flag to detect if at least one of the node's children is mixed
	for (int i = 0; i < sigmaLength; i++)
	{
		if (node->child[charToIndex(sigma[i], sigma, sigmaLength)] != NULL) {
			if (previousChild != NULL) {
				if ((node->child[charToIndex(sigma[i], sigma, sigmaLength)]->stringID != previousChild->stringID && node->stringID != -1) || previousChild->stringID == -1) {
					mixed++;
				}
			}
			previousChild = node->child[charToIndex(sigma[i], sigma, sigmaLength)];
		}
	}
	if (mixed > 0) {
		node->stringID = -1; //The node's string color is set to mixed
		for (int i = 0; i < sigmaLength; i++) {
			if (node->child[charToIndex(sigma[i], sigma, sigmaLength)] != NULL && node->child[charToIndex(sigma[i], sigma, sigmaLength)]->stringID != -1) { //As long as a node's child isn't mixed we can update the string's shortest match
				if (shortestMatches[node->child[charToIndex(sigma[i], sigma, sigmaLength)]->stringID] == NULL) {
					shortestMatches[node->child[charToIndex(sigma[i], sigma, sigmaLength)]->stringID] = node->child[charToIndex(sigma[i], sigma, sigmaLength)];
				}
				else if (shortestMatches[node->child[charToIndex(sigma[i], sigma, sigmaLength)]->stringID]->parent->string_depth > node->string_depth && stringsCat[node->child[charToIndex(sigma[i], sigma, sigmaLength)]->start] != '$') {
					shortestMatches[node->child[charToIndex(sigma[i], sigma, sigmaLength)]->stringID] = node->child[charToIndex(sigma[i], sigma, sigmaLength)];
				}
			}
		}
	}
	else {
		node->stringID = previousChild->stringID;
	}
}

int DFSColorNodesLCS(Node* node, int sigmaLength, Node** lcsChildren, char *sigma) {
	if (node == NULL || (node->leafID != -1 && node->leafID != 0)) {
		return 0;
	}

	for (int i = 0; i < sigmaLength; i++)
	{
		DFSColorNodesLCS(node->child[i], sigmaLength, lcsChildren, sigma);
	}
	//Assign color to node by looking at all children
	Node* previousChild = NULL;
	int mixed = 0; //flag to detect if at least one of the node's children is mixed
	for (int i = 0; i < sigmaLength; i++)
	{
		if (node->child[charToIndex(sigma[i], sigma, sigmaLength)] != NULL) {
			if (previousChild != NULL) {
				if ((node->child[charToIndex(sigma[i], sigma, sigmaLength)]->stringID != previousChild->stringID && node->stringID != -1) || previousChild->stringID == -1) {
					mixed++;
				}
			}
			previousChild = node->child[charToIndex(sigma[i], sigma, sigmaLength)];
		}
	}
	if (mixed > 0) {
		node->stringID = -1; //The node's string color is set to mixed
		for (int i = 0; i < sigmaLength; i++) {
			if (node->child[charToIndex(sigma[i], sigma, sigmaLength)] != NULL && node->child[i]->stringID != -1) { //As long as a node's child isn't mixed we can update the string's shortest match
				if (lcsChildren[node->child[charToIndex(sigma[i], sigma, sigmaLength)]->stringID] == NULL) {
					lcsChildren[node->child[charToIndex(sigma[i], sigma, sigmaLength)]->stringID] = node->child[charToIndex(sigma[i], sigma, sigmaLength)];
				}
				else if (lcsChildren[node->child[charToIndex(sigma[i], sigma, sigmaLength)]->stringID]->parent->string_depth < node->string_depth) {
					lcsChildren[node->child[charToIndex(sigma[i], sigma, sigmaLength)]->stringID] = node->child[charToIndex(sigma[i], sigma, sigmaLength)];
				}
			}
		}
	}
	else {
		node->stringID = previousChild->stringID;
	}
}

void ComputeLCSCoords(Node** lcsChildren, int** lcsStartCoords, int** lcsEndCoords, char* stringsCat, int i, int j) {
	char* alpha = malloc(lcsChildren[0]->parent->string_depth + 1);
	alpha[lcsChildren[0]->parent->string_depth] = '\0';
	char* substringS1, * substringS2; //stringsCat is the haystack and alpha is the needle. The 'substringS#' char * is the return value of the needle string in the haystack
	Node* curNode = lcsChildren[0]->parent;

	while (curNode->id != 0) {
		memcpy(&alpha[curNode->string_depth - (curNode->end - curNode->start) - 1], &stringsCat[curNode->start], curNode->end - curNode->start + 1);
		curNode = curNode->parent;
	}

	substringS1 = strstr(stringsCat, alpha);
	(*lcsStartCoords)[0] = substringS1 - stringsCat;
	(*lcsEndCoords)[0] = (*lcsStartCoords)[0] + strlen(alpha) - 1;

	substringS2 = strstr(&stringsCat[inputStringRanges[lcsChildren[1]->staticStringID]->i], alpha);
	(*lcsStartCoords)[1] = substringS2 - stringsCat;
	(*lcsEndCoords)[1] = (*lcsStartCoords)[1] + strlen(alpha) - 1;
	printf("LCS Length for Strings (%d, %d): %d\n", i + 1, j + 1, strlen(alpha));
	free(alpha);
}

//Recursive retrace algorithm for the DP Table *****NEED TO MAKE NOT RECURSIVE*****
void retrace(DP_cell** table, int i, int j, char* s1, char* s2, int maxVal, int* matches, int offseti, int offsetj)
{
	while (i - offseti > 0 && j - offsetj > 0) {
		switch (table[i - offseti][j - offsetj].retraceCase) {
		case 0:
			//retrace(table, i - 1, j - 1, s1, s2, maxVal, matches, offseti, offsetj);
			i -= 1;
			j -= 1;
			if (s1[i - 1] == s2[j - 1]) {
				(*matches)++;
			}
			break;
		case 1:
			//retrace(table, i - 1, j, s1, s2, maxVal, matches, offseti, offsetj);
			i -= 1;
			break;
		case 2:
			//retrace(table, i, j - 1, s1, s2, maxVal, matches, offseti, offsetj);
			j -= 1;
			break;
		}
	}
}

void FreeDPTable(DP_cell** table, int m) {
	for (int i = 0; i < m; i++) {
		free(table[i]);
	}
	free(table);
}

//Computes the similarity matrix
Node* ComputeSimilarityMatrix(char** argv, char* sigma, int sigmaLength, FILE* file1, int* sequenceLength, char** name, char** sequence, int numInputs, DP_cell* currentRow, DP_cell* previousRow, int** similarityMatrix) {
	LARGE_INTEGER start, end, elapsedMicroseconds;
	LARGE_INTEGER frequency;
	Node* root, ** lcsChildren;
	int a = 0, b = 0, c = 0, * lcsStartCoords, * lcsEndCoords, m, n;
	char** reversedInputStrings = (char**)malloc(numInputs * sizeof(char*)), * stringsCat = NULL;

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&start);

	lcsChildren = malloc(numInputs * sizeof(Node*));
	lcsStartCoords = (int*)malloc(2 * sizeof(int*));
	lcsEndCoords = (int*)malloc(2 * sizeof(int*));
	for (int i = 0; i < numInputs; i++) {
		for (int j = i + 1; j < numInputs; j++) {
			globalStringID = 0;
			for (int i = 0; i < numInputs; i++) {
				lcsChildren[i] = NULL;
			}
			printf("Time Building ST (%d, %d)\n", i, j);
			root = BuildGST2Inputs(argv[i + 2], argv[j + 2], sigma, sigmaLength, file1, sequenceLength, name, sequence, &stringsCat);
			DFSColorNodesLCS(root, sigmaLength, lcsChildren, sigma);
			ComputeLCSCoords(lcsChildren, &lcsStartCoords, &lcsEndCoords, stringsCat, i, j);
			m = (lcsStartCoords[0] + 1) - inputStringRanges[lcsChildren[0]->staticStringID]->i;
			n = (lcsStartCoords[1] + 1) - inputStringRanges[lcsChildren[1]->staticStringID]->i;
			ReverseStrings(lcsChildren, m, n, inputStringRanges[lcsChildren[0]->staticStringID]->i, inputStringRanges[lcsChildren[0]->staticStringID]->i + m, inputStringRanges[lcsChildren[1]->staticStringID]->i, inputStringRanges[lcsChildren[1]->staticStringID]->i + n, reversedInputStrings, stringsCat);
			currentRow = NULL;
			previousRow = NULL;
			printf("Aligning a...\n");
			a = ComputeGlobalAlignment(&currentRow, &previousRow, m, n, lcsChildren, reversedInputStrings, 0, 0, i, j);
			b = lcsChildren[0]->parent->string_depth;
			m = (inputStringRanges[lcsChildren[0]->staticStringID]->j - inputStringRanges[lcsChildren[0]->staticStringID]->i) - (lcsEndCoords[0] - inputStringRanges[lcsChildren[0]->staticStringID]->i);
			n = (inputStringRanges[lcsChildren[1]->staticStringID]->j - inputStringRanges[lcsChildren[1]->staticStringID]->i) - (lcsEndCoords[1] - inputStringRanges[lcsChildren[1]->staticStringID]->i);
			currentRow = NULL;
			previousRow = NULL;
			printf("Aligning c...\n");
			c = ComputeGlobalAlignment(&currentRow, &previousRow, m, n, lcsChildren, inputStrings, lcsEndCoords[0] - (inputStringRanges[lcsChildren[0]->staticStringID]->i) + 1, lcsEndCoords[1] - (inputStringRanges[lcsChildren[1]->staticStringID]->i) + 1, i, j);
			printf("\n");
			similarityMatrix[i][j] = a + b + c;
			FreeTree(root, sigma, sigmaLength);
			free(stringsCat);
		}
	}

	QueryPerformanceCounter(&end);
	elapsedMicroseconds.QuadPart = end.QuadPart - start.QuadPart;
	elapsedMicroseconds.QuadPart *= 1000000;
	elapsedMicroseconds.QuadPart /= frequency.QuadPart;

	printf("Total Time to Compute Matrix D: %llu microseconds\n", elapsedMicroseconds.QuadPart);

	free(lcsChildren);
	free(lcsStartCoords);
	free(lcsEndCoords);
}

int ComputeNumInputs(char** argv) {
	int numInputs = 0;
	int i = 2;
	while (argv[i] != NULL && strcmp(argv[i], "NULL")) {
		numInputs++;
		i++;
	}
	return numInputs;
}

DP_cell** InitDPTable(DP_cell** currentRow, DP_cell** previousRow, int m, int n, int h, int g) {
	*currentRow = malloc(n * sizeof(DP_cell));
	*previousRow = malloc(n * sizeof(DP_cell));

	//Initializes the DP table for global alignment
	for (int i = 0; i < n; i++)
	{
		(*currentRow)[i].score = 0;
		(*currentRow)[i].Dscore = 0;
		(*currentRow)[i].Sscore = 0;
		(*currentRow)[i].Iscore = 0;
		(*currentRow)[i].numMatchesS = 0;
		(*currentRow)[i].numMatchesD = 0;
		(*currentRow)[i].numMatchesI = 0;
		(*previousRow)[i].score = 0;
		(*previousRow)[i].Dscore = 0;
		(*previousRow)[i].Sscore = 0;
		(*previousRow)[i].Iscore = 0;
		(*previousRow)[i].numMatchesS = 0;
		(*previousRow)[i].numMatchesD = 0;
		(*previousRow)[i].numMatchesI = 0;
	}

	(*currentRow)[0].Sscore = INT_MIN - h - g;
	(*currentRow)[0].Dscore = h + 1 * g;
	(*currentRow)[0].Iscore = INT_MIN - h - g;
	(*currentRow)[0].retraceCase = DetermineRetrace((*currentRow)[0].Sscore, (*currentRow)[0].Dscore, (*currentRow)[0].Iscore);
	(*previousRow)[0].Dscore = INT_MIN - h - g;
	(*previousRow)[0].Sscore = h + 2 * g;
	(*previousRow)[0].Iscore = INT_MIN - h - g;
	(*previousRow)[0].retraceCase = DetermineRetrace((*previousRow)[0].Sscore, (*previousRow)[0].Dscore, (*previousRow)[0].Iscore);

	for (int i = 1; i < n; i++)
	{
		(*previousRow)[i].Sscore = INT_MIN - h - g;
		(*previousRow)[i].Dscore = INT_MIN - h - g;
		(*previousRow)[i].Iscore = h + i * g;
		(*previousRow)[i].retraceCase = DetermineRetrace((*previousRow)[i].Sscore, (*previousRow)[i].Dscore, (*previousRow)[i].Iscore);
	}
}

int DetermineRetrace(int a, int b, int c) {
	if (a >= b && a >= c) {
		return 0;
	}
	else if (b >= a && b >= c) {
		return 1;
	}
	else {
		return 2;
	}
}

int Maximum3Vars(DP_cell** currentRow, DP_cell** previousRow, int x, int y, int z, int i, int j)
{
	if (x >= y && x >= z)
	{
		(*currentRow)[j].retraceCase = 0;
		return x;
	}
	else if (y >= x && y >= z)
	{
		(*currentRow)[j].retraceCase = 1;
		return y;
	}
	else
	{
		(*currentRow)[j].retraceCase = 2;
		return z;
	}
}

int maximum(int x, int y, int *totalMatches, DP_cell currentCell) {
	if (x <= y) {
		switch (currentCell.retraceCase)
		{
		case 0:
			*totalMatches = currentCell.numMatchesS;
			break;
		case 1:
			*totalMatches = currentCell.numMatchesD;
			break;
		case 2:
			*totalMatches = currentCell.numMatchesI;
			break;
		}
		return y;
	}
	else {
		return x;
	}
}

int s(char a, char b, const int match, const int mismatch) {
	if (a == b) {
		return match;
	}
	else {
		return mismatch;
	}
}

void ReverseStrings(Node** lcsChildren, int m, int n, int siStart, int siEnd, int sjStart, int sjEnd, char** reversedInputStrings, char* stringsCat) {
	reversedInputStrings[lcsChildren[0]->staticStringID] = (char*)malloc(m);
	reversedInputStrings[lcsChildren[1]->staticStringID] = (char*)malloc(n);


	int j = 0;
	for (int i = siEnd - 2; i >= siStart; i--) {
		reversedInputStrings[lcsChildren[0]->staticStringID][j] = stringsCat[i];
		j++;
	}
	reversedInputStrings[lcsChildren[0]->staticStringID][j] = '\0';

	j = 0;
	for (int i = sjEnd - 2; i >= sjStart; i--) {
		reversedInputStrings[lcsChildren[1]->staticStringID][j] = stringsCat[i];
		j++;
	}
	reversedInputStrings[lcsChildren[1]->staticStringID][j] = '\0';
}

void MainRecurrenceGlobal(DP_cell** currentRow, DP_cell** previousRow, int m, int n, Node** lcsChildren, const int match, const int mismatch, const int h, const int g, char** string, int* matches, int offseti, int offsetj, int s1Index, int s2Index) {
	int a, b, c, totalMax = INT_MIN, totalMatches = 0; //totalMax is used to determine the cell that contains the maximum score from the overall matrix
	DP_cell** temp;

	//Main Recurrence
	for (int i = 1; i < m; i++) {
		for (int j = 1; j < n; j++) {
			a = Maximum3Vars(currentRow, previousRow, (*previousRow)[j - 1].Sscore, (*previousRow)[j - 1].Dscore, (*previousRow)[j - 1].Iscore, i, j) + s(string[lcsChildren[0]->staticStringID][i - 1 + offseti], string[lcsChildren[1]->staticStringID][j - 1 + offsetj], match, mismatch);
			b = Maximum3Vars(currentRow, previousRow, (*previousRow)[j].Sscore + h + g, (*previousRow)[j].Dscore + g, (*previousRow)[j].Iscore + h + g, i, j);
			c = Maximum3Vars(currentRow, previousRow, (*currentRow)[j - 1].Sscore + h + g, (*currentRow)[j - 1].Dscore + h + g, (*currentRow)[j - 1].Iscore + g, i, j);

			int maxVal = Maximum3Vars(currentRow, previousRow, a, b, c, i, j);
			(*currentRow)[j].score = maxVal;
			(*currentRow)[j].Sscore = a;
			(*currentRow)[j].Dscore = b;
			(*currentRow)[j].Iscore = c;
			switch ((*currentRow)[j].retraceCase)
			{
			case 0:
				if (s(string[lcsChildren[0]->staticStringID][i - 1 + offseti], string[lcsChildren[1]->staticStringID][j - 1 + offsetj], match, mismatch) == match) {
					switch ((*previousRow)[j - 1].retraceCase) {
					case 0:
						(*currentRow)[j].numMatchesS = (*previousRow)[j - 1].numMatchesS + 1;
						break;
					case 1:
						(*currentRow)[j].numMatchesS = (*previousRow)[j - 1].numMatchesD + 1;
						break;
					case 2:
						(*currentRow)[j].numMatchesS = (*previousRow)[j - 1].numMatchesI + 1;
						break;
					}
				}
				else {
					switch ((*previousRow)[j - 1].retraceCase) {
					case 0:
						(*currentRow)[j].numMatchesS = (*previousRow)[j - 1].numMatchesS;
						break;
					case 1:
						(*currentRow)[j].numMatchesS = (*previousRow)[j - 1].numMatchesD;
						break;
					case 2:
						(*currentRow)[j].numMatchesS = (*previousRow)[j - 1].numMatchesI;
						break;
					}
				}
				break;
			case 1:
				switch ((*previousRow)[j].retraceCase) {
				case 0:
					(*currentRow)[j].numMatchesD = (*previousRow)[j].numMatchesS;
					break;
				case 1:
					(*currentRow)[j].numMatchesD = (*previousRow)[j].numMatchesD;
					break;
				case 2:
					(*currentRow)[j].numMatchesD = (*previousRow)[j].numMatchesI;
					break;
				}
				break;
			case 2:
				switch ((*currentRow)[j - 1].retraceCase) {
				case 0:
					(*currentRow)[j].numMatchesI = (*currentRow)[j - 1].numMatchesS;
					break;
				case 1:
					(*currentRow)[j].numMatchesI = (*currentRow)[j - 1].numMatchesD;
					break;
				case 2:
					(*currentRow)[j].numMatchesI = (*currentRow)[j - 1].numMatchesI;
					break;
				}
				break;
			}
			totalMax = maximum(totalMax, maxVal, &totalMatches, (*currentRow)[j]);
		}
		temp = currentRow;
		currentRow = previousRow;
		previousRow = temp;
	}
	*matches = totalMatches;
	//retrace(*table, maxIndices->i + offseti, maxIndices->j + offsetj, string[lcsChildren[0]->staticStringID], string[lcsChildren[1]->staticStringID], totalMax, matches, offseti, offsetj);
}

int ComputeGlobalAlignment(DP_cell** currentRow, DP_cell** previousRow, int m, int n, Node** lcsChildren, char** string, int offseti, int offsetj, int s1Index, int s2Index) {
	LARGE_INTEGER start, end, elapsedMicroseconds;
	LARGE_INTEGER frequency;
	const int match = 1, mismatch = -2, h = -5, g = -1;
	int matches = 0;

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&start);
	InitDPTable(currentRow, previousRow, m, n, h, g);
	MainRecurrenceGlobal(currentRow, previousRow, m, n, lcsChildren, match, mismatch, h, g, string, &matches, offseti, offsetj, s1Index, s2Index);
	QueryPerformanceCounter(&end);
	elapsedMicroseconds.QuadPart = end.QuadPart - start.QuadPart;
	elapsedMicroseconds.QuadPart *= 1000000;
	elapsedMicroseconds.QuadPart /= frequency.QuadPart;

	printf("Time to Align Strings (%d, %d): %llu microseconds\n", s1Index, s2Index, elapsedMicroseconds.QuadPart);
	free(*currentRow);
	free(*previousRow);
	return matches;
}

void InitSimilarityMatrix(int*** similarityMatrix, int numInputs) {
	(*similarityMatrix) = (int**)malloc(numInputs * sizeof(int*));
	for (int i = 0; i < numInputs; i++) {
		(*similarityMatrix)[i] = (int*)malloc(numInputs * sizeof(int));
	}

	for (int i = 0; i < numInputs; i++) {
		for (int j = 0; j < numInputs; j++) {
			(*similarityMatrix)[i][j] = 0;
			if (i == j) { //If we are on the diagonal then we are comparing a string to itself
				(*similarityMatrix)[i][j] = inputStringRanges[i]->j - inputStringRanges[i]->i;
			}
		}
	}
}

void CopySimilarityMatrixDiagonals(int** similarityMatrix, int numInputs) {
	for (int i = 0; i < numInputs; i++) {
		for (int j = i + 1; j < numInputs; j++) {
			similarityMatrix[j][i] = similarityMatrix[i][j];
		}
	}
}

void OutputSimilarityMatrix(int** similarityMatrix, int numInputs) {
	FILE* file;
	file = fopen("similaritymatrix.txt", "w");
	CopySimilarityMatrixDiagonals(similarityMatrix, numInputs);
	printf("Similarity Matrix -----\n   ");
	fprintf(file, "Similarity Matrix -----\n   ");
	for (int i = 0; i < numInputs; i++) {
		if (i >= numInputs - 1) {
			printf("%d", i);
			fprintf(file, "%d", i);
			break;
		}
		printf("%d\t", i);
		fprintf(file, "%d\t", i);
	}
	printf("\n");
	fprintf(file, "\n");
	for (int i = 0; i < numInputs; i++) {
		printf("%d\t|", i);
		fprintf(file, "%d\t|", i);
		for (int j = 0; j < numInputs; j++) {
			if (i >= numInputs - 1) {
				printf("");
			}
			if (j >= numInputs - 1) {
				printf("%d", similarityMatrix[i][j]);
				fprintf(file, "%d", similarityMatrix[i][j]);
				break;
			}
			printf("%d\t", similarityMatrix[i][j]);
			fprintf(file, "%d\t", similarityMatrix[i][j]);
		}
		printf("|\n");
		fprintf(file, "|\n");
	}
}

void InitShortestMatches(Node ***shortestMatches, int numInputs) {
	//Initializes an array to hold the shortest matches for each string Si
	*shortestMatches = (Node*)malloc((numInputs) * sizeof(Node*));
	for (int i = 0; i < numInputs; i++)
	{
		(*shortestMatches)[i] = NULL;
	}
}

void OutputFingerprints(Node **shortestMatches, char *stringsCat, int numInputs) {
	FILE* file;
	char* fingerprint = NULL;
	Node* currNode = NULL;
	file = fopen("fingerprints.txt", "w");
	printf("String ID       | Fingerprint Length\n");
	for (int i = 0; i < numInputs; i++) {
		currNode = shortestMatches[i];
		fingerprint = malloc(currNode->parent->string_depth + 2); //Parent's string depth plus 1 character for ci and 1 for null character
		fingerprint[currNode->parent->string_depth + 1] = '\0';
		memcpy(&fingerprint[currNode->parent->string_depth], &stringsCat[currNode->start], 1);
		currNode = currNode->parent;
		while (currNode->id != 0) {
			memcpy(&fingerprint[currNode->string_depth - (currNode->end - currNode->start) - 1], &stringsCat[currNode->start], currNode->end - currNode->start + 1);
			currNode = currNode->parent;
		}
		fprintf(file, "String ID %d: %s\n", i, fingerprint);
		printf("\t%d\t|\t%d\n", i, strlen(fingerprint));
	}
	fclose(file);
}

void FingerprintStrings(Node *root, char** argv, char* sigma, int sigmaLength, FILE* file1, int* sequenceLength, char** name, char** sequence, char** stringsCat, int numInputs, Node **shortestMatches) {
	LARGE_INTEGER start, end, elapsedMicroseconds;
	LARGE_INTEGER frequency;
	printf("Fingerprinting ST Construction\n");
	root = BuildGST(argv, sigma, sigmaLength, file1, &sequenceLength, &name, &sequence, &stringsCat);

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&start);

	//Colors the nodes in the suffix tree and computes the shortest matches for each string Si
	DFSColorNodesSUS(root, sigmaLength, shortestMatches, sigma, stringsCat);

	QueryPerformanceCounter(&end);
	elapsedMicroseconds.QuadPart = end.QuadPart - start.QuadPart;
	elapsedMicroseconds.QuadPart *= 1000000.0;
	elapsedMicroseconds.QuadPart /= frequency.QuadPart;

	printf("Time to Identify Fingerprints: %llu microseconds\n", elapsedMicroseconds.QuadPart);

	OutputFingerprints(shortestMatches, stringsCat, numInputs);

	FreeTree(root, sigma, sigmaLength);
}

int main(int argc, char* argv[])
{
	//USED IN DEBUG
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	char* arg1, * arg2, * name = NULL, * sequence = NULL, * sigma, *stringsCat = NULL;
	int sequenceLength = 0, sigmaLength, * numInternalNodes = malloc(sizeof(int)), numLeaves = 0, * averageStringDepth = malloc(sizeof(int)), * deepestString = malloc(sizeof(int)), numInputs;
	FILE* file1 = NULL, * file2 = NULL;
	Node* deepestNode = NULL, ** shortestMatches;
	DP_cell* currentRow = NULL, * previousRow = NULL; //Dynamic programming table for the global alignment algorithm
	int** similarityMatrix = NULL;

	*numInternalNodes = 0;
	*averageStringDepth = 0;
	*deepestString = 0;
	Node* root = NULL;

	arg1 = argv[1];
	/* INPUTS USED DURING DEBUGGING
	argv = realloc(argv, 15 * sizeof(char *));
	arg1 = "DNAAlphabet.txt";
	argv[2] = "input1.txt";
	argv[3] = "input2.txt";
	argv[4] = "input3.txt";
	argv[5] = "input4.txt";
	argv[6] = "input5.txt";
	argv[7] = "input6.txt";
	argv[8] = "input7.txt";
	argv[9] = "input8.txt";
	argv[10] = "input9.txt";
	argv[11] = "input10.txt";
	argv[12] = NULL;
	*/

	numInputs = ComputeNumInputs(argv);

	inputStrings = (char**)malloc(numInputs * sizeof(char*));
	inputStringRanges = (Tuple**)malloc(numInputs * sizeof(Tuple*));
	InitShortestMatches(&shortestMatches, numInputs);

	//Reads the alphabet file, will be the first argument in the program from cmd line
	if (!readAlphabet(arg1, &sigma)) {
		printf("Error: Couldn't Read Alphabet File\n");
		return 1;
	}

	sigmaLength = strlen(sigma); //Length of the alphabet

	FingerprintStrings(root, argv, sigma, sigmaLength, file1, sequenceLength, name, sequence, stringsCat, numInputs, shortestMatches);

	InitSimilarityMatrix(&similarityMatrix, numInputs);

	root = ComputeSimilarityMatrix(argv, sigma, sigmaLength, file1, &sequenceLength, &name, &sequence, numInputs, currentRow, previousRow, similarityMatrix);

	OutputSimilarityMatrix(similarityMatrix, numInputs);

	free(sigma);
	free(sequence);
	free(name);
	free(numInternalNodes);
	free(averageStringDepth);
	free(deepestString);

	return 0;
}