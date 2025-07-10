/*
 * @overview        {FileName}
 *
 * @version         2.0
 *
 * @author          Dyson Arley Parra Tilano <dysontilano@gmail.com>
 *
 * @copyright       Dyson Parra
 * @see             github.com/DysonParra
 *
 * History
 * @version 1.0     Implementation done.
 * @version 2.0     Documentation added.
 */
#include "flag_processor.h"
#include "flag.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>

CALLING String compareRequiredAndOptionalFlags(String requiredFlags[][20], String optionalFlags[][20]);
CALLING String convertArgsToFlagsArray(String args[], Flag** inputFlags);
CALLING String compareInputFlags(Flag** inputFlags, Flag** outputFlags, String acceptedFlags[][20], int required);
CALLING String validateSpecifiedFlags(Flag** inputFlags, Flag** outputFlags, String requiredFlags[][20], String optionalFlags[][20], int allowUnknownFlags);
CALLING int getArraySize(String* args);

#if (!defined STATIC_LIB_) && (!defined NO_DLL_MAIN_)
/**
 * Entrada principal de la dll (creado si no se indica que es biblioteca estática)
 *
 * @param instance identificador del módulo.
 * @param reason   razón de la llamada.
 * @param reserved argumento reservado.
 * @return {@code TRUE} si se completó exitosamente la llamada, caso contrario {@code FALSE}.
 */
BOOL APIENTRY DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved) {
    /*
     * Evalúa la razón de la llamada a la dll.
     */
    switch (reason) {
        /*
         * Se cargó la dll en el proceso actual.
         */
        case DLL_PROCESS_ATTACH:
            // printf("%s\n", "DLL_PROCESS_ATTACH");
            break;

        /*
         * Se liberó la dll en el proceso actual.
         */
        case DLL_PROCESS_DETACH:
            // printf("%s\n", "DLL_PROCESS_DETACH");
            break;

        /*
         * Se cargó la dll en un nuevo hilo.
         */
        case DLL_THREAD_ATTACH:
            // printf("%s\n", "DLL_THREAD_ATTACH");
            break;

        /*
         * Se liberó la dll en el nuevo hilo.
         */
        case DLL_THREAD_DETACH:
            // printf("%s\n", "DLL_THREAD_DETACH");
            break;
    }

    return TRUE;
}
#endif

/**
 * Muestra en consola el array indicado por {@code flags}
 *
 * @param flags     array de {@code Flag} que se va a imprimir en la consola.
 * @param printNull indica si se van a imprimir las flags con valor de {@code null}
 */
LANGUAGE DLLIMPORT CALLING void printFlagsArray(Flag** flags, int printNull) {
    printf("\nFlags START\n");
    if (flags != null) {
        Flag* flag;
        for_each_array(flag, flags) {
            if (flag != NULL_FLAG)
                printf("%s\n", flag_to_string(flag));
            else if (printNull)
                printf("(null)\n");
        }
    }
    printf("Flags END\n\n");
}

/**
 * Muestra en consola la matriz indicada por {@code flags}
 *
 * @param flags   array de {@code String} que se va a imprimir en la consola.
 * @param message mensaje que se mostrará antes de imprimir las flags.
 */
LANGUAGE DLLIMPORT CALLING void printFlagsMatrix(String flags[][20], String message) {
    printf("%s\n", message);
    String line = malloc((200 + 30) * (sizeof(String)));
    char **flag;
    for_each_array(flag, flags) {
        if (flag[0] == NULL)
            break;
        sprintf(line, "%s", flag[0]);
        String tmp;
        for_each_array_idx(valuesOrLength, tmp, flag);
        for (int i = 1; i < valuesOrLength; i++) {
            sprintf(line, "%s or %s", line, flag[i]);
        }
        printf("%s\n", line);
    }
    if (flags[0] == 0)
        printf("Not specified.\n");
    printf("\n");
}

