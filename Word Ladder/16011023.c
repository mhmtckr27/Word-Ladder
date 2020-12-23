#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#define FILENAMELENGTH 32
#define LINELENGTH 10
#define WORDLENGTH 7
#define BUFFERSIZE 7  //kelime uzunlugu + 2 olmali.

typedef struct node{
	int location;
	char* word;
	struct node* ancestor;
	struct node* next;
}NODE;

typedef struct queue{
	NODE* front;
	NODE* rear;
}QUEUE;

void init(QUEUE *q)
{
	q->front = NULL;
	q->rear = NULL;
}

int isEmpty(QUEUE *q)
{
	return q->front == NULL;
}

void enqueue(QUEUE *q, int location, char* word, NODE* ancestor)
{
	NODE* tmp;
	tmp = (NODE*)malloc(sizeof(NODE));
	tmp->word = (char*)malloc(sizeof(char)*WORDLENGTH);
	tmp->location = location;
	tmp->word = word;
	tmp->next = NULL;
	tmp->ancestor = ancestor;
	if (!isEmpty(q))
	{
		q->rear->next = tmp;
		q->rear = tmp;
	}
	else
	{
		q->rear = tmp;
		q->front = tmp;
	}
}

NODE* dequeue(QUEUE *q)
{
	NODE* tmp;
	int val = q->front->location;
	tmp = q->front;
	q->front = q->front->next;
	//free(tmp);
	return tmp;
}

int** getInputFile(int** adjacencyMatrix, int* wordCount, char* fileName)
{
	FILE * file;
	char buffer[LINELENGTH];
	int i;
	printf("Kelimelerin bulundugu dosya adini giriniz: ");
	scanf("%s", fileName);
	file = fopen(fileName, "r");
	*wordCount = 0;
	while (fgets(buffer, BUFFERSIZE, file))
	{
		++*wordCount;
	}
	fclose(file);
	adjacencyMatrix = (int **)calloc(*wordCount,sizeof(int*));
	for (i = 0; i < *wordCount; ++i)
	{
		adjacencyMatrix[i] = (int*)calloc(*wordCount, sizeof(int));
	}
	return adjacencyMatrix;
}

int** populateAdjacencyMatrix( int** adjacencyMatrix, char* fileName, int* wordCount)
{
	int i;
	int j;
	int x;
	int y;
	int konum;
	FILE * file;
	adjacencyMatrix = getInputFile(adjacencyMatrix, wordCount, fileName);
	char word1[WORDLENGTH];
	char word2[WORDLENGTH];
	file = fopen(fileName, "r");
	for (i = 0; i < *wordCount; ++i)
	{
		fgets(word1, BUFFERSIZE, file);
		konum = ftell(file);
		for (j = i + 1; j < *wordCount; ++j)
		{
			fgets(word2, BUFFERSIZE, file);
			x = 0;
			y = 0;
			while ((y < WORDLENGTH-2) && (x < 2))
			{
				if (word1[y] != word2[y])
				{
					++x;
				}
				++y;
			}
			if (x <= 1)
			{
				adjacencyMatrix[i][j] = 1;
				adjacencyMatrix[j][i] = 1;
			}
		}
		fseek(file, konum, SEEK_SET);
	}
	fclose(file);
	printf("Komsuluk matrisi olusturuldu.\n");
	return  adjacencyMatrix;
}

void findWordsInFile(char* fileName, int* findFirst, int* findSecond, int* firstPlace, int* secondPlace, char* word1, char* word2)
{
	FILE *file;
	char buffer[LINELENGTH];
	int x = 0;
	printf("1. kelimeyi giriniz: ");
	scanf("%s", word1);
	printf("2. kelimeyi girinz: ");
	scanf("%s", word2);
	file = fopen(fileName, "r");

	while (fgets(buffer, BUFFERSIZE, file))
	{
		if (strstr(buffer, word1))
		{
			*firstPlace = x;
			*findFirst = 1;
		}
		else if (strstr(buffer, word2))
		{
			*secondPlace = x;
			*findSecond = 1;
		}
		++x;
	}
	fclose(file);	
}

