#! /usr/bin/python2
import sys

def getRegistro(linea):
	if len(linea) > 0:
         if(len(linea.split(',')[0]) > 0 and len(linea.split(',')[1]) > 0):
             try:
                 registro = linea.split(',')[2] + "," + linea.split(',')[1] + "\n"
                 sys.stdout.write(registro)
             except Exception:
                 pass


contenido = sys.stdin.read()
data = contenido.split('\n')
map(getRegistro, data)