/**
 * Compara si hay {@code String} que están tanto en la matriz {@code requiredFlags} como en la
 * matriz {@code optionalFlags}.
 *
 * @param requiredFlags una matriz con las flags requeridas; en cada fila se indican las flags y
 *                      en cada columna indica cuales flags son excluyentes (si se incluye la
 *                      flag de una columna no se pueden incluir las flags en las otras columnas
 *                      de esa fila) al ser requeridas se debe incluir una y solo una flag de
 *                      cada fila.
 * @param optionalFlags una matriz con las flags opcionales; en cada fila se indican las flags y
 *                      en cada columna indica cuales flags son excluyentes (si se incluye la
 *                      flag de una columna no se pueden incluir las flags en las otras columnas
 *                      de esa fila) al ser opcionales se pueden o no incluir una y solo una
 *                      flag de cada fila.
 * @return {@code null} si no hay flags repetidas, caso contrario {@code String} con un mensaje
 *         que dice cuales flags se repiten.
 */
CALLING String compareRequiredAndOptionalFlags(String requiredFlags[][20], String optionalFlags[][20]) {
    char **reqValuesOr, **optValuesOr;
    char *reqFlag, *optFlag;
    for_each_array(reqValuesOr, requiredFlags) {
        if (reqValuesOr[0] == NULL)
            break;
        for_each_array(reqFlag, reqValuesOr) {
            // printf("%s", reqFlag);
            for_each_array(optValuesOr, optionalFlags) {
                if (optValuesOr[0] == NULL)
                    break;
                for_each_array(optFlag, optValuesOr) {
                    // printf(" -> %s", optFlag);
                    if (!strcmp(reqFlag, optFlag)) {
                        String result = (String)calloc(70 + 30, sizeof(String));
                        sprintf(result, "Error: flag '%s' is defined as required flag and as optional flag.", reqFlag);
                        return result;
                    }
                }
            }
            // printf("\n");
        }
        // printf("\n");
    }
    return NULL;
}

/**
 * Analiza si el array {@code args} representa una secuencia válida de flags y en caso
 * afirmativo almacena en {@code inputFlags} el equivalente en {@code Flag} del array.
 *
 * @param args       un array de {@code String} que se va a procesar para verificar si es una
 *                   secuencia de flags válida.
 * @param inputFlags el array de {@code Flag} donde se va a almacenar el equivalente en
 *                   {@code Flag} del array {@code args}; se da por hecho que el array no es
 *                   {@code null} ya que si lo es se generará un {@code NullPointerException}
 * @return {@code null} si {@code inputFlags} representa una secuencia de flags válida, caso
 *         contrario {@code String} con un mensaje que indica porque no es una secuencia válida
 *         de flags.
 */
