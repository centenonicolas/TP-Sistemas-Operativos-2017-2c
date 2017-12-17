#! /usr/bin/python2
import sys

acumulator = 0
old_key = None

for token in sys.stdin.readlines():

    if(len(token.split(',')[0]) > 0):
        word = token.split(',')[0]
        try:
            repetitions = int(token.split(',')[1])
        except Exception:
            repetitions = 0

        if old_key is None:
             old_key = word

        if old_key != word:
            sys.stdout.write(old_key + "," + str(acumulator) + "\n")
            acumulator = 0
            old_key = word

        acumulator += repetitions

sys.stdout.write(old_key + "," + str(acumulator) + "\n")