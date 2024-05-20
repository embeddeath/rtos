
import argparse
import subprocess

parser = argparse.ArgumentParser(
    prog = 'shell',
    description = 'This is a helper program to compile the project.',
)


parser.add_argument(
    'command', 
    choices = ['build', 'flash'], 
    help = '1.- [build] = Builds the software.\
            2.- [flash] = Flash the software to the board.',
    )

parser.add_argument(
    '-r',
    '--release',
    action = 'store_true', 
)

parser.add_argument(
    '-d',
    '--debug',
    action = 'store_true', 
)

parser.add_argument(
    '-c', 
    '--clean', 
    action = 'store_true',
)

args = parser.parse_args()


if (args.command == 'build' and args.debug == True):
    cmake_command = ['cmake', '--preset', 'Debug']
    ninja_command = ['ninja']
    ninja_cwd = './build/Debug'
    subprocess.call(cmake_command)
    subprocess.call(ninja_command, cwd = ninja_cwd)

elif (args.command == 'build' and args.release == True): 
    cmake_command = ['cmake', '--preset', 'Release']
    ninja_cwd = "./build/Release"
    ninja_command = ['ninja']
    subprocess.call(cmake_command)
    subprocess.call(ninja_command, cwd = ninja_cwd)

elif (args.command == 'flash' and args.release == True): 
    openocd_command = ['openocd', '-f', 'board/st_nucleo_g0.cfg', '-c', ' program template.elf verify reset ', '-c', "shutdown"]
    openocd_cwd = "./build/Release"
    subprocess.call(openocd_command, cwd = openocd_cwd)

elif (args.command == 'flash' and args.debug == True): 
    openocd_command = ['openocd', '-f', 'board/st_nucleo_g0.cfg', '-c', ' program template.elf verify reset ', '-c', "shutdown"]
    openocd_cwd = "./build/Debug"
    subprocess.call(openocd_command, cwd = openocd_cwd)

    


    