CALLING String convertArgsToFlagsArray(String args[], Flag** inputFlags) {

    String msg_Invalid_Flag = "Error: invalid flag ";
    String msg_Expect_Value_1 = "Error: expected a value for the flag ";
    String msg_Expect_Value_2 = " but found the flag ";
    String msg_Expect_Flag_1 = "Error: expected a flag or a flag withouth value after the value ";
    String msg_Expect_Flag_2 = " but found the value ";
    String msg_First_Arg_Not_Flag = "Error: expected a flag or a flag withouth value as first parameter but found the value ";
    String msg_Value_For_Flag_No_Value_1 = "Error: The flags started in '--' no need value, but found the value ";
    String msg_Value_For_Flag_No_Value_2 = " for the flag ";
    String msg_Last_Arg_Flag = "Error: Not found a value for the flag ";

#define VALUE 1
#define FLAG_NO_VALUE 2
#define FLAG 3
    Flag* myFlag;
    String result;
    int flagNumber = 0;
    String flag = "";
    String value;
    int oldState;
    int state = VALUE;
    for (int i = 0; i < 1000; i++) {
        if (args[i] == NULL || args[i][0] == 0)
            break;
        oldState = state;
        String arg = args[i];
        if (arg[0] == '-' && ((arg[1] == 0 || (arg[1] == '-' && (arg[2] == 0 || arg[2] == '-'))))) {
            result = malloc((30 + 30) * (sizeof(String)));
            sprintf(result, "%s'%s'", msg_Invalid_Flag, arg);
            return result;
        } else if (arg[0] == '-' && arg[1] == '-')
            state = FLAG_NO_VALUE;
        else if (arg[0] == '-')
            state = FLAG;
        else
            state = VALUE;

        switch (state) {
            case FLAG:
                switch (oldState) {
                    case FLAG:
                        result = malloc((200 + 30) * (sizeof(String)));
                        sprintf(result, "%s'%s'%s'%s'", msg_Expect_Value_1, args[i - 1], msg_Expect_Value_2, arg);
                        return result;
                    case VALUE:
                    case FLAG_NO_VALUE:
                        flag = arg;
                        break;
                }
                break;

            case VALUE:
                switch (oldState) {
                    case FLAG:
                        value = arg;
                        // inputFlags[flagNumber] = new Flag(flag, value, false);
                        myFlag = malloc(sizeof(Flag));
                        myFlag->name = flag;
                        myFlag->value = value;
                        myFlag->required = 0;
                        inputFlags[flagNumber] = myFlag;
                        flagNumber++;
                        break;
                    case VALUE:
                        if (i != 0) {
                            result = malloc((200 + 30) * (sizeof(String)));
                            sprintf(result, "%s'%s'%s'%s'", msg_Expect_Flag_1, args[i - 1], msg_Expect_Flag_2, arg);
                            return result;
                        } else {
                            result = malloc((200 + 30) * (sizeof(String)));
                            sprintf(result, "%s'%s'", msg_First_Arg_Not_Flag, arg);
                            return result;
                        }
                    case FLAG_NO_VALUE:
                        result = malloc((200 + 30) * (sizeof(String)));
                        sprintf(result, "%s'%s'%s'%s'", msg_Value_For_Flag_No_Value_1, arg, msg_Value_For_Flag_No_Value_2, args[i - 1]);
                        return result;
                }
                break;

            case FLAG_NO_VALUE:
                switch (oldState) {
                    case FLAG:
                        result = malloc((200 + 30) * (sizeof(String)));
                        sprintf(result, "%s'%s'%s'%s'", msg_Expect_Value_1, args[i - 1], msg_Expect_Value_2, arg);
                        return result;
                    case VALUE:
                    case FLAG_NO_VALUE:
                        flag = arg;
                        // inputFlags[flagNumber] = new Flag(flag, null, false);
                        myFlag = malloc(sizeof(Flag));
                        myFlag->name = flag;
                        myFlag->value = null;
                        myFlag->required = 0;
                        inputFlags[flagNumber] = myFlag;
                        flagNumber++;
                        break;
                }
                break;
        }
    }

    if (state == FLAG) {
        result = malloc((200 + 30) * (sizeof(String)));
        sprintf(result, "%s'%s'", msg_Last_Arg_Flag, flag);
        return result;
    }
    return null;
}

/**
 * Verifica si el array {@code inputFlags} contiene las flags indicadas por
 * {@code acceptedFlags} de tal modo que si {@code required} es {@code true} significa que las
 * flags son requeridas entonces {@code args} deben incluir todas las flags indicadas por
 * {@code acceptedFlags}, caso contrario si {@code required} es {@code false} {@code args} puede
 * incluir cero o más flags indicadas por {@code acceptedFlags}.
 *
 * @param inputFlags    es el array de {@code Flag} que se va a procesar.
 * @param outputFlags   es un array donde se almacenarán las flags indicada por
 *                      {@code acceptedFlags} que estén en {@code inputFlags}
 * @param acceptedFlags son las flags que se revisará que estén o no en {@code inputFlags}
 * @param required      indica si las flags indicadas por {@code acceptedFlags} son requeridas u
 *                      opcionales.
 * @return {@code null} si no ocurre ningún inconveniente al procesar {@code inputFlags}, caso
 *         contrario {@code String} con un mensaje que indica porque no es posible procesar
 *         {@code inputFlags}
 */
