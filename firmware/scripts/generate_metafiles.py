import os
import subprocess

Import("env")

env.AddPostAction(
    "${PROG_PATH}",
    env.VerboseAction(" ".join([
        "avr-objdump", "-d", "${PROG_PATH}", ">", "${PROJECT_DIR}\\${PROGNAME}.asm",
    ]), "Generating ${PROJECT_DIR}\\${PROGNAME}.asm")
)
env.AddPostAction(
    "${PROG_PATH}",
    env.VerboseAction(" ".join([
        "scripts\\generate_cc.sh", "${PROJECT_DIR}\\${PROGNAME}.asm", "${PROJECT_DIR}\\${PROGNAME}.cycles"
    ]), "Generating ${PROJECT_DIR}\\${PROGNAME}.cycles")
)
