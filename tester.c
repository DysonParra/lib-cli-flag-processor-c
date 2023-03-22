/*
 * @fileoverview    {FileName}
 *
 * @version         2.0
 *
 * @author          Dyson Arley Parra Tilano <dysontilano@gmail.com>
 *
 * @copyright       Dyson Parra
 * @see             github.com/DysonParra
 *
 * History
 * @version 1.0     Implementación realizada.
 * @version 2.0     Documentación agregada.
 */
#include "flag.h"
#include "flag_processor.h"
#include "tester.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * Entrada principal del sistema.
 *
 * @param argc cantidad de argumentos pasados en la linea de comandos.
 * @param argv argumentos de la linea de comandos.
 * @return {0} si el programa se ejecutó correctamente.
 */
int main(int argc, char** argv) {

    argv = &argv[1];

    printf("\n...START...\n");

    char* requiredFlags[][20] = {
        {"-use", "--notUse", "--find"},
        {"-name", "--noName"},
        {"-encoder", "--noEncoder"},
        {0}};

    char* optionalFlags[][20] = {
        {"-flag", "--noFlag"},
        {"-test", "--noTest"},
        {0}};

    char* defaultArgs[] = {
        "--useDefault",
        "-uses",
        "MKV",
        "-noTest",
        "aac",
        "-use",
        "ffmpeg",
        "-name",
        "chromedriver.exe",
        "-test",
        "_urls.xml",
        "--aac",
        "--noEncoder", 0};

    /*-
    char* arg;
    for_each_array(arg, argv)
        printf("%s\n", argv[index-1]);
    printf("\n");
    */
    
    Flag** flags;
    flags = convertArgsToFlags(argv, defaultArgs, requiredFlags, optionalFlags, 1);

    if (flags == NULL) {
        printf("...ERROR IN FLAGS...");
        return 0;
    }

    printFlagsArray(flags, 1);
    printf("...END...");
    return 0;
}