CALLING String compareInputFlags(Flag** inputFlags, Flag** outputFlags, String acceptedFlags[][20], int required) {
    int found;
    int flagNumber = 0;

    Flag* flag;
    for_each_array_idx(last, flag, outputFlags);
    flagNumber = last;
    for_each_array_idx(inputFlagsLength, flag, inputFlags);
    String errorMessage;
    char** reqValuesOr;
    char* reqFlag;
    for_each_array(reqValuesOr, acceptedFlags) {
        if (reqValuesOr[0] == NULL)
            break;
        found = 0;
        for_each_array(reqFlag, reqValuesOr) {
            // printf("%s", reqFlag);
            for (int i = 0; i < inputFlagsLength; i++) {
                if (inputFlags[i] != NULL_FLAG) {
                    // printf(" -> %s", inputFlags[i]->name);
                    if (!strcmp(reqFlag, inputFlags[i]->name)) {
                        if (!found) {
                            // printf("\nAdded %s in %d", inputFlags[i]->name, flagNumber);
                            outputFlags[flagNumber] = inputFlags[i];
                            if (required)
                                inputFlags[i]->required = 1;
                            flagNumber++;
                            inputFlags[i] = NULL_FLAG;
                            found = 1;
                            // printf("\n");
                            // printf("Again?");
                            for (int j = i; j < inputFlagsLength; j++) {
                                if (inputFlags[j] != NULL_FLAG) {
                                    // printf(" -> %s", inputFlags[j]->name);
                                    if (!strcmp(reqFlag, inputFlags[j]->name)) {
                                        // printf("\nFound again!\n");
                                        errorMessage = malloc((200 + 30) * (sizeof(String)));
                                        sprintf(errorMessage, "found the flag '%s' more that one times", reqFlag);
                                        return errorMessage;
                                    }
                                }
                            }
                            break;
                        } else {
                            // printf("\nFound invalid combination\n");
                            errorMessage = malloc((200 + 30) * (sizeof(String)));
                            sprintf(errorMessage, "Error: specified more that one of the flags");
                            String aux;
                            for_each_array(aux, reqValuesOr)
                                sprintf(errorMessage, "%s %s", errorMessage, aux);
                            return errorMessage;
                        }
                    }
                }
            }
            // printf("\n");
        }
        if (!found) {
            // printf("Not found\n");
            if (required) {
                errorMessage = malloc((200 + 30) * (sizeof(String)));
                sprintf(errorMessage, "Not found required flag %s", reqValuesOr[0]);
                String tmp;
                for_each_array_idx(reqValuesOrLength, tmp, reqValuesOr);
                for (int i = 1; i < reqValuesOrLength; i++)
                    sprintf(errorMessage, "%s or %s", errorMessage, reqValuesOr[i]);
                // printf("%s\n", errorMessage);
                return errorMessage;
            }
        }
        // printf("\n");
    }

    return null;
}

/**
 * Verifica el array {@code inputFlags} cuales de las flags indicadas por {@code requiredFlags}
 * y por {@code optionalFlags} contiene y si {@code allowUnknownFlags} es {@code true} acepta
 * que hayan flags que no estén ni en {@code requiredFlags} ni en {@code optionalFlags}.
 *
 * @param inputFlags        es el array de {@code Flag} que se va a procesar.
 * @param outputFlags       es un array donde se almacenarán las flags indicada por
 *                          {@code requiredFlags} y por {@code optionalFlags} que estén en
 *                          {@code inputFlags}
 * @param requiredFlags     una matriz con las flags requeridas; en cada fila se indican las
 *                          flags y en cada columna indica cuales flags son excluyentes (si se
 *                          incluye la flag de una columna no se pueden incluir las flags en las
 *                          otras columnas de esa fila) al ser requeridas se debe incluir una y
 *                          solo una flag de cada fila.
 * @param optionalFlags     una matriz con las flags opcionales; en cada fila se indican las
 *                          flags y en cada columna indica cuales flags son excluyentes (si se
 *                          incluye la flag de una columna no se pueden incluir las flags en las
 *                          otras columnas de esa fila) al ser opcionales se pueden o no incluir
 *                          una y solo una flag de cada fila.
 * @param allowUnknownFlags si {@code true} se aceptan flags que no estén en el array
 *                          {@code requiredFlags} ni en el array {@code optionalFlags}, caso
 *                          contrario si se encuentra una flag que no esté en los arrays
 *                          devuelve {@code String} con mensaje de error.
 * @return {@code null} si no ocurre ningún inconveniente al procesar {@code inputFlags}, caso
 *         contrario {@code String} con un mensaje que indica porque no es posible procesar
 *         {@code inputFlags}
 */
