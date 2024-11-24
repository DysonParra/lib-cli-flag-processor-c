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
 * @version 1.0     Implementation done.
 * @version 2.0     Documentation added.
 */
#include "flag.h"
#include "flag_processor.h"
#include "tester.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * Ejecuta las pruebas de la aplicación.
 *
 * @param args argumentos de la linea de comandos.
 * @return {@code true} si se ejecutan las pruebas correctamente, {@code false} caso contrario.
 */
LANGUAGE DLLIMPORT CALLING int startTesting(int argc, char** argv) {
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

    Flag** flags = convertArgsToFlags(argv, defaultArgs, requiredFlags, optionalFlags, 1);
    if (flags != NULL)
        printf("Flags success processed\n");
    
    return flags != NULL;
}
