import time
import subprocess
import os
import inspect
SCRIPT_PATH =  os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe()))) # script directory
os.chdir(SCRIPT_PATH)
FNULL = open(os.devnull, 'w')
def get_current_git_version():
    proc = subprocess.Popen(['git', 'rev-parse', 'HEAD'], stdout=subprocess.PIPE)
    proc.wait()
    return proc.stdout.readline().rstrip()

def pull_new_changes():
    proc = subprocess.Popen(['git', 'pull'], stdout=FNULL)
    proc.wait()

def run_experiments():
    proc = subprocess.Popen(['python3', os.path.join("experiments","automatic_benchmark","benchmark.py")], stdout=FNULL)
    proc.wait()

def save_experiments():
    proc = subprocess.Popen(['python3', os.path.join("experiments","automatic_benchmark","benchmark.py")], stdout=FNULL)
    proc.wait()

current_git_version = '0'

while True:
    if current_git_version != get_current_git_version():
        os.chdir(SCRIPT_PATH)
        current_git_version = get_current_git_version()
        run_experiments()
        os.rename(os.path.join("experiments","automatic_benchmark","config0"),os.path.join("experiments","automatic_benchmark",str(current_git_version)))
    time.sleep(60)