import os
import configuration as cfg
import subprocess
from multiprocessing.pool import ThreadPool

ROCKS_MACHINES = cfg.ROCKS_MACHINES
PATH_ON_ROCKS = cfg.PATH_ON_ROCKS
EXPERIMENTS = cfg.EXPERIMENTS
LOCAL_PATH = cfg.LOCAL_EXPERIMENTS_FOLDER

def RunExperiments():
    os.chdir('../../')
    def run_for_machine(machine):
        print(machine)
        os.system('ssh '+machine+ ' mkdir -p ' + PATH_ON_ROCKS)
        os.system('scp -r MultidimensionalAdaptiveIndexing/ '+ machine + ':' +PATH_ON_ROCKS)
        command = 'ssh '+machine+' python '+PATH_ON_ROCKS+'MultidimensionalAdaptiveIndexing/experiments/generatedscripts/'+ machine+'_experiment_' + str(EXPERIMENTS) + '.py'
        subprocess.Popen(command.split())
    print(ROCKS_MACHINES)
    p = ThreadPool(len(ROCKS_MACHINES))
    p.map(run_for_machine, ROCKS_MACHINES)

def CleanMachines():
    for machine in ROCKS_MACHINES:
        os.system('ssh '+machine+ ' rm -r -f ' + PATH_ON_ROCKS)

def KillAll():
    processes = ['gendata', 'crackingmain', 'python']
    for machine in ROCKS_MACHINES:        
        for process in processes:
            os.system('ssh '+ machine +' killall ' + process)

def CloneResults():
    if os.path.exists(LOCAL_PATH) !=1:
        os.system('mkdir -p '+LOCAL_PATH)
    for machine in ROCKS_MACHINES:
        os.system('scp -r '+ machine + ':' +PATH_ON_ROCKS + 'experimentresults '+ LOCAL_PATH)

KillAll()
CleanMachines()
RunExperiments()
# CloneResults()