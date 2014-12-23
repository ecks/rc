import sys
import random
import argparse

def err_age(line):
  d_list = []
  d_list = line.split(',')
  age = int(d_list[3])
  age = age + random.randint(1,10)
  d_list[3] = str(age)
  line = ','.join(d_list)
  return line

def err_prefix(line):
  d_list = []
  octet = 3
  d_list = line.split(',')
  prefix = (d_list[0].split('.'))
  mask = (d_list[1].split('.'))
  mask_lsd = int(mask[octet])
  prefix_lsd = int(prefix[octet])
  prefix_len = 0

  while(prefix_lsd >= mask_lsd):
    # we need to go to the next octet
    octet = octet - 1
    mask_lsd = int(mask[octet])
    prefix_lsd = int(prefix[octet])

  if(prefix_lsd < mask_lsd):
    prefix_len = prefix_length(mask_lsd)
    small_prefix = int(smallest_prefix(prefix_len), 2)
    prefix_lsd = small_prefix*random.randint(1,(2 ** prefix_len)-1)

  prefix[octet] = str(prefix_lsd)
  d_list[0] = '.'.join(prefix)
  line = ','.join(d_list)
#  age = age + random.randint(1,10)
#  d_list[3] = str(age)
#  line = ','.join(d_list)
  return line

def err_mask(line):
  d_list = []
  d_list = line.split(',')
  mask = (d_list[1].split('.'))
  mask_lsd = int(mask[3])
  mask_lsd = random.randint(1,255)
  mask[3] = str(mask_lsd)
  d_list[1] = '.'.join(mask)
  line = ','.join(d_list)
  return line
   
def prefix_length(mask_lsd):
  mask_lsd_bin = bin(mask_lsd)
  i = 0
  prefix_len = 0
  end_of_str = False
  start_counting = False
  while(not end_of_str):
    if(mask_lsd_bin[i] == 'b'):
      start_counting = True
    if(start_counting and mask_lsd_bin[i] == '1'):
      prefix_len = prefix_len + 1
    if(start_counting and (mask_lsd_bin[i] == '0' or i+1 == len(mask_lsd_bin))):
      end_of_str = True
    i = i + 1
  return prefix_len

def smallest_prefix(prefix_len):
  prefix = '0b'
  for i in range(0,8):
    if i == (prefix_len - 1):
      prefix = prefix + '1'
    else:
      prefix = prefix + '0'
  return prefix

def gen_error(pre_err, mask_err, data_err, text_read):
  in_r  = ''
  l_list = text_read.split()
  l_list_len = len(l_list)

  pre_n = int((pre_err * l_list_len))
  pre_list_indices = random.sample(range(0,l_list_len), pre_n)
  for i in pre_list_indices:
    l_list[i] = err_prefix(l_list[i])

  mask_n = int((mask_err * l_list_len))
  mask_list_indices = random.sample(range(0,l_list_len), mask_n)
  for i in mask_list_indices:
    l_list[i] = err_mask(l_list[i])

  data_n = int((data_err * l_list_len))
  data_list_indices = random.sample(range(0,l_list_len), data_n)
  for i in data_list_indices:
    l_list[i] = err_age(l_list[i])

  in_r = '\n'.join(l_list)
  in_r = in_r + '\n'
  return in_r

parser = argparse.ArgumentParser(description='Duplicate routing tables')

parser.add_argument('-p', dest='pre_err', type=float, default=0.0, help='percent of prefix error')
parser.add_argument('-s', dest='mask_err', type=float, default=0.0, help='percent of subnet mask error')
parser.add_argument('-d', dest='data_err', type=float, default=0.0, help='percent of data error')
parser.add_argument('-i', dest='input', default='routes1.csv', help='input data file')
parser.add_argument('-o', dest='output', default='routes2.csv', help='output data file')

args = parser.parse_args()

input = args.input
output = args.output
pre_err = args.pre_err
mask_err = args.mask_err
data_err = args.data_err

inf = open(input, 'r')
ouf = open(output, 'w')

text_read = inf.read()

out = gen_error(pre_err, mask_err, data_err, text_read)

ouf.write(out)

inf.close()
ouf.close()  
