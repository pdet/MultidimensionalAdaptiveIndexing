import os 
import inspect
MAIN_PATH =  os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe()))) # script directory

import sys 
sys.path.append(MAIN_PATH+ "/experiments")
 
import GenerateExperiments as generate
generate.run()
sys.path.append(MAIN_PATH+ "/experiments/generatedscripts") 

import experiment_1

experiment_1.run()
