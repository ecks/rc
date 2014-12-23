import subprocess
import os

subprocess.check_call(['python3', 'net_rt/rt_gen.py', '-n', '64', '-c', 'sf', '-o', 'routes1.csv'])
subprocess.check_call(['python3', 'net_rt/rt_dup.py', '-d' '0.25'])
out = subprocess.check_output(['./rc'])
print('SF <one entry in data different>: ',out)

subprocess.check_call(['python3', 'net_rt/rt_dup.py', '-d' '0.50'])
out = subprocess.check_output(['./rc'])
print('SF <two entries in data different>: ',out)

subprocess.check_call(['python3', 'net_rt/rt_dup.py', '-d' '0.75'])
out = subprocess.check_output(['./rc'])
print('SF <three entries in data different>: ',out)

subprocess.check_call(['python3', 'net_rt/rt_dup.py', '-d', '1.0'])
out = subprocess.check_output(['./rc'])
print('SF <all entries in data different>: ',out)

subprocess.check_call(['python3', 'net_rt/rt_dup.py', '-p', '0.25'])
out = subprocess.check_output(['./rc'])
print('SF <one prefix different>:',out)

subprocess.check_call(['python3', 'net_rt/rt_dup.py', '-p', '0.50'])
out = subprocess.check_output(['./rc'])
print('SF <two prefixes different>:',out)

subprocess.check_call(['python3', 'net_rt/rt_dup.py', '-p', '0.50'])
out = subprocess.check_output(['./rc'])
print('SF <three prefixes different>:',out)

subprocess.check_call(['python3', 'net_rt/rt_dup.py', '-p', '1.0'])
out = subprocess.check_output(['./rc'])
print('SF <all prefixes different>:',out)

subprocess.check_call(['python3', 'net_rt/rt_dup.py', '-s', '0.25'])
out = subprocess.check_output(['./rc'])
print('SF <one netmask different>:',out)

subprocess.check_call(['python3', 'net_rt/rt_dup.py', '-s', '0.50'])
out = subprocess.check_output(['./rc'])
print('SF <two netmasks different>:',out)

subprocess.check_call(['python3', 'net_rt/rt_dup.py', '-s', '0.75'])
out = subprocess.check_output(['./rc'])
print('SF <three netmasks different>:',out)

subprocess.check_call(['python3', 'net_rt/rt_dup.py', '-s', '1.0'])
out = subprocess.check_output(['./rc'])
print('SF <all netmasks different>:',out)


subprocess.check_call(['python3', 'net_rt/rt_gen.py', '-n', '64', '-c', 'ny', '-o', 'routes1.csv'])
subprocess.check_call(['python3', 'net_rt/rt_dup.py', '-d' '0.25'])
out = subprocess.check_output(['./rc'])
print('NY <one entry in data different>: ',out)

subprocess.check_call(['python3', 'net_rt/rt_dup.py', '-d' '0.50'])
out = subprocess.check_output(['./rc'])
print('NY <two entries in data different>: ',out)

subprocess.check_call(['python3', 'net_rt/rt_dup.py', '-d' '0.75'])
out = subprocess.check_output(['./rc'])
print('NY <three entries in data different>: ',out)

subprocess.check_call(['python3', 'net_rt/rt_dup.py', '-d', '1.0'])
out = subprocess.check_output(['./rc'])
print('NY <all entries in data different>: ',out)

subprocess.check_call(['python3', 'net_rt/rt_dup.py', '-p', '0.25'])
out = subprocess.check_output(['./rc'])
print('NY <one prefix different>:',out)

subprocess.check_call(['python3', 'net_rt/rt_dup.py', '-p', '0.50'])
out = subprocess.check_output(['./rc'])
print('NY <two prefixes different>:',out)

subprocess.check_call(['python3', 'net_rt/rt_dup.py', '-p', '0.75'])
out = subprocess.check_output(['./rc'])
print('NY <three prefixes different>:',out)

subprocess.check_call(['python3', 'net_rt/rt_dup.py', '-p', '1.0'])
out = subprocess.check_output(['./rc'])
print('NY <all prefixes different>:',out)

subprocess.check_call(['python3', 'net_rt/rt_dup.py', '-s', '0.25'])
out = subprocess.check_output(['./rc'])
print('NY <one netmask different>:',out)

subprocess.check_call(['python3', 'net_rt/rt_dup.py', '-s', '0.50'])
out = subprocess.check_output(['./rc'])
print('NY <two netmasks different>:',out)

subprocess.check_call(['python3', 'net_rt/rt_dup.py', '-s', '0.75'])
out = subprocess.check_output(['./rc'])
print('NY <three netmask different>:',out)

subprocess.check_call(['python3', 'net_rt/rt_dup.py', '-s', '1.0'])
out = subprocess.check_output(['./rc'])
print('NY <all netmasks different>:',out)