CALLING String validateSpecifiedFlags(Flag** inputFlags, Flag** outputFlags, String requiredFlags[][20], String optionalFlags[][20], int allowUnknownFlags) {
    String result;
    result = compareInputFlags(inputFlags, outputFlags, requiredFlags, 1);
    if (result == null) {
        result = compareInputFlags(inputFlags, outputFlags, optionalFlags, 0);
        if (result == null) {
            int flagQuantity = 0;
            Flag* outputFlag;
            for_each_array_idx(quantity, outputFlag, outputFlags);
            flagQuantity = quantity;

            Flag* inputFlag;
            for_each_array(inputFlag, inputFlags) {
                if (inputFlag != NULL_FLAG) {
                    if (!allowUnknownFlags) {
                        if (result == null) {
                            result = malloc((200 + 30) * (sizeof(String)));
                            sprintf(result, "Error: unknown flags found");
                        }
                        sprintf(result, "%s  %s", result, inputFlag->name);
                        // printf("%s\n", result);
                    } else {
                        outputFlags[flagQuantity] = inputFlag;
                        flagQuantity++;
                    }
                }
            }
        }
    }
    return result;
}

/**
 * Obtiene el tamaño de un array de {@code String} (char**).
 *
 * @param args array al que se le va a obtener el tamaño.
 * @return el tamaño del array.
 */
CALLING int getArraySize(String* args) {
    char* arg;
    for_each_array_idx(size, arg, args);
    return size;
}

/**
 * Analiza si el array {@code args} representa una secuencia válida de flags, verifica que las
 * flags requeridas {@code requiredFlags} estén todas especificadas allí, revisa que se incluyan
 * cero o más flags opcionales {@code optionalFlags} y si {@code allowUnknownFlags} es
 * {@code true} se aceptan flags que no sean opcionales ni requeridas caso afirmativo.
 *
 * @param args              un array de {@code String} que se va a procesar para verificar si es
 *                          una secuencia de flags válida.
 * @param requiredFlags     una matriz con las flags requeridas; en cada fila se indican las
 *                          flags y en cada columna indica cuales flags son excluyentes (si se
 *                          incluye la flag de una columna no se pueden incluir las flags en las
 *                          otras columnas de esa fila) al ser requeridas se debe incluir una y
 *                          solo una flag de cada fila.
 * @param optionalFlags     una matriz con las flags opcionales; en cada fila se indican las
 *                          flags y en cada columna indica cuales flags son excluyentes (si se
 *                          incluye la flag de una columna no se pueden incluir las flags en las
 *                          otras columnas de esa fila) al ser opcionales se pueden o no incluir
 *                          una y solo una flag de cada fila.
 * @param allowUnknownFlags si {@code true} se aceptan flags que no estén en el array
 *                          {@code requiredFlags} ni en el array {@code optionalFlags}, caso
 *                          contrario si se encuentra una flag que no esté en los arrays se
 *                          devuelve {@code null} y se mostrará mensaje de error.
 * @return array de {@code Flag} si se puede procesar {@code args} utilizando
 *         {@code requiredFlags} y {@code optionalFlags} sin ningún inconveniente, caso
 *         contrario {@code null} y se mostrará en consola porqué no fue posible procesar
 *         {@code args}.
 */
