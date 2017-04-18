#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//Estructura para funciones
struct Function{
    char * functionName;
    struct Instruction * functionInstructions;
    struct Parameter * parameterList;
    struct Function * next;
}*firstFunction,*lastFunction;
//Estructura para los parametros de las funciones
struct Parameter{
    void * paramValue;
    int paramType;
    struct Parameter * next;
};
//Estructura para guardar cada instrucción del archivo
struct Instruction{
    int instructionNum;
    char * instructionName;
    void * param;
    int paramType;
    struct Instruction * next;
};
//Estructura para soporte de listas
struct List{
    void * element;
    int elementType;
    struct List * next;
};
// Almacen de datos para guardar variable, funciones y listas
struct DataWarehouse{
    char* name;
    int type;
    void * value;
    struct DataWarehouse * next;
}*dataHeader;
//Pila de datos
struct Stack{
    void * value;
    int type;
    struct Stack * next;
}*mainStack,*tempStack;


void insertFunction(struct Function ** functionP){
    struct Function * function = *functionP;

    if(firstFunction == NULL){
        firstFunction = function;
        lastFunction = function;
    }else{
        lastFunction->next = function;
        lastFunction = lastFunction->next;
    }
}

void insertFunctionParam(struct Function ** functionP,struct Parameter ** parameterP, struct Parameter * funcParamListCopy){
    struct Function * function = *functionP;
    struct Parameter * parameter = *parameterP;

    if(function->parameterList == NULL){
        function->parameterList = parameter;
    } else{
        while(funcParamListCopy->next != NULL){
            funcParamListCopy = funcParamListCopy->next;
        }
        funcParamListCopy->next = parameter;
    }
}

void extractFunction(char * charAt){
    char functionName[50] = {};
    int nameCounter = 0;

    struct Function * newFunction = (struct Function*)malloc(sizeof(struct Function));
    newFunction->parameterList = NULL;
    newFunction->functionInstructions = NULL;
    newFunction->next = NULL;

    while(*charAt != '('){      //diferente de '(' (40), si no, empiezan parametros
        functionName[nameCounter] = *charAt;
        nameCounter++;                      //extrae el nombre de función
        charAt++;
    }
    charAt++;   // pasa el '('
    newFunction->functionName = malloc(50);
    strcpy(newFunction->functionName, functionName);

    while(*charAt != ')'){ //si entra al while es porque tiene parametros
        if(*charAt == ')'){ //solo para cuando sigue leyendo caracteres
            break;
        }

        int paramCounter = 0;
        char paramHolder[30] = {};

        while(*charAt != ','){
            if(*charAt == ')'){
                break;
            }
            paramHolder[paramCounter] = *charAt;
            paramCounter++;
            charAt++;
        }
        struct Parameter * newParam = (struct Parameter*)malloc(sizeof(struct Parameter));
        newParam->next = NULL;
        newParam->paramValue = malloc(30);
        strcpy(newParam->paramValue,paramHolder);
        if(paramHolder[0] >= 48 && paramHolder[0] <= 57){//si el parametro empieza con numeros
            newParam->paramType = 2;
        }else{ //sino, es un string o nombre de variable
            newParam->paramType = 1;
        }
        insertFunctionParam(&newFunction,&newParam,newFunction->parameterList);
        if(*charAt == ')'){
            break;
        }else{
            charAt++;
        }
    }
    insertFunction(&newFunction);
}

void insertInstruction(struct Instruction ** instructionP){
    struct Instruction * funcInstructions = lastFunction->functionInstructions;
    struct Instruction * instruction = *instructionP;

    if(funcInstructions == NULL){
        lastFunction->functionInstructions = instruction;
    } else{
        while(funcInstructions->next != NULL){
            funcInstructions = funcInstructions->next;
        }
        funcInstructions->next = instruction;
    }
}