void testGraphValidity(int ** adjacencyMatrix, char* fileName, int* wordCount)
{
	char word1[WORDLENGTH];
	char word2[WORDLENGTH];
	int *findFirst = (int*)malloc(sizeof(int));
	int *findSecond = (int*)malloc(sizeof(int));
	int *firstPlace = (int*)malloc(sizeof(int));
	int *secondPlace = (int*)malloc(sizeof(int));
	findWordsInFile(fileName, findFirst, findSecond, firstPlace, secondPlace, word1, word2);
	if(!findFirst || !findSecond)
	{
		printf("Kelimelerden birisi dosyada bulunamadi");
		return;
	}
	if(adjacencyMatrix[(*firstPlace)][(*secondPlace)])
	{
		printf("Komsuluk VAR.\n");
		return;
	}
	printf("Komsuluk YOK.\n");
}

void findTransformation(int** adjacencyMatrix, char* fileName, int* wordCount)
{
	FILE *file;
	char word1[WORDLENGTH];
	char word2[WORDLENGTH];
	char currentWord[WORDLENGTH];
	int * visited = (int*)calloc(*wordCount, sizeof(int));
	QUEUE *q = (QUEUE*)malloc(sizeof(QUEUE));
	NODE* tmp;
	int *findFirst = (int*)malloc(sizeof(int));
	int *findSecond = (int*)malloc(sizeof(int));
	int *firstPlace = (int*)malloc(sizeof(int));
	int *secondPlace = (int*)malloc(sizeof(int));
	int exitNode;
	int i;
	int j = 0;
	char** printStack;
	init(q);
	findWordsInFile(fileName, findFirst, findSecond, firstPlace, secondPlace, word1, word2);
	file = fopen(fileName, "r");
	if (!*findFirst || !*findSecond)
	{
		printf("Kelimelerden birisi dosyada bulunamadi");
		return;
	}
	enqueue(q, *firstPlace, word1, NULL);
	printStack = (char**)malloc(sizeof(char*)*WORDLENGTH);
	visited[*firstPlace] = 1;
	printf("Tum kuyruk yazdiriliyor..\n");
	while(!isEmpty(q))
	{
		tmp = dequeue(q);
		exitNode = tmp->location;
		if(j!=0)
		printf("\t%d-%s", j, tmp->word);
		if(exitNode==*secondPlace)
		{
			printf("\n\nYol yazdiriliyor..\n");
			i = 0;
			printStack[i] = (char*)malloc(sizeof(char)*WORDLENGTH);
			strcpy(printStack[i], word1);
			++i;
			while(tmp->ancestor != NULL)
			{
				printStack[i] = (char*)malloc(sizeof(char)*WORDLENGTH);
				fseek(file, tmp->location*WORDLENGTH, SEEK_SET);
				fgets(tmp->word, BUFFERSIZE, file);
				strcpy(printStack[i], tmp->word);
				tmp = tmp->ancestor;
				++i;
			}
			printf("\t%d-%s\n", 0, word1);
			for (j = i - 1; j > 0; --j) 
			{
				printf("\t%d-%s", i - j, printStack[j]);
			}
			fclose(file);
			printf("\n>>>Islem %d adimda tamamlandi.\n%s bulundu.", i-j-1,word2);
			return;
		}
		fseek(file, exitNode*WORDLENGTH, SEEK_SET);
		fgets(currentWord, BUFFERSIZE, file);
		for (i = 0; i < *wordCount; ++i)
		{
			if(adjacencyMatrix[exitNode][i]==1)
			{
				if(!visited[i])
				{
					enqueue(q, i, currentWord, tmp);
					visited[i] = 1;
				}
			}
		}
		++j;
	}
	printf("Donusum bulunamadi.");
	fclose(file);
}

void start()
{
	int **adjacencyMatrix = (int**) malloc(sizeof(int*));
	char fileName[FILENAMELENGTH];
	int * wordCount = (int*) calloc(1, sizeof(int));
	int choice;
	printf("0- Cikis\n1- Kelime grafini olustur\n2- Graf dogrulugu testi\n3- Iki kelime arasi donusum testi\n\n");
	do
	{
		printf("\n>>Yapmak istediginiz islemi seciniz: ");
		scanf("%d", &choice);
		switch (choice)
		{
		case 0:
			return;
		case 1:
			adjacencyMatrix = populateAdjacencyMatrix(adjacencyMatrix, fileName, wordCount);
			break;
		case 2:
			testGraphValidity(adjacencyMatrix, fileName, wordCount);
			break;
		case 3:
			findTransformation(adjacencyMatrix, fileName, wordCount);
			break;
		default: 
			break;
		}
	} while (choice);

}

int main()
{
	start();
	return 0;
}