CALLING Flag** validateFlags(String* args, String requiredFlags[][20], String optionalFlags[][20], int allowUnknownFlags) {
    String result;
    int argsSize = getArraySize(args);
    Flag** inputFlags;
    Flag** outputFlags;

    result = compareRequiredAndOptionalFlags(requiredFlags, optionalFlags);
    if (result != null) {
        printf("%s\n", result);
        return null;
    }

    inputFlags = calloc(argsSize + 1, sizeof(Flag*));
    result = convertArgsToFlagsArray(args, inputFlags);
    if (result != null) {
        printf("%s\n", result);
        return null;
    }

    int flagQuantity = 0;
    Flag* flag;
    for_each_array(flag, inputFlags)
        flagQuantity++;
    outputFlags = calloc(flagQuantity + 1, sizeof(Flag*));

    result = validateSpecifiedFlags(inputFlags, outputFlags, requiredFlags, optionalFlags, allowUnknownFlags);
    if (result != null) {
        printf("%s\n", result);
        return null;
    }
    return outputFlags;
}

/**
 * Analiza si el array {@code args} representa una secuencia válida de flags, verifica que las
 * flags requeridas {@code requiredFlags} estén todas especificadas allí, revisa que se incluyan
 * cero o más flags opcionales {@code optionalFlags} y si {@code allowUnknownFlags} es
 * {@code true} se aceptan flags que no sean opcionales ni requeridas caso afirmativo.
 *
 * @param args              un array de {@code String} que se va a procesar para verificar si es
 *                          una secuencia de flags válida.
 * @param defaultArgs       un array de {@code String} que se va a procesar para verificar si es
 *                          una secuencia de flags válida en caso de {@code  args} se encuentre
 *                          vacío.
 * @param requiredFlags     una matriz con las flags requeridas; en cada fila se indican las
 *                          flags y en cada columna indica cuales flags son excluyentes (si se
 *                          incluye la flag de una columna no se pueden incluir las flags en las
 *                          otras columnas de esa fila) al ser requeridas se debe incluir una y
 *                          solo una flag de cada fila.
 * @param optionalFlags     una matriz con las flags opcionales; en cada fila se indican las
 *                          flags y en cada columna indica cuales flags son excluyentes (si se
 *                          incluye la flag de una columna no se pueden incluir las flags en las
 *                          otras columnas de esa fila) al ser opcionales se pueden o no incluir
 *                          una y solo una flag de cada fila.
 * @param allowUnknownFlags si {@code true} se aceptan flags que no estén en el array
 *                          {@code requiredFlags} ni en el array {@code optionalFlags}, caso
 *                          contrario si se encuentra una flag que no esté en los arrays se
 *                          devuelve {@code null} y se mostrará mensaje de error.
 * @return array de {@code Flag} si se puede procesar {@code args} utilizando
 *         {@code requiredFlags} y {@code optionalFlags} sin ningún inconveniente, caso
 *         contrario {@code null} y se mostrará en consola porqué no fue posible procesar
 *         {@code args}.
 */
LANGUAGE DLLIMPORT CALLING Flag** convertArgsToFlags(String* args, String* defaultArgs, String requiredFlags[][20], String optionalFlags[][20], int allowUnknownFlags) {
    Flag** flags;
    char* emptyFlags[][20] = {{0}};
    requiredFlags = requiredFlags != null ? requiredFlags : emptyFlags;
    optionalFlags = optionalFlags != null ? optionalFlags : emptyFlags;
    if ((defaultArgs == null || defaultArgs[0] == 0)
            && (args == null || args[0] == 0)) {
        printf("Flags and default flags not specified...\n");
        flags = null;
    } else if (args != null && args[0] != 0) {
        printf("Validating specified flags...\n");
        flags = validateFlags(args, requiredFlags, optionalFlags, allowUnknownFlags);
    } else {
        printf("No flags specified, validating default flags...\n");
        flags = validateFlags(defaultArgs, requiredFlags, optionalFlags, allowUnknownFlags);
    }

    if (flags == null) {
        printf("\n");
        printFlagsMatrix(requiredFlags, "Required flags:");
        printFlagsMatrix(optionalFlags, "Optional flags:");
        printf("Error in flags\n");
    } else
        printFlagsArray(flags, 1);

    return flags;
}
