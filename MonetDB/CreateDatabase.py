import os
import urllib2
import inspect
import subprocess
import time

MAIN_PATH = '/export/scratch1/home/holanda/'
SCRIPT_PATH = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe()))) # script directory

parameters = '--enable-optimize --disable-debug --disable-assert --with-rubygem-dir=/export/scratch1/home/holanda/.gem/ruby --disable-jaql'
create_scripts = ['schema.sql','load.sql']
query_scripts = ['query.sql']

def Download_Compile():
	# print("Downloading MonetDB Source Code...")
	# monetdb_tarball_url = 'https://www.monetdb.org/downloads/sources/Mar2018/MonetDB-11.29.3.tar.bz2'
	# monetdb_code = urllib2.urlopen(monetdb_tarball_url).read()
	# f = open(MAIN_PATH+'monetdb.tar.bz2', 'w+')
	# f.write(monetdb_code)
	# f.close()

	# print("Building MonetDB...")
	# os.chdir(MAIN_PATH)

	# if os.system('tar xvf monetdb.tar.bz2') != 0:
	# 	print("Failed to build MonetDB")
	# 	exit()

	# if os.system('mv MonetDB-11.29.3 monetdb') != 0:
	# 	print("Failed to rename MonetDB folder")
	# 	exit()

	os.chdir(os.path.join(MAIN_PATH,'monetdb'))
	# if os.system('./bootstrap') != 0:
	# 	print("Failed to bootstrap MonetDB")
	# 	exit()

	monetdb_install_dir = os.path.join(MAIN_PATH,'monetdb','Install')
	os.system('rm -r -f %s'% monetdb_install_dir)
	os.system('mkdir %s'% monetdb_install_dir)

	if os.system('./configure %s --prefix="%s"' % (parameters, monetdb_install_dir)) != 0:
		print("Failed to configure MonetDB")
		exit()
	if os.system('make -j4') != 0:
		print("Failed to make MonetDB")
		exit()

	if os.system('make install') != 0:
		print("Failed to make install MonetDB")
		exit()

def LoadData():
	print("Loading Data...")
	monetdb_install_dir = os.path.join(MAIN_PATH,'monetdb','Install')
	client = os.path.join(monetdb_install_dir, 'bin', 'mclient')
	server = os.path.join(monetdb_install_dir, 'bin', 'mserver5')
	if os.system('ls $HOME/.monetdb') != 0:
		f = open('%s/.monetdb' % os.environ['HOME'], 'w+')
		f.write('user=monetdb\npassword=monetdb')
		f.close()
	proc = subprocess.Popen([server, '--daemon=no'])
	while os.system('%s -s "SELECT 1"' % client) != 0:
		time.sleep(1)
	for script in create_scripts:
		script = os.path.join(SCRIPT_PATH, script)
		os.system('%s "%s"' % (client, script))

	proc.kill()

def RunQueries():
	print("Run Queries...")
	monetdb_install_dir = os.path.join(MAIN_PATH,'monetdb','Install')
	client = os.path.join(monetdb_install_dir, 'bin', 'mclient')
	server = os.path.join(monetdb_install_dir, 'bin', 'mserver5')
	if os.system('ls $HOME/.monetdb') != 0:
		f = open('%s/.monetdb' % os.environ['HOME'], 'w+')
		f.write('user=monetdb\npassword=monetdb')
		f.close()
	proc = subprocess.Popen([server, '--daemon=no','--set', 'gdk_nr_threads=1'])
	while os.system('%s -s "SELECT 1"' % client) != 0:
		time.sleep(1)
	os.system('%s -s "set optimizer=\'sequential_pipe\';"' % client)
	start_time_total = time.time()
	for script in query_scripts:
		script = os.path.join(SCRIPT_PATH, script)
		os.system('%s "%s"' % (client, script))
	end_time_total = time.time()
	total_time = end_time_total - start_time_total
	print "Total Loading Time: " + str(total_time)

	proc.kill()

Download_Compile()
LoadData()
RunQueries()