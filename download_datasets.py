import os
import inspect
import urllib.request
import zipfile

SCRIPT_PATH = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))  # script directory

DATA_FOLDER = os.path.join(SCRIPT_PATH, 'data')

if not os.path.isdir(DATA_FOLDER):
    os.makedirs(DATA_FOLDER)
    print("Downloading Power and Genomic")
    urllib.request.urlretrieve("https://zenodo.org/record/3835521/files/mdrq-analysis.zip?download=1",
                               os.path.join(DATA_FOLDER, "mdrq-analysis.zip"))
    print("Downloading SkyServer")
    urllib.request.urlretrieve("https://zenodo.org/record/3835487/files/skyserver_2.csv?download=1",
                               os.path.join(DATA_FOLDER, "skyserver_2.csv"))
    urllib.request.urlretrieve("https://zenodo.org/record/3835562/files/skyserver2_query.csv?download=1",
                               os.path.join(DATA_FOLDER, "skyserver2_query.csv"))
    print("Extracting Power and Genomic")
    with zipfile.ZipFile(os.path.join(DATA_FOLDER, 'mdrq-analysis.zip'), 'r') as zip_ref:
         zip_ref.extractall(DATA_FOLDER)