void extractInstruction(char * charAt){
    char instNumberHolder[5] = {};
    int instNumber;
    char instName[30] = {};

    int numCounter = 0;
    int nameCounter = 0;           //contadores para las posiciones del arreglo
    int paramCounter = 0;

    struct Instruction * newInstruction = (struct Instruction*)malloc(sizeof(struct Instruction));
    newInstruction->next = NULL;
    newInstruction->param = NULL;

    while(*charAt >= 48 && *charAt <=57){ //guarda los numeros en
        instNumberHolder[numCounter] = *charAt;
        charAt++;
        numCounter++;
    }
    instNumber = atoi(instNumberHolder);
    newInstruction->instructionNum = instNumber;

    while(!(*charAt >= 65 && *charAt <= 90)){  //mientras no sea mayuscula lo que sigue
        charAt++;
    }
    while((*charAt >= 65 && *charAt <= 90)||(*charAt == 95)){  //si es mayuscula o '_'
        instName[nameCounter] = *charAt;
        charAt++;
        nameCounter++;
    }
    newInstruction->instructionName = malloc(30);
    strcpy(newInstruction->instructionName,instName);

    while((*charAt == ' ') || (*charAt == '\t')){
        charAt++;
    }//llega al parametro
    if(*charAt == '\n'){ //no hay parametros
        newInstruction->paramType = 0;
        //createAndSaveInstruction(instNumber,instName,NULL,0,lastFunction); // 1 para cadenas,2 enteros, 0 sin parametros
    }
    else if(!(*charAt >= 48 && *charAt <=57)){  //aca se capta cuando el parametro es una cadena, ya que no empieza con numero
        newInstruction->paramType = 1;
        //createAndSaveInstruction(instNumber,instName,instParam,1,lastFunction); // 1 para cadenas,2 enteros, 0 sin parametros
    }
    else{  // si no es cadena, es numero
        newInstruction->paramType = 2;
        //createAndSaveInstruction(instNumber,instName,instParam,2,lastFunction); // 1 para cadenas,2 enteros, 0 sin parametros
    }
    char instParam[50] = {};

    while(*charAt != 10){
        if(*charAt == 0){
            break;
        }
        instParam[paramCounter] = *charAt;
        charAt++;
        paramCounter++;
    }
    newInstruction->param = malloc(50);
    strcpy(newInstruction->param,instParam);
    insertInstruction(&newInstruction);
}

void parseLineAndSave(char * line){
    char * charAt = &line[0];

    if(*charAt >=65 && *charAt <= 122){ //Si vienen caracteres entonces es nombre de función
        extractFunction(charAt);
    }
    else if(*charAt >= 48 && *charAt <=57){ //Si es una instrucción, o sea, empieza con numero
        extractInstruction(charAt);
    }
}

void imprimeFunciones(){
    struct Function * current = firstFunction;
    while(current != NULL){
        printf("Nombre de funcion: %s\n",current->functionName);
        struct Parameter * params = current->parameterList;
        printf("Parametros: ");
        if(params != NULL){
            while(params != NULL){
                char * param = (char *) params->paramValue;
                printf("%s,",param);
                params = params->next;
            }
        } else{
            printf("Sin Parametros\n");
        }
        struct Instruction * instructions = current->functionInstructions;
        printf("\nInstrucciones: ");
        if(instructions != NULL){
            while(instructions != NULL){
                printf("Nombre: %s\n",instructions->instructionName);
                if(instructions->paramType == 1){
                    char* param = (char*)instructions->param;
                    printf("Parametro: %s\n",param);
                }
                else if(instructions->paramType == 2){
                    char * paramChar = (char*)(instructions->param);
                    int numero = atoi(paramChar);
                    printf("Parametro: %i\n",numero);
                }
                instructions = instructions->next;
            }
        } else{
            printf("Sin Instrucciones\n");
        }
        printf("------------------------------------------------------\n");
        current = current->next;
    }
}

void readFile(){
    FILE * file;
    char * line;

    file = fopen("C:\\Users\\Jose Herrera\\CLionProjects\\NuevoInterprete\\bytecodeFile.txt","r");
    if(file == NULL){
        printf("Error al abrir archivo, verifique la direccion.");
    }else{
        while (feof(file) == 0)
        {
            fgets(line,100,file);
            parseLineAndSave(line);
        }
    }
}

int main() {
    readFile();
    imprimeFunciones();
    return 0;
}