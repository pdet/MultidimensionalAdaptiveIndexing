'''Runs all experiments
'''
import os


def main():
    '''Main
    '''
    # Get the list of experiments
    experiment_directories = list(os.walk('.'))[0][1]

    for experiment in experiment_directories:
        # Run only the ones with run.py inside of it
        if 'run.py' in os.listdir(experiment):
            os.system(f"python3 {experiment}/run.py --generate")


if __name__ == "__main__":
    main()
