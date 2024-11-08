#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 102
#define ERR_ARGUMENTS_COUNT 2
#define ERR_NUMBER_SEQ_NOT_ONLY_NUMBERS 3
#define ERR_NAME_TOO_LONG 4
#define ERR_CONTACT_NUMBER_NOT_ONLY_VALID_CHARS 5
#define ERR_ARG_WRONG_ORDER 6

#define BONUS_ARGUMENT "-s"

// LookUpTabulka na zadefinovani znaku hledanych podle sekvence
const char *TNINE_LOOKUP[10] = {
    "0+",   "1",    "2abc",  "3def", "4ghi",   // 0-4
    "5jkl", "6mno", "7pqrs", "8tuv", "9wxyz"}; // 5-9


//metoda ktera porovna dva stringy
int compareString(char *string1, char *string2) {
  int str1Length = strlen(string1);
  int str2Length = strlen(string2);
  if (str1Length != str2Length) {
    return false;
  }

  for (int actCharPos = 0; actCharPos < str1Length; actCharPos++) {
    if (string1[actCharPos] != string2[actCharPos]) {
      return false;
    }
  }
  return true;
}

// prevod na mala pismena 
void convertToLower(char *buffer) { 
  int bufferLen = strlen(buffer);
  for (int i = 0; i < bufferLen; i++) {
    if (buffer[i] >= 'A' && buffer[i] <= 'Z')
      buffer[i] += 32;
  }
}

//odstraneni znaku \n na konci radku
void removeEndOfLine(char *buffer) {
  int bufferLen = strlen(buffer);
  if (buffer[bufferLen - 1] == '\n') {
    buffer[bufferLen - 1] = '\0';
  }
}

// kontroluje input
// podle poctu argumentu resime jestli je zadan bonus,pripadne jestli je spravne poradi
// pripadne nastava chyba
// pripadne je zadano vic argumentu nez bylo domluveno => nastava chyba
int checkInput(int argc, char *argument, int *bonus) {
  switch (argc) {
  case 1:case 2: 
    return true;
  case 3:
    if (compareString(argument, BONUS_ARGUMENT)) {
      return *bonus = 1;
    } else {
      fprintf(stderr, "Wrong order or arguments!!\n");
      return ERR_ARG_WRONG_ORDER;
    }
  default:
    fprintf(stderr, "Too many arguments!\n");
    return ERR_ARGUMENTS_COUNT;
  }
}

//metoda ktera kontroluje nactena data
//kontrolujeme jestli je zadana sekvence a pripadne jestli neobsahuje neco jineho nez cisla
int checkData(char buffer[BUFFER_SIZE], char bufferNum[BUFFER_SIZE],char *numSequence) {

  int bufferNumLen = strlen(bufferNum);
  int numSequenceLen;

  if (numSequence != NULL) { // Pokud uzivatel zada sekvenci, kontrola sekvence

    numSequenceLen = strlen(numSequence);

    for (int i = 0; i < numSequenceLen; i++) {
      // kontrola aby sekvence obsahovala pouze cisla, plusko je jako 0
      if (!(numSequence[i] >= '0' && numSequence[i] <= '9')) {
        fprintf(stderr, "Number sequence doesn't contain only numbers!\n");
        return ERR_NUMBER_SEQ_NOT_ONLY_NUMBERS;
      }
    }
  }

  if (strlen(buffer) > BUFFER_SIZE-1) { // kontrola delky jmena
    fprintf(stderr, "Name in contact list is too long!\n");
    return ERR_NAME_TOO_LONG;
  }

  // kontrola cisla aby neobsahovalo jine znaky nez cisla
  for (int i = 0; i < bufferNumLen - 1; i++) {
    if (!(((bufferNum[i] >= '0') && (bufferNum[i] <= '9')) ||
          (bufferNum[i] == '+'))) {
      fprintf(stderr, "Number in contact list doesn't contain only numbers!\n");
      return ERR_CONTACT_NUMBER_NOT_ONLY_VALID_CHARS;
    }
  }
  return 0;
}

int checkBufferForSequence(char buffer[BUFFER_SIZE], char *numSequence,
                           int bonus) {
  int bufferLen = strlen(buffer);
  int numberSequenceLen = strlen(numSequence);
  int numberMatches;
  // kontrola jestli jmeno obsahuje ciselnou sekvenci
  if (bonus != 1) {
    for (int j = 0; j <= bufferLen - numberSequenceLen; j++) {
      numberMatches = 1;
      for (int i = 0; i < numberSequenceLen; i++) {
        int number = numSequence[i] - '0';
        char lowerChar = buffer[j + i];
        // pokud je znak v tabulce
        if (strchr(TNINE_LOOKUP[number], lowerChar) == NULL) {
          numberMatches = 0;
          break;
        }
      }
      if (numberMatches) {
        return true;
      }
    }
  } else { // bonus -s, 
    int sequenceIndex = 0;
    for (int j = 0; j < bufferLen && sequenceIndex < numberSequenceLen; j++) {
      int number = numSequence[sequenceIndex] - '0';
      char lowerChar = buffer[j];
      if (strchr(TNINE_LOOKUP[number], lowerChar) != NULL) {
        sequenceIndex++;
      }
    }
    if (sequenceIndex == numberSequenceLen) {
      return true;
    }
  }
  return false;
}

int checkContactsForNumSequence(char *numSequence, char buffer[BUFFER_SIZE],
                                char bufferNum[BUFFER_SIZE], int bonus) {
  bool notFound = true;

  while (fgets(buffer, BUFFER_SIZE, stdin) != NULL &&
         fgets(bufferNum, BUFFER_SIZE, stdin) != NULL) {
    // kontrola nactenych dat
    int errorCode = checkData(buffer, bufferNum, numSequence);
    if (errorCode != 0) {
      return errorCode; // Pokud je chyba, vrátíme chybový kód
    }
    convertToLower(buffer); // prevedeni jmena na mala pismena

    // odstraneni znaku '\n' na konci radku v nactenych datech
    removeEndOfLine(buffer);
    removeEndOfLine(bufferNum);

    // pokud je aspon jeden kontakt nalezen
    if (numSequence == NULL || checkBufferForSequence(buffer, numSequence, bonus) ||
        checkBufferForSequence(bufferNum, numSequence, bonus)) {
      notFound = false; // Not found se nevypise
      fprintf(stdout, "%s, %s\n", buffer, bufferNum);
    }
  }

  if (notFound) { // pokud nebyl nalezen zadny kontakt
    fprintf(stdout, "Not found\n");
  }
  return 0;
}


int main(int argc, char *argv[]) {
  char buffer[BUFFER_SIZE];
  char bufferNum[BUFFER_SIZE]; // string na ukladani nacteneho cisla
  int bonus = 0;
  char *numSequence;
  switch (checkInput(argc, argv[1],&bonus)) {
  case true:
       numSequence = (argc > 1) ? (bonus ? argv[2] : argv[1]) : NULL; 
       return checkContactsForNumSequence(numSequence, buffer, bufferNum, bonus);
  case ERR_ARGUMENTS_COUNT:
    return ERR_ARGUMENTS_COUNT;
  case ERR_ARG_WRONG_ORDER:
    return ERR_ARG_WRONG_ORDER;
  default:
    printf("Unexpected state.\n");
    return 42;
  }
}
