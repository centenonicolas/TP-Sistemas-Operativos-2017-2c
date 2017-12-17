#! /usr/bin/python2
import sys

def getRegistro(linea):
	if len(linea) > 0:
         if(len(linea.split(',')[0]) > 0 and len(linea.split(',')[1]) > 0):
             try:
                 inicial = linea.replace(' ', '').split(',')[0][:1]
                 registro = inicial + "," + linea.replace(' ', '').split(',')[1]+ "," + linea.replace(' ', '').split(',')[2] + "\n"
                 sys.stdout.write(registro)
             except Exception:
                 pass

contenido = sys.stdin.read()
data = contenido.split('\n')
map(getRegistro, data)