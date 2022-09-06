/*
 * @fileoverview {FileName} se encarga de realizar tareas especificas.
 *
 * @version             1.0
 *
 * @author              Dyson Arley Parra Tilano <dysontilano@gmail.com>
 * Copyright (C) Dyson Parra
 *
 * @History v1.0 --- La implementacion de {FileName} fue realizada el 31/07/2022.
 * @Dev - La primera version de {FileName} fue escrita por Dyson A. Parra T.
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

    char* argsAux[] = {
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
    String arg;
    for_each_array(arg, argsAux)
        printf("%s\n", argsAux[index]);
    */
    Flag** flags = validateFlags(argsAux, requiredFlags, optionalFlags, 1);
    if (flags == NULL) {
        printf("...ERROR IN FLAGS...");
        return 0;
    }

    printFlagsArray(flags, 1);
    printf("...END...");
    return 0;
}
