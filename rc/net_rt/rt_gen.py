import sys
import random
import ipaddr
import math
import argparse

parser = argparse.ArgumentParser(description='Generate routing table')

parser.add_argument('-n', dest='num_routes', type=int, default=4, help='number of routes to generate')
parser.add_argument('-c', dest='city', default='sf', help='city for which to generate a routing table')
parser.add_argument('-o', dest='output', default='routes1.csv', help='file to output routing table to')

args = parser.parse_args()

num_routes = args.num_routes
city = args.city
output = args.output

pool = []
pf = open('net_rt/pools/'+city,'r')
of = open(output, 'w')

routes_processed = 0
for l in pf:
    if(l.find('#') < 0):
      pool = l.split()

      prefix = pool[1]
      prefix_len = int(pool[2])
      shortest_prefix = int(pool[3])
      net_addr = ipaddr.IPNetwork(prefix+'/'+str(prefix_len))
      min_addr = ipaddr.IPNetwork(pool[4]+'/'+str(prefix_len))
      max_addr = ipaddr.IPNetwork(pool[5]+'/'+str(prefix_len))
      mod = int(pool[6])
      routes_counter = 0
      started_counting = False
#    print('common_net_pre: '+str(common_net_pre))
#    smallest_prefix_size = int(math.log(common_net_pre,2)) + 1
      base_addr = ipaddr.IPNetwork(prefix+'/'+str(shortest_prefix))
#    print('smallest_len: '+str(prefix_len-smallest_prefix_size))
      for addr in base_addr.iter_subnets(new_prefix=prefix_len):
        if(addr == min_addr):
          started_counting = True
        if(addr >= min_addr and addr <= max_addr and ((routes_counter % mod) == 0) and routes_processed < num_routes):
#          print(addr)
          network = addr.network
          net_mask = addr.netmask
          age = str(random.randint(0,9999))
          seq_num = str(random.randint(0,99999999))
          chk_sum = str(hex(random.randint(0,4294967295))).rstrip('L')
          of.write(str(network)+','+str(net_mask)+','+age+','+seq_num+','+chk_sum+'\n')      
          routes_processed = routes_processed + 1
        if(started_counting):
          routes_counter = routes_counter + 1

pf.close()
of.close()
