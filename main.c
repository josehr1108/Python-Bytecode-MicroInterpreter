#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//Estructura para funciones
struct Function{
    char * functionName;
    struct Instruction * functionInstructions;
    struct Parameter * parameterList;
    struct Function * next;
}*firstFunction,*lastFunction,*currentFunction;
//Estructura para los parametros de las funciones
struct Parameter{
    char * paramName;
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
struct DataWareHouse{
    char* name;
    int type;
    void * value;
    struct DataWareHouse * next;
}*dataHeader;
//Pila de datos
struct Stack{
    void * value;
    int type;
    struct Stack * next;
}*mainStack,*callsStack;

/**************************************  Funciones para las lineas del archivo  *************************************************/

void insertFunction(struct Function ** functionP){
    struct Function * function = *functionP;

    if(firstFunction == NULL){
        firstFunction = function;
        lastFunction = function;
        currentFunction = function;
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
        newParam->paramValue = NULL;
        newParam->paramName = malloc(strlen(paramHolder));
        strcpy(newParam->paramName,paramHolder);
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
    if((*charAt == '\n')||(*charAt == 0)){ //no hay parametros
        newInstruction->paramType = 0;
        newInstruction->param = NULL;
        insertInstruction(&newInstruction);
        return;
    } else if(!(*charAt >= 48 && *charAt <=57) || (*charAt == 34) || (*charAt == 39)){  //aca se capta cuando el parametro es una cadena, ya que no empieza con numero
        newInstruction->paramType = 1;
        if((*charAt == 34) || (*charAt == 39)){  //se brinca la primera comilla doble o comilla simple
            charAt++;
        }
    }
    else{  // si no es cadena, es numero
        newInstruction->paramType = 2;
    }
    char instParam[50] = {};

    while(*charAt != 10){
        if((*charAt == 0)||(*charAt == '/')||(*charAt == 32)||(*charAt == 34)||(*charAt == 39)||(*charAt == '\t')){ // {0 = '\0', 32 = string vacio, 34= '"'}
            if(instParam[1] == 0){ //si es de un solo caracter, sobreescribe el tipo
                newInstruction->paramType = 3;
            }
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

/**************************************  FUNCIONES USO GENERAL  *************************************************/
void imprimeFunciones(){
    struct Function * current = firstFunction;
    while(current != NULL){
        printf("Nombre de funcion: %s\n",current->functionName);
        struct Parameter * params = current->parameterList;
        printf("Parametros: ");
        if(params != NULL){
            while(params != NULL){
                printf("%s,",params->paramName);
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

void push(void * data,int type,struct Stack ** stackDestiny){
    struct Stack * nn = (struct Stack *) malloc(sizeof(struct Stack));

    nn->next = NULL;
    nn->type = type;
    if((type == 4) || (type == 5)){  //si es lista o funcion guarda la referencia
        nn->value = data;
    } else{
        nn->value = malloc(strlen(data));
        strcpy(nn->value,data);
    }

    if(*stackDestiny == NULL){
        *stackDestiny = nn;
    } else{    //insercion al inicio
        nn->next = *stackDestiny;
        *stackDestiny = nn;
    }
}

struct Stack * pop(struct Stack ** stack){
    struct Stack * temp = *stack;
    struct Stack * second = temp->next;

    temp->next = NULL;
    *stack = second;

    return temp;
}

struct Parameter * createNewParamList(int amountOfParams){
    struct Parameter * tempParamList = (struct Parameter *)malloc(sizeof(struct Parameter));
    tempParamList->paramName = NULL;
    tempParamList->next = NULL;
    tempParamList->paramValue = NULL;

    struct Stack * node = pop(&mainStack);
    if(node->type == 4){
        tempParamList->paramValue = node->value;
        tempParamList->paramType = 4;
    } else{
        tempParamList->paramValue = malloc(strlen(node->value));
        strcpy(tempParamList->paramValue,node->value);
        tempParamList->paramType = node->type;
    }
    amountOfParams--;

    while (amountOfParams != 0){
        struct Parameter * newParam = (struct Parameter *)malloc(sizeof(struct Parameter));
        newParam->next = NULL;
        newParam->paramName = NULL;

        struct Stack * newNode = pop(&mainStack);
        if(newNode->type == 4){
            tempParamList->paramValue = newNode->value;
            tempParamList->paramType = 4;
        } else{
            tempParamList->paramValue = malloc(strlen(newNode->value));
            strcpy(tempParamList->paramValue,newNode->value);
            tempParamList->paramType = newNode->type;
        }
        struct Parameter * copyParamList = tempParamList;
        while(copyParamList->next != NULL){
            copyParamList = copyParamList->next;
        }
        copyParamList->next = newParam;
        amountOfParams--;
    }
    return tempParamList;
}

void pruebaLista(){
    struct Stack * nodoStack = pop(&mainStack);
    if(nodoStack->type == 3){
        struct List * lista = (struct List *)nodoStack->value;
        struct List * siguiente = lista->next;
    }
}

/**************************************  FUNCIONES BYTECODE  *************************************************/
struct DataWareHouse * getVarByName(char * nameP){
    struct DataWareHouse * tempVar = dataHeader;
    while (tempVar != NULL){
        if(strcmp(tempVar->name,nameP) == 0){
            return tempVar;
        }
        tempVar = tempVar->next;
    }
    return NULL;
}

struct Function * getFunctionByName(char * nameP){
    struct Function * tempVar = firstFunction;
    while (tempVar != NULL){
        if(strcmp(tempVar->functionName,nameP) == 0){
            return tempVar;
        }
        tempVar = tempVar->next;
    }
    return NULL;
}

void loadGlobal(char * functionNameP){
    struct Function * function = getFunctionByName(functionNameP);
    currentFunction = function;
    push(function,5,&mainStack);
}

void buildList(int elementsAmount){
    struct List * list = (struct List *) malloc(sizeof(struct List));
    struct Stack * node = pop(&mainStack);
    list->next = NULL;
    list->element = malloc(sizeof(node->value));
    strcpy(list->element,node->value);
    list->elementType = node->type;

    while(elementsAmount != 1){
        struct Stack * nextNode = pop(&mainStack);
        struct List * listItem = (struct List *) malloc(sizeof(struct List));
        listItem->next = NULL;
        listItem->element = malloc(sizeof(nextNode->value));
        strcpy(listItem->element,nextNode->value);
        listItem->elementType = nextNode->type;

        struct List * temp = list;
        while(temp->next != NULL){
            temp = temp->next;
        }
        temp->next = listItem;

        elementsAmount--;
    }
    push(list,4,&mainStack);
}

void binarySubstract(){
    struct Stack * first = pop(&mainStack);
    struct Stack * second = pop(&mainStack);

    char * firstOperandChar = (char*)first->value;
    char * secondOperandChar = (char*)second->value;

    int firstOperand = 0;
    int secondOperand = 0;

    if(first->type == 1){
        struct DataWareHouse * tempVar = getVarByName(firstOperandChar);
        char * varValueChar = (char *)tempVar->value;
        firstOperand = atoi(varValueChar);
    } else{
        firstOperand = atoi(firstOperandChar);
    }

    if(second->type == 1){
        struct DataWareHouse * tempVar2 = getVarByName(secondOperandChar);
        char * varValueChar2 = (char *)tempVar2->value;
        secondOperand = atoi(varValueChar2);
    } else{
        secondOperand = atoi(secondOperandChar);
    }

    int substract = firstOperand - secondOperand;
    char * substractToChar;
    itoa(substract,substractToChar,10);
    push(substractToChar,2,&mainStack);

    free(first);
    free(second);
}

void binaryAdd(){
    struct Stack * first = pop(&mainStack);
    struct Stack * second = pop(&mainStack);

    char * firstOperandChar = (char*)first->value;
    char * secondOperandChar = (char*)second->value;

    int firstOperand = 0;
    int secondOperand = 0;

    if(first->type == 1){
        struct DataWareHouse * tempVar = getVarByName(firstOperandChar);
        char * varValueChar = (char *)tempVar->value;
        firstOperand = atoi(varValueChar);
    } else{
        firstOperand = atoi(firstOperandChar);
    }

    if(second->type == 1){
        struct DataWareHouse * tempVar2 = getVarByName(secondOperandChar);
        char * varValueChar2 = (char *)tempVar2->value;
        secondOperand = atoi(varValueChar2);
    } else{
        secondOperand = atoi(secondOperandChar);
    }

    int sum = firstOperand + secondOperand;
    char * sumToChar;
    itoa(sum,sumToChar,10);
    push(sumToChar,2,&mainStack);

    free(first);
    free(second);
}

void binaryMultiply(){
    struct Stack * first = pop(&mainStack);
    struct Stack * second = pop(&mainStack);

    char * firstOperandChar = (char*)first->value;
    char * secondOperandChar = (char*)second->value;

    int firstOperand = 0;
    int secondOperand = 0;

    if(first->type == 1){
        struct DataWareHouse * tempVar = getVarByName(firstOperandChar);
        char * varValueChar = (char *)tempVar->value;
        firstOperand = atoi(varValueChar);
    } else{
        firstOperand = atoi(firstOperandChar);
    }

    if(second->type == 1){
        struct DataWareHouse * tempVar2 = getVarByName(secondOperandChar);
        char * varValueChar2 = (char *)tempVar2->value;
        secondOperand = atoi(varValueChar2);
    } else{
        secondOperand = atoi(secondOperandChar);
    }

    int mult = firstOperand * secondOperand;
    char * multToChar;
    itoa(mult,multToChar,10);
    push(multToChar,2,&mainStack);

    free(first);
    free(second);
}

void binaryDivide(){
    struct Stack * first = pop(&mainStack);
    struct Stack * second = pop(&mainStack);

    char * firstOperandChar = (char*)first->value;
    char * secondOperandChar = (char*)second->value;

    int firstOperand = 0;
    int secondOperand = 0;

    if(first->type == 1){
        struct DataWareHouse * tempVar = getVarByName(firstOperandChar);
        char * varValueChar = (char *)tempVar->value;
        firstOperand = atoi(varValueChar);
    } else{
        firstOperand = atoi(firstOperandChar);
    }

    if(second->type == 1){
        struct DataWareHouse * tempVar2 = getVarByName(secondOperandChar);
        char * varValueChar2 = (char *)tempVar2->value;
        secondOperand = atoi(varValueChar2);
    } else{
        secondOperand = atoi(secondOperandChar);
    }

    int divide = firstOperand / secondOperand;
    char * divideToChar;
    itoa(divide,divideToChar,10);
    push(divideToChar,2,&mainStack);

    free(first);
    free(second);
}

void loadFast(char * varName){
    struct DataWareHouse * currentVariable = dataHeader;
    while (currentVariable != NULL){
        if(strcmp(currentVariable->name,varName) == 0){
            push(currentVariable->value,currentVariable->type,&mainStack);
            return;
        }
        currentVariable = currentVariable->next;
    }
}

void storeFast(char * name){
    struct Stack * node = pop(&mainStack);
    struct DataWareHouse * variable = (struct DataWareHouse *) malloc(sizeof(struct DataWareHouse));
    variable->next = NULL;
    variable->type = node->type;
    variable->name = malloc(strlen(name));
    strcpy(variable->name, name);
    if(variable->type == 4){
        variable->value = node;
    }else {
        char * valueAsCharArray = (char *) node->value;
        variable->value = malloc(sizeof(node->value));
        strcpy(variable->value, node->value);
    }

    if(dataHeader == NULL){
        dataHeader = variable;
    } else{       //insercion al inicio
        variable->next = dataHeader;
        dataHeader = variable;
    }
}

void callFunction(int amountOfParams){
    if(amountOfParams > 0){
        struct Parameter * newParamList = createNewParamList(amountOfParams);
        struct Stack * functionStackNode = pop(&mainStack);
        struct Function * function = (struct Function *)functionStackNode->value;

        struct Parameter * tempNew = newParamList;
        struct Parameter * currentFunctParams = function->parameterList;
        while(currentFunctParams != NULL){
            tempNew->paramName = malloc(strlen(currentFunctParams->paramName));
            strcpy(tempNew->paramName,currentFunctParams->paramName);
            currentFunctParams = currentFunctParams->next;
            tempNew = tempNew->next;
        }
        currentFunction->parameterList = newParamList; //setea a null
    }
}

void saveCallBreakpoint(int lineNumber){
    char * numChar;
    itoa(lineNumber,numChar,10);
    push(numChar,7,&callsStack);
}

void readByteCode(){
    struct Instruction * instructions = currentFunction->functionInstructions;
    while(instructions != NULL){
        if(strcmp(instructions->instructionName,"LOAD_CONST") == 0){
            push(instructions->param,instructions->paramType,&mainStack);
        } else if(strcmp(instructions->instructionName,"BUILD_LIST") == 0){
            char * numberText = (char *)instructions->param;
            int number = atoi(numberText);
            buildList(number);
        } else if(strcmp(instructions->instructionName,"STORE_FAST") == 0) {
            char *nameParam = (char *) instructions->param;
            storeFast(nameParam);
        } else if(strcmp(instructions->instructionName,"LOAD_FAST") == 0) {
            char * varName = (char *)instructions->param;
            loadFast(varName);
        } else if(strcmp(instructions->instructionName,"LOAD_GLOBAL") == 0) {
            char * param = (char *)instructions->param;
            loadGlobal(param);
        } else if(strcmp(instructions->instructionName,"BINARY_SUBSTRACT") == 0) {
            binarySubstract();
        } else if(strcmp(instructions->instructionName,"BINARY_ADD") == 0) {
            binaryAdd();
        } else if(strcmp(instructions->instructionName,"BINARY_MULTIPLY") == 0) {
            binaryMultiply();
        } else if(strcmp(instructions->instructionName,"BINARY_DIVIDE") == 0) {
            binaryDivide();
        } else if(strcmp(instructions->instructionName,"CALL_FUNCTION") == 0){
            char * param = (char *)instructions->param;
            int instructionNum = instructions->instructionNum;
            saveCallBreakpoint(instructionNum);
            int  amountOfParams = atoi(param);
            callFunction(amountOfParams);
            readByteCode();
        }
        instructions = instructions->next;
    }
}

void readFile(){
    FILE * file;
    char * line;

    file = fopen("C:\\Users\\Jose Herrera\\CLionProjects\\NuevoInterprete\\bytecodeFileOriginal.txt","r");
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
    readByteCode();
    return 0;
}

//Tipos de datos: string:1 - int:2 - char:3 - lista:4 - funcion:5 - numInstruccionLLamada: 7(int)