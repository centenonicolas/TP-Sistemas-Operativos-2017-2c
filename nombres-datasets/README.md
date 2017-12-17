# SO-Nombres-Dataset
Ejemplo de uso de transoformador y Reductor para contar nombres sobre Datasets de www.datos.gob.ar


#Modo de Uso

    cat nombres.csv | ./transformador.py | sort | ./reductor.py > CantidadPersonas.txt
    cat nombres.csv | ./transformador_Iniciales.py | sort | ./reductor.py > CantidadPersonas_Iniciales.txt
    cat nombres.csv | ./transformador_Anuales.py | sort | ./reductor.py > CantidadPersonas_Anuales.txt


 #Observaciones

 En algunos casos los transformadores y los reductores pueden fallar por falta de permisos. En ese caso ejecutar previamente chmod 777 *.py